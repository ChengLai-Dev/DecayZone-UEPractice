#!/usr/bin/env python
"""ExtractionPlatform 贴图图集生成（程序化 + 可选 AI 污渍层）。

UV 图集约定（与 build_platform.py 完全一致，单位 0..1，v 向上）：
    DECK     u 0.02..0.74   v 0.14..0.86   顶面（俯视平面投影，圆内切）
    SIDE     u 0.76..0.98   v 0.02..0.10   侧壁（沿周向 4 次重复）
    CHAMFER  u 0.76..0.98   v 0.12..0.18   顶外缘倒角（沿周向 4 次重复）
    RIM      未使用（发光环用独立无贴图材质）

产出：
    T_ExtractionPlatform_01_BC.png    sRGB 基础色
    T_ExtractionPlatform_01_N.png     OpenGL 约定法线
    T_ExtractionPlatform_01_ORM.png   R=AO G=Roughness B=Metallic
"""
import argparse
import os

import numpy as np
from PIL import Image, ImageFilter

SIZE = 1024

DECK = (0.02, 0.74, 0.14, 0.86)
SIDE = (0.76, 0.98, 0.02, 0.10)
CHAMFER = (0.76, 0.98, 0.12, 0.18)

STEEL = np.array([0.340, 0.356, 0.382], dtype=np.float32)
STEEL_DARK = np.array([0.130, 0.138, 0.152], dtype=np.float32)
HAZARD = np.array([0.720, 0.550, 0.160], dtype=np.float32)
RUST = np.array([0.380, 0.220, 0.130], dtype=np.float32)


def px_box(region):
    """region -> (x0, x1, y0, y1) 像素索引（v 向上，数组行向下）。"""
    u0, u1, v0, v1 = region
    x0, x1 = int(round(u0 * SIZE)), int(round(u1 * SIZE))
    y0, y1 = int(round((1.0 - v1) * SIZE)), int(round((1.0 - v0) * SIZE))
    return x0, x1, y0, y1


def noise2d(h, w, cells, seed):
    """平滑值噪声：小网格双三次放大，返回 0..1。"""
    rng = np.random.default_rng(seed)
    small = rng.random((max(2, cells), max(2, cells))).astype(np.float32)
    img = Image.fromarray((small * 255).astype(np.uint8), mode="L")
    img = img.resize((w, h), Image.BICUBIC)
    return np.asarray(img, dtype=np.float32) / 255.0


