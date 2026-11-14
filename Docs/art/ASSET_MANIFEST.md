# 素材清单 · DecayZone

> 元信息口径（程序接入与接缝审查的唯一依据）：
> **朝向基准** = 模型正面朝哪个轴；**锚点** = 原点在模型的什么位置；**尺寸** = UE 内包围盒（cm）。
> 本批只有一件资产，其余条目等后续批次补。

## 批次 2026-11-12（新增，无修复项）

### SM_ExtractionPlatform_01 — 撤离点工业平台

| 项 | 值 |
|---|---|
| 运行期路径 | `/Game/Assets/Props/ExtractionPlatform/SM_ExtractionPlatform_01` |
| 源文件路径 | `Content/Assets/_Source/ExtractionPlatform/`（.blend / .glb / 生成脚本 / FLUX 原图） |
| 类型 | 静态网格（场景件） |
| 尺寸 | 600.0 × 600.0 × 30.1 cm（X/Y 为直径） |
| **锚点** | **底面中心**（包围盒 min Z = 0.00），直接摆在 z=0 即贴地 |
| **朝向基准** | 无方向性（旋转对称，96 段圆周）；警示环与径向拼缝每 30° 重复一次，无"正面"概念 |
| 旋转对齐 | 允许任意 Yaw；如需与别的构件对缝，以 Yaw=0 时 12 条径向拼缝分别落在 0°/30°/…/330° |
| 顶点 / 三角 | 482 / 480 |
| 材质槽 | 0 = `M_ExtractionPlatform_01`（金属），1 = `M_ExtractionPlatform_01_Rim`（发光） |
| 贴图 | `T_ExtractionPlatform_01_BC` / `_N` / `_ORM`，各 1024² |
| 碰撞 | 无。需在蓝图侧用 Box（600×600×30）承担 |
| LOD | 无 |
| 动画 | 无（静态件） |
| 生成手段 | Blender 程序化几何 + numpy 程序化图集 + FLUX 3 Klein 污渍层（seed 88991） |
| 授权结论 | 可商用（见 `LICENSE_LIST.md`） |
| 备注 | 因为只有 480 面，Nanite 已开但无实际收益，可关 |

## 批次 2026-11-14（M5 技能栏 UI 资源）

### T_UI_Icon_Skill_Shield / T_UI_Icon_Skill_Sprint — 技能图标

| 项 | 值 |
|---|---|
| 运行期路径 | `/Game/Assets/SkillIcons/Runtime/T_UI_Icon_Skill_Shield`、`.../T_UI_Icon_Skill_Sprint` |
| 源文件路径 | `Content/Assets/_Source/UI/Icons/`（PNG + `.post.json` 后处理账本 + `SkillIconsImport.py`） |
| 类型 | UI 图标（Texture2D） |
| 尺寸 | 512 × 512，四角 alpha = 0 |
| 贴图设置 | `TC_EditorIcon` / `TEXTUREGROUP_UI` / 无 mip / `never_stream` / `Clamp`，与 `ItemIcons/Runtime` 一致 |
| **锚点 / 朝向** | 不适用（UI 平面资源） |
| 生成手段 | 本地 MOSS-Image 1024² 出图 → 边缘泛洪抠底 → 内容归一 + 6% 留白 → 512² |
| 授权结论 | Apache-2.0，可商用（见 `LICENSE_LIST.md`） |
| 备注 | 加速图标首版（seed 770102）带地面投影，抠底后残留灰晕，已换 seed 770103 重出 |

### M_CooldownRing / MI_CooldownRing — 技能冷却扇形遮罩

| 项 | 值 |
|---|---|
| 运行期路径 | `/Game/UI/Materials/M_CooldownRing`、`/Game/UI/Materials/MI_CooldownRing` |
| 类型 | UI 域材质（Translucent，输出 Opacity） |
| 参数 | 标量 `Percent`：0 = 无遮罩，1 = 整圆遮住，0.4 = 从 12 点顺时针遮 40% |
| 视觉 | 半透明暗色扇形（Opacity ≈ 0.72），不接颜色参数 |
| 与 M_LootRing 的差异 | `M_LootRing` 是细环描边（Radius/Thickness/Progress/Color），几何上做不出填充扇形，故另建 |
| 授权结论 | 自有产出（见 `LICENSE_LIST.md`） |

### WBP_AbilityBar / WBP_AbilitySlot — 技能栏界面

| 项 | 值 |
|---|---|
| 运行期路径 | `/Game/UI/HUD/WBP_AbilityBar`、`/Game/UI/HUD/WBP_AbilitySlot` |
| JSON 源 | `UIWidgets/UI/HUD/WBP_AbilityBar.json`、`WBP_AbilitySlot.json` |
| 尺寸 | 单槽 64×64，栏 140×64（两槽 + 12px 间距），底部居中、距底 36px |
| 交接文档 | `Docs/UI/AbilityBar_Handoff.md` |

## 命名与放置约定（本批确立）

| 类别 | 规则 | 示例 |
|---|---|---|
| 静态网格 | `SM_<物件>_<两位序号>`，一物件一目录 | `SM_ExtractionPlatform_01` |
| 主材质 | `M_<物件>_<序号>` | `M_ExtractionPlatform_01` |
| 附属材质（同一物件的第二个及以后材质槽） | `M_<物件>_<序号>_<用途>` | `M_ExtractionPlatform_01_Rim` |
| 贴图 | `T_<物件>_<序号>_<BC\|N\|ORM>` | `T_ExtractionPlatform_01_BC` |
| 图集通道 | BC=RGB 基础色；N=法线；ORM：R=AO，G=Roughness，B=Metallic | — |
