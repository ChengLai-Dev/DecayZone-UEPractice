"""Re-orient + de-junk + rescale a single-image reconstruction into a horizontal platform slab.

What it does (rigid transform + clipping only, no geometry synthesis):
  1. fit the disc plane (2 passes: PCA on all verts, then refit on the deck only)
  2. drop triangles that lie entirely on the back side of the deck (the hallucinated
     support cage / block that Pixal3D invents behind a flat object)
  3. rotate the fitted plane normal onto -Z so the visible face points up, compact vertices
  4. uniform scale so the disc diameter hits the target, recentre, sit base on z = 0
  5. rewrite the GLB, keeping TEXCOORD_0 / indices order / PNG textures byte-identical

Usage:
    python fix_platform.py --src in.glb --dst out.glb --diameter-m 6.0
"""
import argparse
import json
import struct
from pathlib import Path

import numpy as np

COMPONENT = {5120: ("i1", 1), 5121: ("u1", 1), 5122: ("i2", 2),
             5123: ("u2", 2), 5125: ("u4", 4), 5126: ("f4", 4)}
NCOMP = {"SCALAR": 1, "VEC2": 2, "VEC3": 3, "VEC4": 4, "MAT4": 16}


def parse_glb(path):
    data = Path(path).read_bytes()
    magic, version, length = struct.unpack_from("<4sII", data, 0)
    assert magic == b"glTF"
    offset = 12
    gltf = None
    bin_chunk = b""
    while offset < length:
        clen, ctype = struct.unpack_from("<II", data, offset)
        chunk = data[offset + 8: offset + 8 + clen]
        if ctype == 0x4E4F534A:
            gltf = json.loads(chunk.decode("utf-8"))
        elif ctype == 0x004E4942:
            bin_chunk = chunk
        offset += 8 + clen + ((4 - clen % 4) % 4)
    return gltf, bin_chunk


def read_accessor(gltf, bin_chunk, index):
    acc = gltf["accessors"][index]
    bv = gltf["bufferViews"][acc["bufferView"]]
    dtype, csize = COMPONENT[acc["componentType"]]
    n = NCOMP[acc["type"]]
    count = acc["count"]
    start = bv.get("byteOffset", 0) + acc.get("byteOffset", 0)
    stride = bv.get("byteStride") or (csize * n)
    np_dtype = np.dtype("<" + dtype)
    if stride == csize * n:
        arr = np.frombuffer(bin_chunk, dtype=np_dtype, count=count * n, offset=start).reshape(count, n)
    else:
        arr = np.empty((count, n), dtype=np_dtype)
        for i in range(count):
            arr[i] = np.frombuffer(bin_chunk, dtype=np_dtype, count=n, offset=start + i * stride)
    return arr


def rotation_between(a, b):
    a = a / np.linalg.norm(a)
    b = b / np.linalg.norm(b)
    v = np.cross(a, b)
    c = float(np.dot(a, b))
    if np.linalg.norm(v) < 1e-8:
        return np.eye(3) if c > 0 else -np.eye(3)
    vx = np.array([[0, -v[2], v[1]], [v[2], 0, -v[0]], [-v[1], v[0], 0]])
    return np.eye(3) + vx + vx @ vx * (1.0 / (1.0 + c))


