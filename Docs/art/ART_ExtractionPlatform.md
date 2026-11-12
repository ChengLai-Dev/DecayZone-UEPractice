# 撤离点平台 SM_ExtractionPlatform_01 · 制作与交付说明

日期：2026-11-12　产出：artist　状态：**已完成，可直接进图**

## 1. 一句话

直径 600cm、厚 30cm 的圆形工业平台，顶面平整可站，外缘一圈青绿发光环。原点在**底面中心**，放进场景摆在 z=0 即可，顶面自动落在 30cm。

## 2. 生成方式（含参数，可复现）

**没有走图生 3D。** 判定依据：本件是「扁而大」的规则形体，Pixal3D 单图路线在深度方向会把薄件挤出数倍厚度、且顶面不可能保证平整（`game-art-model` 技能已知缺陷 3/4：深度场尺度失准、深度方向漂移）。这类形体用程序化几何是唯一能保证「不走样」的做法。

| 环节 | 手段 | 具体参数 |
|---|---|---|
| 几何 | Blender 5.2.1 程序化建模 | `_Source/ExtractionPlatform/build_platform.py`；外径 3.0m、厚 0.30m、顶外缘倒角 1.8cm、圆周 96 段、发光环 r 2.80–2.92m 抬高 1mm |
| 贴图主体 | numpy 程序化绘制 UV 图集 | `_Source/ExtractionPlatform/make_textures.py`；同心凹槽 4 道、径向拼缝 12 条、警示环 15° 交替、铆钉 24 粒 |
| 污渍细节层 | 本地 FLUX 3 Klein（1024²，16 步，CFG 1.0，euler/simple，seed **88991**） | 提示词：`top down flat view of a worn steel metal plate surface, dark grey industrial steel, scratches, oil stains and light rust, even diffuse lighting, no shadows, no highlights, no panel lines, no objects, no text, seamless texture`；只取中央 78% 内容区，去均值后以 ×0.22 权重叠进 BC 与粗糙度 |
| 法线 | 高度场 Sobel 求导（OpenGL 约定） | 梯度强度 26 |
| 导入 | UE 5.8 无头 commandlet | `_Source/ExtractionPlatform/ue_import_platform.py`（只操作目标目录，不清 `/Game/Assets`） |

原始 FLUX 出图留在 `_Source/ExtractionPlatform/raw_flux_grunge/plate_b_FLUX_seed88991.png`，可重出对照。

## 3. 导入结果（脚本实测输出）

| 项 | 值 |
|---|---|
| 目标目录 | `/Game/Assets/Props/ExtractionPlatform/` |
| 包围盒 | X=600.0 / Y=600.0 / **Z=30.1** cm |
| 包围盒 min Z | 0.00（原点在底面中心） |
| 顶点 / 三角 | 482 verts / 480 faces |
| Nanite | 已开启（480 面用不上，可关，不影响外观） |
| 材质槽 | 槽0 → `M_ExtractionPlatform_01`；槽1 → `M_ExtractionPlatform_01_Rim` |
| 贴图 | BC sRGB / TC_DEFAULT；N 非 sRGB / TC_NORMALMAP / flip_green=True（导入器自动翻绿，符合 UE 约定）；ORM 非 sRGB / TC_MASKS |
| 碰撞 | **没有**。按需求由蓝图里的 Box 负责 |

## 4. 资产清单

```
Content/Assets/Props/ExtractionPlatform/
    SM_ExtractionPlatform_01.uasset        静态网格
    M_ExtractionPlatform_01.uasset         金属主材质（Material Instance，父 /InterchangeAssets/gltf/Substrate/M_GLTF）
    M_ExtractionPlatform_01_Rim.uasset     发光环材质（Material Instance，同一个父）
    T_ExtractionPlatform_01_BC.uasset      1024² 基础色
    T_ExtractionPlatform_01_N.uasset       1024² 法线
    T_ExtractionPlatform_01_ORM.uasset     1024² R=AO G=Roughness B=Metallic

Content/Assets/_Source/ExtractionPlatform/
    SM_ExtractionPlatform_01.blend         源工程（含材质与贴图节点）
    SM_ExtractionPlatform_01.glb           导出件（导入用）
    T_ExtractionPlatform_01_BC/N/ORM.png   贴图源文件
    build_platform.py / make_textures.py   生成脚本（可复现）
    ue_import_platform.py                  导入脚本
    raw_flux_grunge/                        FLUX 原始出图
    ue_import.log                          导入日志
```

