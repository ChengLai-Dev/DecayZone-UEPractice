# WBP_AbilityBar 交互文档

屏幕底部正中常驻的技能栏，持有两个技能槽。

## 1. 资产

| 项 | 值 |
|---|---|
| 资产路径 | `/Game/UI/HUD/WBP_AbilityBar` |
| JSON 源 | `UIWidgets/UI/HUD/WBP_AbilityBar.json` |
| 父类 | `UDZ_AbilityBar`（已建，继承 `UDZ_UserWidget`） |
| 设计尺寸 | `Desired` |
| 依赖 | `WBP_AbilitySlot`（同目录，两个实例） |
| 宿主 | `WBP_HUD` 的子控件（`UDZ_HUDLayout::AbilityBar`） |

## 2. 控件树

```
Size_AbilityBar (SizeBox 140×64, SelfHitTestInvisible, 根节点)
└─ HB_Slots (HorizontalBox)
   ├─ AbilitySlot_1 → WBP_AbilitySlot（右侧留 12px 间距，垂直居中）
   └─ AbilitySlot_2 → WBP_AbilitySlot
```

位置与锚点**不在本控件里**：由 `WBP_HUD` 的 `CanvasPanelSlot` 决定（锚点 `(0.5,1)`、轴心 `(0.5,1)`、偏移 `top=-36`，见 `Docs/UI/WBP_HUD.md`）。本控件根节点已经压平成 `SizeBox`，尺寸只由 `WidthOverride` / `HeightOverride` 决定，HUD 根的槽位开 `bAutoSize` 跟随。

宽度 140 = 64 + 12 + 64。加槽时同步改 `Size_AbilityBar` 的 `WidthOverride`，否则右半截会被裁掉。

## 3. 程序要绑定的节点

| 节点 | 类型 | 用途 |
|---|---|---|
| `AbilitySlot_1` | `UDZ_AbilitySlot*` | 技能槽 1（护盾） |
| `AbilitySlot_2` | `UDZ_AbilitySlot*` | 技能槽 2（加速） |
| `HB_Slots` | `UHorizontalBox` | 仅当需要运行时动态加槽才用，固定两槽时可不绑 |

`BindWidget` 用**精确名**（不带 `BindWidgetOptional`），缺控件会编译报错，便于早期发现改坏。

## 4. 与输入键位的对应

| 槽 | 键位 | 图标 | 默认技能（数据，可改） |
|---|---|---|---|
| `AbilitySlot_1` | `1` | 六边形护盾 | `Shield` 行 |
| `AbilitySlot_2` | `2` | 战术靴 + 速度线 | `Sprint` 行 |

键位角标**必须由代码写入**：两个槽都是同一个 `WBP_AbilitySlot` 的实例，JSON 里 `Text_Key` 的默认值只有一份（`1`），所以在 Designer 里两个槽都会显示 `1`，这是预期状态，不是配错。跑起来后由 `UDZ_AbilityBar::SetKeyText(槽位, 文案)` 分别填 `1` / `2`。

键位属于**槽位**，不属于技能：技能换槽后键位不变。默认技能来自 `DA_UIConfig.AbilitySlots`，运行时由 `ADZ_PlayerController::SetSlotSkill(槽位, 技能行名)` 改写；把技能装到已存在该技能的槽时会自动交换，不产生重复。

## 5. 显示方式

- **常驻**：本控件不再自己 `AddToViewport`，作为 `WBP_HUD` 的子控件随 HUD 根一起显示一次。
- **层级**：由 HUD 根内部的控件树顺序决定，不再走 `UDZ_UIConfig::ZOrderMap`。
- **输入**：根节点 `SizeBox` 为 `SelfHitTestInvisible`，不拦鼠标，不参与输入模式切换。
- **显隐**：进局显示；死亡 / 结算面板打开时建议隐藏（由程序决定，见未完成项 4）。

## 6. 尺寸与位置口径

| 项 | 值 | 来源 |
|---|---|---|
| 单槽 | 64×64 | `WBP_AbilitySlot` 根 `SizeBox` |
| 槽间距 | 12px | `AbilitySlot_1` 的 `HorizontalBoxSlot` 右 padding |
| 整栏尺寸 | 140×64 | `Size_AbilityBar` 的 `WidthOverride` / `HeightOverride` |
| 水平位置 | 屏幕水平居中 | `WBP_HUD` 槽位锚点 `x=0.5`、轴心 `0.5` |
| 垂直位置 | 距屏幕底 36px | `WBP_HUD` 槽位锚点 `y=1`、轴心 `y=1`、偏移 `top=-36` |

36px 是留白，不是硬需求；`WBP_StatusBars` 在左下角距底 40px，两者视觉基线接近。要微调只改 `WBP_HUD` 里 `AbilityBar` 的槽位偏移即可，不需要动本控件。

## 7. 仍未做

| 项 | 归属 |
|---|---|
| 换槽界面（技能列表、拖拽/点击装备） | 后续 |
| 映射持久化（M8 的 SaveGame） | 后续 |
| 扩到 4 槽（同时改 `Size_AbilityBar` 宽度与 `AbilitySlots` 长度） | 后续 |
| `GA_Shield` / `GA_Sprint`、Cost/Cooldown GE、`IA_Skill1/2` 与 IMC 映射 | 程序/玩法侧 |

## 8. 验收

| 场景 | 期望 |
|---|---|
| 进游戏 | 屏幕底部正中出现两个技能槽，左右对称 |
| 窗口缩放 | 技能栏始终水平居中、距底距离不变 |
| 打开背包 / 搜刮 | 技能栏仍在 |
| 死亡 / 结算 | 技能栏隐藏 |
| 无 GA 时 | 图标与 `1` / `2` 正常显示，整体变暗，无冷却遮罩 |
| GA 落地后 | 按 1 / 2 出现冷却遮罩与倒计时，能量不足时变暗 |
| 换槽（调用 `SetSlotSkill`） | 两个槽的图标互换，冷却跟着技能走 |
