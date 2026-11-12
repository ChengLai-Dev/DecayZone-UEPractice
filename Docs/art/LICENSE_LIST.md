# 授权清单 · DecayZone

口径：任何进入 `Content/` 的模型、贴图、字体、图库都要在这里有结论。来源不明或授权受限的一律不用，并注明排除了什么。

## 批次 2026-11-12

### SM_ExtractionPlatform_01（撤离点平台）

| 组成 | 来源 | 授权 | 结论 |
|---|---|---|---|
| 网格几何 | 本项目 artist 用 Blender 5.2.1 程序化生成（`build_platform.py`） | 自有产出 | ✅ 可商用、可改、可闭源 |
| 贴图主体（环/缝/警示/铆钉/锈） | 本项目 artist 用 numpy 程序化绘制（`make_textures.py`） | 自有产出 | ✅ 可商用 |
| 贴图污渍细节层 | 本地 FLUX 3 Klein 生成，seed 88991，原始图留档 `_Source/.../raw_flux_grunge/` | FLUX [Komm​unity License] | ✅ 可用于商用出图（条件：年营收 < $5M）<br>⛔ 不得用于训练/微调模型 |

**FLUX 授权注意事项**：本项目若营收超过 500 万美元，或打算用这批贴图微调模型，需要改为 MOSS-Image（Apache-2.0）重出污渍层。当前是求职作品集项目，不触发。

### 使用的工具（不是资产，但记录在案）

| 工具 | 版本 | 授权 | 用途 |
|---|---|---|---|
| Blender | 5.2.1 LTS | GPL（产出物不受 GPL 传染） | 建模与导出 |
| Python + numpy + Pillow | 3.12 / numpy 2.5.4 / Pillow 13.0.0 | BSD / HPND | 贴图程序化绘制 |
| FLUX 3 Klein（本地 ComfyUI） | int8 蒸馏版 | Komm​unity License | 污渍细节层 |
| Unreal Engine | 5.8 | 自有引擎授权 | 导入与材质生成 |

## 明确排除的东西

| 排除对象 | 原因 |
|---|---|
| 网络抓取的钢铁/工业贴图 | 来源不可考证，无法证明可商用 |
| Sketchfab / poly.pizza 现成平台模型 | 本件形状需精确控制，且要避免授权条款差异；程序化即可满足 |
| Quaternius 角色包里的贴图 | 该包仅用于丧尸资产，授权范围未覆盖场所件，不挪用 |

## 待办

- 后续批次若引入第三方模型/字体，逐条补到此表，并要求可查证的授权页链接。