## 5. 材质里以后要手工调的参数

两个材质都是 Material Instance（Interchange 自动生成），参数名是固定的：

**`M_ExtractionPlatform_01_Rim`（发光环）—— 主要要调的是这个**

| 参数 | 当前值 | 怎么调 |
|---|---|---|
| `EmissiveFactor` | (0.35, 0.95, 0.88) | 改色相/改琥珀色就改这里；想更亮只能靠后处理 Bloom 或换自建材质，因为 glTF 父材质把发光钳在 0–1 |
| `BaseColorFactor` | (0.05, 0.20, 0.19) | 熄灭状态下环的颜色 |
| `RoughnessFactor` | 0.45 | 无所谓，发光盖过它 |
| `MetallicFactor` | 0.0 | 保持 0 |

**`M_ExtractionPlatform_01`（金属本体）**

| 参数 | 说明 |
|---|---|
| `BaseColorTexture` / `NormalTexture` / `MetallicRoughnessTexture` | 三张贴图，替换即换风格 |
| 整体偏亮/偏暗 | 父材质没暴露标量。可在 MI 上手动 Add Override → 加一个 `BaseColorFactor` 覆写整体染色（这是最省事的做法），否则只能改 BC 贴图重导 |

**发光要"真的亮"**：目前 EmissiveFactor 最大 1.0，不开 Bloom 时只是"荧光色块"。要更亮两条路 —— ① 后处理体积里加 Bloom（推荐，零资产改动）；② 把 `M_ExtractionPlatform_01_Rim` 换成自建材质并用 `Emissive * 5~8` 直连（需要动材质资产，本批没做）。

## 6. 已知瑕疵（按严重度）

1. **侧壁贴图偏糊**：图集里侧壁只分到 225×82px，沿周向重复 6 次，折算约 5.6cm/像素。近距离（<1.5m）能看到竖向拉丝与 6 处重复接缝。远观无感。
2. **顶面 0.81cm/像素**：凑近看警示黄环边缘有像素台阶。
3. **图案固定**：同一套 UV，多个平台并排时磨损花样会明显重复。要消除得做 2–3 个变体贴图。
4. **顶面凹槽全在法线里**：关掉法线或画质设很低时，盘面会变得像一块平板（几何上是平的，这是有意为之——保证玩家站得稳）。
5. **发光环略悬浮**：为防 z-fighting 抬高 1mm，极斜视角能看出环与盘面之间一道极细的缝。
6. **AI 污渍层不是无缝贴图**：本件只用中央 78% 内容区，所以不受影响；若以后想平铺复用这张图，边缘会有接缝。程序化本体本身是按 UV 布局画的，不依赖平铺。
7. 没有碰撞、没有 LOD、没做面数优化——按需求范围外。

## 7. 进引擎后建议先做的三件事（不属于我职责，交接给程序）

1. 放一个 Box Collision 覆盖 600×600×30（顶面 z=30），或直接用盒体碰撞的 BP。
2. 触发体积用球/圆柱套在平台中心，半径 ≈ 280cm。
3. 后处理体积里开 Bloom，`M_ExtractionPlatform_01_Rim` 才会有"发光"观感。

## 8. 预览图

`Docs/art/previews/ExtractionPlatform_34.png`（3/4）、`_side.png`（平视）、`_top.png`（俯视）。
预览由 Blender EEVEE 直接从 `_Source` 的 .blend 渲染，未做后期，与环境色的关系仅作参考。
