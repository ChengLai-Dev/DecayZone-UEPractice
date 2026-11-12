"""构建 SM_ExtractionPlatform_01：直径 600cm、厚 30cm 的圆形工业平台。

在 Blender 5.2 里无头运行：
  blender.exe -b --factory-startup --python build_platform.py -- <texture_dir> <out_dir>

几何（单位 m，导出 glb 时 UE 会按 cm 解释）：
  顶面 z=0.30 / 底面 z=0.00 / 外径 6.0 / 顶外缘 2cm 倒角
  顶面 r 2.80–2.92 处一圈发光环（独立材质槽，抬高 1mm 防 z-fighting）
UV 图集布局见 make_textures.py 顶部注释。
"""
import math
import os
import sys

import bpy

N = 96              # 圆周分段
R = 3.0             # 外半径 m
H = 0.30            # 厚度 m
CHAMFER = 0.018     # 顶外缘倒角 m
R_TOP = R - CHAMFER
RIM_IN = 2.80
RIM_OUT = 2.92
RIM_Z = H + 0.001

DECK_U = (0.02, 0.74)
DECK_V = (0.14, 0.86)
SIDE_U = (0.76, 0.98)
SIDE_V = (0.02, 0.10)
CHAM_U = (0.76, 0.98)
CHAM_V = (0.12, 0.18)
REPEAT = 4.0        # 侧壁/倒角沿周向重复次数


def circ(i, n=N, r=R, z=0.0):
    a = 2.0 * math.pi * i / n
    return (r * math.cos(a), r * math.sin(a), z)


class Builder:
    def __init__(self):
        self.verts = []
        self.faces = []
        self.uvs = []        # 每个 face 的 uv 列表
        self.mats = []
        self.smooth = []

    def add_ring(self, r, z):
        start = len(self.verts)
        for i in range(N):
            self.verts.append(circ(i, N, r, z))
        return start

    def add_point(self, xyz):
        self.verts.append(tuple(xyz))
        return len(self.verts) - 1

    def face(self, idx, uv, mat=0, smooth=False):
        self.faces.append(tuple(idx))
        self.uvs.append(uv)
        self.mats.append(mat)
        self.smooth.append(smooth)


def uv_deck(x, y):
    u = DECK_U[0] + (DECK_U[1] - DECK_U[0]) * (x / (2 * R) + 0.5)
    v = DECK_V[0] + (DECK_V[1] - DECK_V[0]) * (y / (2 * R) + 0.5)
    return (u, v)


def uv_band(i, n, u_range, v_top, v_bot, repeat=REPEAT):
    t = (i / n * repeat) % 1.0
    u = u_range[0] + (u_range[1] - u_range[0]) * t
    return u, v_top, v_bot


def build_mesh():
    b = Builder()

    # --- 顶面 ---
    center = b.add_point((0.0, 0.0, H))
    top = b.add_ring(R_TOP, H)
    for i in range(N):
        j = (i + 1) % N
        b.face((center, top + i, top + j),
               (uv_deck(0, 0), uv_deck(*b.verts[top + i][:2]), uv_deck(*b.verts[top + j][:2])))

    # --- 顶外缘倒角 ---
    cham = b.add_ring(R, H - CHAMFER)
    for i in range(N):
        j = (i + 1) % N
        u0, vt, vb = uv_band(i, N, CHAM_U, CHAM_V[1], CHAM_V[0])
        u1, _, _ = uv_band(j, N, CHAM_U, CHAM_V[1], CHAM_V[0])
        b.face((top + i, cham + i, cham + j, top + j),
               ((u0, vt), (u0, vb), (u1, vb), (u1, vt)), smooth=True)

    # --- 侧壁 ---
    bot = b.add_ring(R, 0.0)
    for i in range(N):
        j = (i + 1) % N
        u0, vt, vb = uv_band(i, N, SIDE_U, SIDE_V[1], SIDE_V[0])
        u1, _, _ = uv_band(j, N, SIDE_U, SIDE_V[1], SIDE_V[0])
        b.face((cham + i, bot + i, bot + j, cham + j),
               ((u0, vt), (u0, vb), (u1, vb), (u1, vt)), smooth=True)

    # --- 底面 ---
    bcenter = b.add_point((0.0, 0.0, 0.0))
    for i in range(N):
        j = (i + 1) % N
        b.face((bcenter, bot + j, bot + i),
               (uv_deck(0, 0), uv_deck(*b.verts[bot + j][:2]), uv_deck(*b.verts[bot + i][:2])))

    # --- 发光环（材质槽 1） ---
    rin = b.add_ring(RIM_IN, RIM_Z)
    rout = b.add_ring(RIM_OUT, RIM_Z)
    for i in range(N):
        j = (i + 1) % N
        u0 = 0.90 + 0.09 * ((i / N * REPEAT) % 1.0)
        u1 = 0.90 + 0.09 * ((j / N * REPEAT) % 1.0)
        b.face((rin + i, rout + i, rout + j, rin + j),
               ((u0, 0.2), (u0, 0.8), (u1, 0.8), (u1, 0.2)), mat=1)

    mesh = bpy.data.meshes.new("SM_ExtractionPlatform_01")
    mesh.from_pydata(b.verts, [], b.faces)
    mesh.update()

    uv_layer = mesh.uv_layers.new(name="UVMap")
    for poly_idx, poly in enumerate(mesh.polygons):
        poly.material_index = b.mats[poly_idx]
        poly.use_smooth = b.smooth[poly_idx]
        uvs = b.uvs[poly_idx]
        for k, loop_idx in enumerate(poly.loop_indices):
            uv_layer.data[loop_idx].uv = uvs[k]

    mesh.validate()
    mesh.update()
    return mesh


