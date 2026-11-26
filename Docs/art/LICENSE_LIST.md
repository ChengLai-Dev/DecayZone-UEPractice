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

## 批次 2026-11-14

### 技能图标 T_UI_Icon_Skill_Shield / T_UI_Icon_Skill_Sprint（M5 技能栏）

| 组成 | 来源 | 授权 | 结论 |
|---|---|---|---|
| 图标主体 | 本地 MOSS-Image 生成（seed 770101 / 770103，ComfyUI），后处理抠底归一到 512² | **Apache-2.0** | ✅ 可商用、可改、可微调，无营收门槛 |
| `M_CooldownRing` / `MI_CooldownRing` | 本项目 artist 用 UE 材质与 HLSL 自建 | 自有产出 | ✅ 可商用 |

**未使用网络素材**：图标由本地模型生成后抠底，冷却环材质自建，因此本批没有任何第三方图库依赖，不需要署名。上一轮备选的 game-icons.net（CC BY 3.0，需署名）**未采用**。

## 2026-11-26

### 步枪与枪械表现资源（SM_Weapon_Rifle_01 / 枪声 / 枪口火光 / 弹孔）

| 组成 | 来源 | 授权 | 结论 |
|---|---|---|---|
| 步枪模型 | Quaternius《Rifle》，`https://poly.pizza/m/cCAgiMOQow` | **CC0 1.0** | ✅ 可商用、可改、无需署名；来源页与源文件已归档 |
| 枪声 | 《The Free Firearm Sound Library》，Ben Jaszczak 等，`https://opengameart.org/content/the-free-firearm-sound-library` | **CC0 1.0** | ✅ 可商用、可改、无需署名；裁切参数写入同名 json |
| 枪口火光 / 命中火花 / 弹孔贴图 | 本项目 artist 用 numpy + Pillow 程序化生成 | 自有产出 | ✅ 可商用 |
| 火光 / 火花网格 | 本项目 artist 用 Blender 5.2 生成 | 自有产出 | ✅ 可商用 |
| 武器材质与贴花材质 | 本项目 artist 在 UE 内自建 | 自有产出 | ✅ 可商用 |

**关于 poly.pizza 的例外**：此前排除 poly.pizza 的原因是「来源与授权需精确溯源」。本次使用的 Rifle 页面明确标注 Public Domain（CC0），作者 Quaternius 可追溯，模型未做二次训练或微调，源文件与来源页记录在 `Content/Assets/_Source/Weapons/SM_Weapon_Rifle_01.source.json`，因此例外采用。

## 明确排除的东西

| 排除对象 | 原因 |
|---|---|
| 网络抓取的钢铁/工业贴图 | 来源不可考证，无法证明可商用 |
| Sketchfab / poly.pizza 现成平台模型（未逐件溯源时） | 本件形状需精确控制，且要避免授权条款差异；程序化即可满足。2026-11-26 的 Quaternius Rifle（页面明确 CC0）例外见上节 |
| Quaternius 角色包里的贴图 | 该包仅用于丧尸资产，授权范围未覆盖场所件，不挪用 |

## 待办

- 后续批次若引入第三方模型/字体，逐条补到此表，并要求可查证的授权页链接。
