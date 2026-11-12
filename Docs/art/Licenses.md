# 授权清单

任何引入的模型、素材、字体、图库都要在这里留一条：用了什么、许可证、商用范围、结论。

## 已使用

| 物件 | 来源 | 许可证 | 商用 | 结论 |
|---|---|---|---|---|
| FLUX 3 Klein（概念图生成） | 本机 `ImagePipeline/models`，`flux3-image-klein-int8-convrot` | FLUX Kommunity License | 年营收 < $5M 可商用；不允许商用微调 | 可用。本项目为作品集、无营收，且不做微调 |
| Pixal3D（图生 3D） | 本机 `UEAssetsPipeline/models`，`pixal3d_int8_convrot` | MIT（2026 年中由腾讯改为 MIT） | 无限制 | 可用 |
| TRELLIS.2（结构/稀疏阶段） | 同上，`trellis_2_int8_convrot` | MIT | 无限制 | 可用 |
| MoGe 2（相机 FOV 估计） | 同上，`moge_2_vitl_normal_fp16` | MIT | 无限制 | 可用 |
| BiRefNet（去背景） | 同上，`birefnet` | MIT | 无限制 | 可用 |
| 生成结果本身（SM/M/T_ExtractionPlatform_01） | 上述管线产出，无第三方素材混入 | 随生成模型条款，可作为项目原创资产使用 | 可 | 可用 |

## 未使用 / 已排除

| 物件 | 原因 |
|---|---|
| 第三方模型网站的道具/平台模型 | 未引入。本次全部走本机生成，无需核对第三方授权 |
| 外部字体、图库、材质扫描图 | 未引入。材质仅由生成管线的 BC/ORM/N 三张贴图构成 |

## 备注

- `_Source/ExtractionPlatform/_prev_attempt_procedural/` 里是上一轮程序化建模尝试的产物（Blender 文件、程序化贴图），**未进引擎、未参与本次交付**，仅留档。其中的贴图由脚本程序化生成，不涉及第三方版权。