def fit_plane(points):
    center = points.mean(axis=0)
    cov = np.cov((points - center).T)
    w, v = np.linalg.eigh(cov)
    return v[:, 0], center


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--src", required=True)
    parser.add_argument("--dst", required=True)
    parser.add_argument("--diameter-m", type=float, default=6.0)
    parser.add_argument("--thickness-m", type=float, default=0.30)
    parser.add_argument("--cut-margin", type=float, default=0.15,
                        help="cut plane = deck_back + margin * deck_thickness")
    parser.add_argument("--cut-offset-m", type=float, default=None,
                        help="absolute override: cut plane = deck_back + this (metres)")
    args = parser.parse_args()

    gltf, bin_chunk = parse_glb(args.src)
    prim = gltf["meshes"][0]["primitives"][0]
    pos = read_accessor(gltf, bin_chunk, prim["attributes"]["POSITION"]).astype(np.float64)
    nrm = read_accessor(gltf, bin_chunk, prim["attributes"]["NORMAL"]).astype(np.float64)
    tan = read_accessor(gltf, bin_chunk, prim["attributes"]["TANGENT"]).astype(np.float64)
    uv = read_accessor(gltf, bin_chunk, prim["attributes"]["TEXCOORD_0"]).astype(np.float64)
    idx = read_accessor(gltf, bin_chunk, prim["indices"]).reshape(-1).astype(np.int64)
    tri = idx.reshape(-1, 3)

    # --- pass 1: rough plane so we can measure the deck band
    n1, c1 = fit_plane(pos)
    d1 = (pos - c1) @ n1
    ordered = np.sort(d1)
    # deck is the dense slab at the low end; junk is the sparse tail
    hist, edges = np.histogram(d1, bins=200)
    dense_floor = np.percentile(d1, 0.5)
    # find the last bin above 20% of the peak count: that is the back of the deck
    peak = hist.max()
    dense_bins = np.where(hist > 0.20 * peak)[0]
    deck_back = edges[dense_bins.max() + 1]
    deck_front = edges[dense_bins.min()]
    thick = deck_back - deck_front
    if args.cut_offset_m is not None:
        cut = deck_back + args.cut_offset_m
    else:
        cut = deck_back + args.cut_margin * thick
    print("pass1 normal %s" % np.round(n1, 4))
    print("deck band %.4f .. %.4f  (thickness %.4f)  cut at %.4f" %
          (deck_front, deck_back, thick, cut))
    print("verts beyond cut: %d / %d" % ((d1 > cut).sum(), len(d1)))

    keep_tri = (d1[tri].max(axis=1) <= cut)
    print("triangles kept: %d / %d" % (keep_tri.sum(), len(tri)))

    # --- pass 2: refit the plane on the kept geometry only
    used = np.unique(tri[keep_tri])
    n2, c2 = fit_plane(pos[used])
    # empirical: the surface carrying the platform texture (rim glow, seams) faces +n1
    d2 = (pos - c2) @ n2
    print("pass2 normal %s" % np.round(n2, 4))
    print("kept extent along normal: %.4f .. %.4f" % (d2[used].min(), d2[used].max()))

    # --- rotate so the face the camera saw ends up pointing at glTF's up axis (+Y).
    # glTF is Y-up; UE converts Y-up -> Z-up on import, so authoring +Z up here would
    # make the platform stand on edge in the engine.
    UP = np.array([0.0, 1.0, 0.0])
    UP_AXIS = 1
    R = rotation_between(n2, UP)

    p = (pos - c2) @ R.T
    nv = nrm @ R.T
    tv = tan[:, :3] @ R.T

    # --- compact to referenced vertices
    remap = -np.ones(len(pos), dtype=np.int64)
    keep_idx = np.unique(tri[keep_tri])
    remap[keep_idx] = np.arange(len(keep_idx))
    tri2 = remap[tri[keep_tri]]
    p, nv, tv, uv = p[keep_idx], nv[keep_idx], tv[keep_idx], uv[keep_idx]
    print("compact verts: %d, tris: %d" % (len(p), len(tri2)))

    # --- scale so the disc diameter hits target; thickness normalised separately
    ext = p.max(axis=0) - p.min(axis=0)
    side_axes = [i for i in range(3) if i != UP_AXIS]
    diameter = 0.5 * (ext[side_axes[0]] + ext[side_axes[1]])
    s_xy = args.diameter_m / diameter
    thickness = ext[UP_AXIS]
    s_z = args.thickness_m / thickness
    print("raw extents %s (diameter %.4f, thickness %.4f)" % (np.round(ext, 4), diameter, thickness))
    print("scale side %.4f  up %.4f" % (s_xy, s_z))

    scl = np.ones(3)
    scl[side_axes] = s_xy
    scl[UP_AXIS] = s_z
    p = p * scl
    # normals: inverse-transpose of diag(scl)
    nv = nv / scl
    nv /= np.linalg.norm(nv, axis=1, keepdims=True)
    tv /= scl
    tv /= np.linalg.norm(tv, axis=1, keepdims=True)

    # --- recentre: footprint centred on 0, base on the up axis = 0
    lo, hi = p.min(axis=0), p.max(axis=0)
    for ax in side_axes:
        p[:, ax] -= 0.5 * (lo[ax] + hi[ax])
    p[:, UP_AXIS] -= lo[UP_AXIS]
    lo, hi = p.min(axis=0), p.max(axis=0)
    print("final bbox min %s max %s" % (np.round(lo, 4), np.round(hi, 4)))

    # ---------------- rebuild the GLB, textures copied verbatim
    images = gltf["images"]
    img_bytes = []
    for im in images:
        bv = gltf["bufferViews"][im["bufferView"]]
        start = bv.get("byteOffset", 0)
        img_bytes.append(bin_chunk[start:start + bv["byteLength"]])

    blob = bytearray()
    new_bvs, new_accs = [], []

    def add_view(raw, target=None):
        while len(blob) % 4:
            blob.append(0)
        offset = len(blob)
        blob.extend(raw)
        view = {"buffer": 0, "byteOffset": offset, "byteLength": len(raw)}
        if target:
            view["target"] = target
        new_bvs.append(view)
        return len(new_bvs) - 1

    def add_acc(arr, component_type, type_name, target, minmax=False):
        raw = np.ascontiguousarray(arr).astype(np.dtype("<" + COMPONENT[component_type][0])).tobytes()
        view = add_view(raw, target)
        acc = {"bufferView": view, "componentType": component_type,
               "count": int(arr.shape[0]), "type": type_name}
        if minmax:
            acc["min"] = [float(v) for v in arr.min(axis=0)]
            acc["max"] = [float(v) for v in arr.max(axis=0)]
        new_accs.append(acc)
        return len(new_accs) - 1

    pos_acc = add_acc(p.astype(np.float32), 5126, "VEC3", 34962, minmax=True)
    nrm_acc = add_acc(nv.astype(np.float32), 5126, "VEC3", 34962)
    tan_acc = add_acc(np.hstack([tv, tan[keep_idx][:, 3:4]]).astype(np.float32), 5126, "VEC4", 34962)
    uv_acc = add_acc(uv.astype(np.float32), 5126, "VEC2", 34962)
    idx_acc = add_acc(tri2.reshape(-1).astype(np.uint32), 5125, "SCALAR", 34963)

    img_views = []
    for raw in img_bytes:
        img_views.append(add_view(raw))

    while len(blob) % 4:
        blob.append(0)

    out = {
        "asset": {"version": "2.0", "generator": "DecayZone fix_platform (transform+clip only)"},
        "buffers": [{"byteLength": len(blob)}],
        "bufferViews": new_bvs,
        "accessors": new_accs,
        "meshes": [{
            "name": "SM_ExtractionPlatform_01",
            "primitives": [{
                "attributes": {"POSITION": pos_acc, "NORMAL": nrm_acc,
                               "TANGENT": tan_acc, "TEXCOORD_0": uv_acc},
                "indices": idx_acc,
                "material": 0,
                "mode": 4,
            }],
        }],
        "nodes": [{"mesh": 0, "name": "SM_ExtractionPlatform_01"}],
        "scenes": [{"nodes": [0]}],
        "scene": 0,
        "images": [{"bufferView": v, "mimeType": images[i]["mimeType"]}
                   for i, v in enumerate(img_views)],
        "samplers": gltf.get("samplers", [{}]),
        "textures": gltf["textures"],
        "materials": gltf["materials"],
    }

    json_bytes = json.dumps(out, ensure_ascii=False).encode("utf-8")
    while len(json_bytes) % 4:
        json_bytes += b" "
    total = 12 + 8 + len(json_bytes) + 8 + len(blob)
    out_bytes = bytearray()
    out_bytes += struct.pack("<4sII", b"glTF", 2, total)
    out_bytes += struct.pack("<II", len(json_bytes), 0x4E4F534A) + json_bytes
    out_bytes += struct.pack("<II", len(blob), 0x004E4942) + bytes(blob)

    Path(args.dst).parent.mkdir(parents=True, exist_ok=True)
    Path(args.dst).write_bytes(out_bytes)
    print("written %s (%.2f MB)" % (args.dst, len(out_bytes) / 1048576))


if __name__ == "__main__":
    main()
