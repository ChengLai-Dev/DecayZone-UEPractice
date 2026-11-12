# 素材清单

每个运行期素材一行。技术规格以 `docs/contracts/assets.md` 为准（当前该文件尚不存在，见 `STATE.md` 未决项）。

## Props / ExtractionPlatform

| 资产 | 路径 | 类型 | 尺寸（cm） | 三角面 | 材质 | 贴图 | 状态 |
|---|---|---|---|---|---|---|---|
| SM_ExtractionPlatform_01 | `/Game/Assets/Props/ExtractionPlatform/` | 静态网格 | 600.0 × 599.8 × 30.0 | 631,656（Nanite） | M_ExtractionPlatform_01 | T_ExtractionPlatform_01_BC / _N / _ORM，4096² | 可进引擎，占位级 |
| M_ExtractionPlatform_01 | 同上 | 材质 | — | — | — | 同上 | 程序化搭建，未在编辑器内目视验收 |
| T_ExtractionPlatform_01_BC | 同上 | 贴图 | 4096×4096 | — | — | sRGB，TC_Default | 可用（图集有明显的虚构背面区域） |
| T_ExtractionPlatform_01_ORM | 同上 | 贴图 | 4096×4096 | — | — | 线性，TC_Masks | 可用 |
| T_ExtractionPlatform_01_N | 同上 | 贴图 | 4096×4096 | — | — | 线性，TC_Normalmap | 可用 |

### 元信息（程序接入用）

| 项 | 取值 |
|---|---|
| 资源类型 | 静态网格，无动画、无骨骼、无 LOD、无碰撞 |
| 朝向基准 | 正面朝 +Z（UE 世界坐标）。水平方向无朝向基准：几何与贴图绕 Z 轴旋转对称，任意 Yaw 放置均成立 |
| 锚点 | 几何原点位于圆盘底面中心；平台最低点 Z = 0，可直接贴地放置 |
| 单帧尺寸 | 直径 600cm，厚 30cm（非方形贴图资产，无帧概念） |
| 帧数 / 帧率 / 帧序 | 不适用（静态件） |
| UV | 单套 UV0，单材质槽（槽名 `Platform`） |
| 碰撞 | 无。需要站立/阻挡时由程序侧加 Box 或 Auto Convex |

### 生产可复现信息

| 步骤 | 手段 | 参数 |
|---|---|---|
| 概念图 | FLUX 3 Klein（`flux3-image-klein-int8-convrot`） | 16 步 / CFG 1.0 / euler / simple / alpha=16 / 1024² / seed 7702 |
| 图生 3D | Pixal3D 单图管线（`3d_pixal3d_trellis2_image_to_model.json`） | `pixal3d_int8_convrot` + `trellis_2_int8_convrot`，seed 7702，约 193s，目标面数 70 万，产出 GLB 42MB |
| 几何修正 | `tools/fix_platform.py`（纯 CPU，约 20s） | PCA 平面拟合 → 裁掉盘面背侧全部三角形 → 旋转到 +Y 上 → 缩放至直径 6.0m / 厚 0.30m → 底面落 0 |
| 导入 UE | `tools/ue_build_platform.py`（无头 commandlet） | glTF 导入 → 贴图按用途重命名 → 建材质 → 赋材质 → 开 Nanite → 保存 |

提示词（最终采用的那一张）：

```
a single circular industrial metal platform, a thick round steel deck with radial panel
seams and bolted plating, dark weathered gunmetal, a glowing teal-cyan light strip running
all the way around the outer rim, chamfered edge, slightly elevated three-quarter view,
near orthographic perspective, single game asset, product shot, isolated on plain white
background, even lighting, no shadow, full object in frame with margin around it, high detail
```

源文件（概念图、原始重建 GLB、成品 GLB、贴图、脚本）在 `Content/Assets/_Source/ExtractionPlatform/`。
