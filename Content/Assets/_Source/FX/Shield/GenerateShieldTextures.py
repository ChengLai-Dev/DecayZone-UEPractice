"""生成护盾材质用的贴图。

产出（与脚本同目录）：
  T_Shield_HexGrid.png   六边形能量网格，白线黑底，按等距柱状铺满一次不重复
  T_Shield_Noise.png     分形值噪声，可平铺，用于破盾溶解

用法：python GenerateShieldTextures.py
"""

import math
import os

import numpy as np
from PIL import Image, ImageDraw

OUT_DIR = os.path.dirname(os.path.abspath(__file__))

HEX_SIZE = 1024
HEX_SUPERSAMPLE = 4
HEX_COLUMNS = 16
HEX_LINE_PX = 3.0

NOISE_SIZE = 1024
NOISE_OCTAVES = 5
NOISE_BASE_CELL = 8
NOISE_FLOOR = 0.15


def build_hex_grid() -> Image.Image:
    """画一张按等距柱状铺一次的六边形网格，非平铺，接缝不参与计算。"""
    size = HEX_SIZE * HEX_SUPERSAMPLE
    image = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(image)

    radius = HEX_SIZE / (math.sqrt(3.0) * HEX_COLUMNS)
    row_step = 1.5 * radius
    col_step = math.sqrt(3.0) * radius
    row_count = int(HEX_SIZE / row_step) + 2

    # 六边形顶点（pointy-top，顶点朝上）
    corner_offsets = []
    for i in range(6):
        angle = math.radians(60.0 * i - 90.0)
        corner_offsets.append((radius * math.cos(angle), radius * math.sin(angle)))

    line_width = HEX_LINE_PX * HEX_SUPERSAMPLE

    for row in range(row_count):
        for col in range(HEX_COLUMNS + 1):
            center_x = col * col_step
            if row % 2 == 1:
                center_x += col_step * 0.5
            center_y = row * row_step
            points = [
                ((center_x + dx) * HEX_SUPERSAMPLE, (center_y + dy) * HEX_SUPERSAMPLE)
                for dx, dy in corner_offsets
            ]
            draw.line(points + [points[0]], fill=255, width=int(line_width), joint="curve")

    return image.resize((HEX_SIZE, HEX_SIZE), Image.LANCZOS)


def value_noise(size: int, cell: int, rng: np.random.Generator) -> np.ndarray:
    """一圈可平铺的平滑值噪声，返回 0..1。"""
    grid = rng.random((cell, cell)).astype(np.float32)

    axis = np.arange(size, dtype=np.float32) * (cell / size)
    x0 = np.floor(axis).astype(np.int32) % cell
    y0 = np.floor(axis).astype(np.int32) % cell
    fx = axis - np.floor(axis)
    fy = axis - np.floor(axis)
    # 五次平滑插值，避免出现网格状折痕
    sx = fx * fx * fx * (fx * (fx * 6.0 - 15.0) + 10.0)
    sy = fy * fy * fy * (fy * (fy * 6.0 - 15.0) + 10.0)

    grid_next_x = np.roll(grid, -1, axis=1)
    grid_next_y = np.roll(grid, -1, axis=0)
    grid_next_xy = np.roll(grid_next_x, -1, axis=0)

    top = grid[np.ix_(y0, x0)] * (1.0 - sx)[None, :] + grid_next_x[np.ix_(y0, x0)] * sx[None, :]
    bottom = grid_next_y[np.ix_(y0, x0)] * (1.0 - sx)[None, :] + grid_next_xy[np.ix_(y0, x0)] * sx[None, :]
    return np.clip(top * (1.0 - sy)[:, None] + bottom * sy[:, None], 0.0, 1.0)


def build_noise() -> Image.Image:
    """分形噪声：多层不同尺度的可平铺值噪声相加。"""
    rng = np.random.default_rng(20261115)
    total = np.zeros((NOISE_SIZE, NOISE_SIZE), dtype=np.float32)
    amplitude = 1.0
    amplitude_sum = 0.0
    cell = NOISE_BASE_CELL

    for _ in range(NOISE_OCTAVES):
        total += value_noise(NOISE_SIZE, cell, rng) * amplitude
        amplitude_sum += amplitude
        amplitude *= 0.5
        cell *= 2

    total /= amplitude_sum
    # 拉开对比度，让溶解阈值更好看
    total = np.clip((total - 0.5) * 1.6 + 0.5, 0.0, 1.0)
    # 下限抬到 0.15：材质里 Dissolve=0 时用 noise * 12 做不透明遮罩，
    # 噪声若压到 0 会让壳在未溶解时出现透明破洞
    total = NOISE_FLOOR + (1.0 - NOISE_FLOOR) * total
    return Image.fromarray((total * 255.0).astype(np.uint8), mode="L")


def main() -> None:
    hex_grid = build_hex_grid()
    hex_path = os.path.join(OUT_DIR, "T_Shield_HexGrid.png")
    hex_grid.save(hex_path)

    noise = build_noise()
    noise_path = os.path.join(OUT_DIR, "T_Shield_Noise.png")
    noise.save(noise_path)

    print(f"hex grid  -> {hex_path}  ({hex_grid.size[0]}x{hex_grid.size[1]})")
    print(f"noise     -> {noise_path}  ({noise.size[0]}x{noise.size[1]})")


if __name__ == "__main__":
    main()