def image_mat(name, tex_dir):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    nt = mat.node_tree
    nt.nodes.clear()

    out = nt.nodes.new("ShaderNodeOutputMaterial")
    bsdf = nt.nodes.new("ShaderNodeBsdfPrincipled")
    out.location = (600, 0)
    bsdf.location = (260, 0)
    nt.links.new(bsdf.outputs["BSDF"], out.inputs["Surface"])

    def tex(fname, non_color):
        img = bpy.data.images.load(os.path.join(tex_dir, fname), check_existing=True)
        img.name = os.path.splitext(fname)[0]
        node = nt.nodes.new("ShaderNodeTexImage")
        node.image = img
        node.location = (-400, 0)
        if non_color:
            img.colorspace_settings.name = "Non-Color"
        return node

    bc = tex("T_ExtractionPlatform_01_BC.png", False)
    nt.links.new(bc.outputs["Color"], bsdf.inputs["Base Color"])

    nrm = tex("T_ExtractionPlatform_01_N.png", True)
    nmap = nt.nodes.new("ShaderNodeNormalMap")
    nmap.location = (-100, -320)
    nt.links.new(nrm.outputs["Color"], nmap.inputs["Color"])
    nt.links.new(nmap.outputs["Normal"], bsdf.inputs["Normal"])

    orm = tex("T_ExtractionPlatform_01_ORM.png", True)
    sep = nt.nodes.new("ShaderNodeSeparateColor")
    sep.location = (-100, -620)
    nt.links.new(orm.outputs["Color"], sep.inputs["Color"])
    nt.links.new(sep.outputs["Green"], bsdf.inputs["Roughness"])
    nt.links.new(sep.outputs["Blue"], bsdf.inputs["Metallic"])
    return mat


def rim_mat():
    mat = bpy.data.materials.new("M_ExtractionPlatform_01_Rim")
    mat.use_nodes = True
    nt = mat.node_tree
    bsdf = nt.nodes.get("Principled BSDF")
    glow = (0.35, 0.95, 0.88, 1.0)
    bsdf.inputs["Base Color"].default_value = (0.05, 0.20, 0.19, 1.0)
    bsdf.inputs["Metallic"].default_value = 0.0
    bsdf.inputs["Roughness"].default_value = 0.45
    bsdf.inputs["Emission Color"].default_value = glow
    bsdf.inputs["Emission Strength"].default_value = 1.0
    return mat


def main():
    argv = sys.argv[sys.argv.index("--") + 1:]
    tex_dir, out_dir = argv[0], argv[1]
    os.makedirs(out_dir, exist_ok=True)

    bpy.ops.wm.read_factory_settings(use_empty=True)

    mesh = build_mesh()
    obj = bpy.data.objects.new("SM_ExtractionPlatform_01", mesh)
    bpy.context.collection.objects.link(obj)
    mesh.materials.append(image_mat("M_ExtractionPlatform_01", tex_dir))
    mesh.materials.append(rim_mat())
    for mat in mesh.materials:
        mat.use_backface_culling = True

    obj.location = (0.0, 0.0, 0.0)

    glb = os.path.join(out_dir, "SM_ExtractionPlatform_01.glb")
    bpy.ops.export_scene.gltf(
        filepath=glb,
        export_format="GLB",
        use_selection=False,
        export_apply=True,
        export_yup=True,
        export_materials="EXPORT",
    )

    blend = os.path.join(out_dir, "SM_ExtractionPlatform_01.blend")
    bpy.ops.wm.save_as_mainfile(filepath=blend)

    print("MESH verts=%d faces=%d" % (len(mesh.vertices), len(mesh.polygons)))
    print("GLB %s (%.2f MB)" % (glb, os.path.getsize(glb) / 1024 / 1024))
    print("BLEND %s" % blend)


main()