def _box1d(a, k, axis):
    pad = [(0, 0)] * a.ndim
    pad[axis] = (k // 2, k - 1 - k // 2)
    p = np.pad(a, pad, mode="edge")
    cs = np.cumsum(p, axis=axis)
    zeros = list(cs.shape)
    zeros[axis] = 1
    cs = np.concatenate([np.zeros(zeros, dtype=cs.dtype), cs], axis=axis)
    n = a.shape[axis]
    hi = [slice(None)] * cs.ndim
    lo = [slice(None)] * cs.ndim
    hi[axis] = slice(k, k + n)
    lo[axis] = slice(0, n)
    return (cs[tuple(hi)] - cs[tuple(lo)]) / float(k)


def blur(arr, radius):
    """可分离盒式模糊（PIL 的 F 模式不支持 GaussianBlur）。"""
    k = max(1, int(radius) * 2 + 1)
    out = np.asarray(arr, dtype=np.float32)
    out = _box1d(out, k, 0)
    out = _box1d(out, k, 1)
    return out


def paste(dst, patch, region):
    x0, x1, y0, y1 = px_box(region)
    dst[y0:y1, x0:x1] = patch


def build_deck(h, w, grunge=None):
    """返回 deck 的 albedo / height / rough / metal / ao（各自同尺寸）。"""
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    # 归一化到 -1..1，圆内切于短边
    nx = (xx + 0.5) / w * 2.0 - 1.0
    ny = 1.0 - (yy + 0.5) / h * 2.0
    rr = np.sqrt(nx * nx + ny * ny)
    ang = np.arctan2(ny, nx)  # -pi..pi

    albedo = np.broadcast_to(STEEL, (h, w, 3)).copy()
    height = np.full((h, w), 0.55, dtype=np.float32)
    rough = np.full((h, w), 0.62, dtype=np.float32)
    metal = np.full((h, w), 0.90, dtype=np.float32)
    ao = np.full((h, w), 1.0, dtype=np.float32)

    inside = rr <= 1.0
    px_per_metre = w / 6.0  # deck 图集覆盖 600cm

    def ring_line(radius, width_cm):
        # rr 是归一化半径（1.0 = 300cm），宽度必须换算成同一单位
        half = (width_cm / 300.0) * 0.5
        return np.abs(rr - radius) < half

    # 大尺度色差
    stain = noise2d(h, w, 5, 11)
    albedo *= (0.90 + 0.20 * stain)[..., None]
    rough += (stain - 0.5) * 0.12

    # 一圈同心凹槽
    for radius, wcm in ((0.10, 1.2), (0.42, 1.6), (0.70, 1.6), (0.865, 1.2)):
        m = ring_line(radius, wcm)
        albedo[m] = albedo[m] * 0.55 + STEEL_DARK * 0.45
        height[m] -= 0.22
        rough[m] = 0.82
        ao[m] = 0.62
    # 凹槽两侧提亮（避免一眼平）
    for radius, wcm in ((0.10, 1.2), (0.42, 1.6), (0.70, 1.6), (0.865, 1.2)):
        m = ring_line(radius, wcm * 2.6) & ~ring_line(radius, wcm * 1.2)
        albedo[m] *= 1.06

    # 12 条径向拼缝（0.10 -> 0.70）
    adeg = (np.degrees(ang) + 360.0) % 360.0
    sector = 30.0
    dist_to_seam = np.minimum(adeg % sector, sector - (adeg % sector))
    seam_w = 0.55  # 度
    seam = (dist_to_seam < seam_w) & (rr > 0.10) & (rr < 0.705)
    albedo[seam] = albedo[seam] * 0.5 + STEEL_DARK * 0.5
    height[seam] -= 0.18
    ao[seam] = 0.7
    rough[seam] = 0.8

    # 危险警示环（磨损黄黑段）
    band = (rr > 0.745) & (rr < 0.855)
    seg = (adeg // 15.0).astype(np.int32) % 2 == 0
    wear = noise2d(h, w, 60, 23)
    paint = band & seg & (wear > 0.16)
    albedo[paint] = HAZARD * (0.85 + 0.28 * wear[paint])[..., None]
    rough[paint] = 0.58
    metal[paint] = 0.05
    height[paint] += 0.05

    # 铆钉：rr=0.66 每 15 度，一粒约 4cm
    rivets = np.zeros((h, w), dtype=bool)
    angs = np.radians((np.arange(24) * 15.0 + 7.5))
    for a0 in angs:
        cx, cy = 0.66 * np.cos(a0), 0.66 * np.sin(a0)
        d_cm = np.sqrt((nx - cx) ** 2 + (ny - cy) ** 2) * 300.0
        rivets |= d_cm < 2.0
    albedo[rivets] = np.array([0.430, 0.445, 0.470])
    height[rivets] += 0.18
    rough[rivets] = 0.36

    # 锈迹：沿凹槽与警示带边缘渗色（不覆盖油漆）
    rustmask = (band | seam) & ~paint
    rust_strength = noise2d(h, w, 40, 77) * rustmask
    rust_amt = np.clip((rust_strength - 0.66) * 2.4, 0, 0.45)
    albedo = albedo * (1 - rust_amt[..., None]) + RUST * rust_amt[..., None]
    metal *= (1 - rust_amt * 0.9)
    rough = np.clip(rough + rust_amt * 0.5, 0, 1)

    # 边缘 AO
    edge_ao = np.clip((1.0 - rr) / 0.10, 0, 1)
    ao *= (0.55 + 0.45 * edge_ao)

    # 细划痕（周向）
    scratch = noise2d(h, w, 160, 5)
    scratch = np.clip((scratch - 0.72) * 3.0, 0, 1) * inside
    albedo = np.clip(albedo + scratch[..., None] * 0.10, 0, 1)
    rough = np.clip(rough - scratch * 0.18, 0, 1)
    height += scratch * 0.01

    # AI 污渍层（可选）
    if grunge is not None:
        g = grunge
        if g.shape[0] != h or g.shape[1] != w:
            g = np.asarray(Image.fromarray((g * 255).astype(np.uint8), "L").resize((w, h), Image.BICUBIC),
                           dtype=np.float32) / 255.0
        g = blur(g, 1.0)
        detail = 1.0 + (g - float(g.mean())) * 0.22
        albedo = np.clip(albedo * detail[..., None], 0, 1)
        rough = np.clip(rough + (g - float(g.mean())) * 0.20, 0, 1)
        height += (g - 0.5) * 0.03

    # 圆外：铺基础钢色（避免 mip 边缘渗色）
    outside = ~inside
    albedo[outside] = STEEL * 0.9
    height[outside] = 0.5
    rough[outside] = 0.7
    metal[outside] = 0.9
    ao[outside] = 1.0

    return albedo, height, rough, metal, ao


def build_strip(h, w, horizontal_repeats=4, grunge=None):
    """侧壁/倒角带：沿 u 重复的磨损条纹。"""
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    u = (xx + 0.5) / w * horizontal_repeats
    v = (yy + 0.5) / h

    albedo = np.broadcast_to(STEEL * 0.88, (h, w, 3)).copy()
    height = np.full((h, w), 0.5, dtype=np.float32)
    rough = np.full((h, w), 0.66, dtype=np.float32)
    metal = np.full((h, w), 0.92, dtype=np.float32)
    ao = np.full((h, w), 0.92, dtype=np.float32)

    # 横向拉丝条纹（u 方向周期性，天然可平铺）
    streak = 0.5 + 0.5 * np.sin(u * 2 * np.pi * 9.0) * 0.35 \
        + 0.5 * np.sin(u * 2 * np.pi * 23.0 + 1.7) * 0.22
    streak = np.clip(streak, 0, 1)
    albedo *= (0.88 + 0.22 * streak)[..., None]
    rough = np.clip(rough + (streak - 0.5) * 0.20, 0, 1)

    # 上下暗边（接地与顶部接触阴影）
    ao *= (0.62 + 0.38 * np.clip(v / 0.18, 0, 1)) * (0.62 + 0.38 * np.clip((1 - v) / 0.18, 0, 1))

    # 竖直拼缝
    seam = (u % 1.0) < 0.006
    albedo[seam] = STEEL_DARK
    height[seam] -= 0.15
    ao[seam] *= 0.65

    if grunge is not None:
        g = grunge
        if g.shape[0] != h or g.shape[1] != w:
            g = np.asarray(Image.fromarray((g * 255).astype(np.uint8), "L").resize((w, h), Image.BICUBIC),
                           dtype=np.float32) / 255.0
        albedo = np.clip(albedo * (1.0 + (g - float(g.mean())) * 0.34)[..., None], 0, 1)
        rough = np.clip(rough + (g - float(g.mean())) * 0.20, 0, 1)

    return albedo, height, rough, metal, ao


def normals_from_height(height, strength):
    gx = np.gradient(height, axis=1)
    gy = np.gradient(height, axis=0)
    nx = -gx * strength
    ny = gy * strength        # 行向下 = +Y 屏幕向下，OpenGL 约定 +Y 向上 -> 取反
    nz = np.ones_like(height)
    ln = np.sqrt(nx * nx + ny * ny + nz * nz)
    n = np.stack([nx / ln, ny / ln, nz / ln], axis=-1)
    return n * 0.5 + 0.5


def load_grunge(path, crop=0.78):
    if not path:
        return None
    img = Image.open(path).convert("L")
    w, h = img.size
    cw, ch = int(w * crop), int(h * crop)
    left, top = (w - cw) // 2, (h - ch) // 2
    img = img.crop((left, top, left + cw, top + ch))
    return np.asarray(img, dtype=np.float32) / 255.0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True)
    ap.add_argument("--grunge", default="")
    args = ap.parse_args()
    os.makedirs(args.out, exist_ok=True)

    grunge = load_grunge(args.grunge)
    # deck 图集是 737px 宽，取 grunge 的中心裁剪
    deck_grunge = None
    if grunge is not None:
        s = SIZE
        gs = grunge.shape[0]
        if gs > s:
            o = (gs - s) // 2
            deck_grunge = grunge[o:o + s, o:o + s]
        else:
            deck_grunge = grunge

    dx0, dx1, dy0, dy1 = px_box(DECK)
    dh, dw = dy1 - dy0, dx1 - dx0
    d_albedo, d_height, d_rough, d_metal, d_ao = build_deck(dh, dw, deck_grunge)

    sx0, sx1, sy0, sy1 = px_box(SIDE)
    s_albedo, s_height, s_rough, s_metal, s_ao = build_strip(sy1 - sy0, sx1 - sx0, 6, grunge)

    cx0, cx1, cy0, cy1 = px_box(CHAMFER)
    c_albedo, c_height, c_rough, c_metal, c_ao = build_strip(cy1 - cy0, cx1 - cx0, 6, grunge)

    albedo = np.broadcast_to(STEEL * 0.75, (SIZE, SIZE, 3)).copy()
    height = np.full((SIZE, SIZE), 0.5, dtype=np.float32)
    rough = np.full((SIZE, SIZE), 0.7, dtype=np.float32)
    metal = np.full((SIZE, SIZE), 0.9, dtype=np.float32)
    ao = np.ones((SIZE, SIZE), dtype=np.float32)

    paste(albedo, d_albedo, DECK)
    paste(height, d_height, DECK)
    paste(rough, d_rough, DECK)
    paste(metal, d_metal, DECK)
    paste(ao, d_ao, DECK)

    paste(albedo, s_albedo, SIDE)
    paste(height, s_height, SIDE)
    paste(rough, s_rough, SIDE)
    paste(metal, s_metal, SIDE)
    paste(ao, s_ao, SIDE)

    paste(albedo, c_albedo, CHAMFER)
    paste(height, c_height, CHAMFER)
    paste(rough, c_rough, CHAMFER)
    paste(metal, c_metal, CHAMFER)
    paste(ao, c_ao, CHAMFER)

    nrm = normals_from_height(height, 26.0)

    def save(arr, name, mode):
        data = (np.clip(arr, 0, 1) * 255.0 + 0.5).astype(np.uint8)
        im = Image.fromarray(data, mode)
        im.save(os.path.join(args.out, name))

    save(albedo, "T_ExtractionPlatform_01_BC.png", "RGB")
    save(nrm, "T_ExtractionPlatform_01_N.png", "RGB")

    orm = np.stack([ao, rough, metal], axis=-1)
    orm_rgba = np.concatenate([orm, np.ones((SIZE, SIZE, 1), dtype=np.float32)], axis=-1)
    save(orm_rgba, "T_ExtractionPlatform_01_ORM.png", "RGBA")

    print("BC 底色均值 %.3f 法线均值 %.3f 粗糙度均值 %.3f 金属度均值 %.3f"
          % (albedo.mean(), nrm.mean(), rough.mean(), metal.mean()))
    print("输出目录:", args.out)


if __name__ == "__main__":
    main()
