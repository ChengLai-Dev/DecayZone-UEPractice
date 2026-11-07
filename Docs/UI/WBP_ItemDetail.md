# WBP_ItemDetail 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_ItemDetail` |
| 父类 | `UUserWidget`（未接 C++） |
| 源文件 | `UIWidgets/UI/Inventory/WBP_ItemDetail.json` |
| 尺寸 | 宽随父级、高随父级 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Switcher_Detail` | WidgetSwitcher | 切换 `0` 背包汇总 / `1` 物品详情 |
| `Text_StateTag` | TextBlock | 右上状态标签。未选中「未选中状态」+ 绿 `#5FA05C`；已选中「已选中」+ 金 `#C9A968` |
| `Text_SummaryWeight` | TextBlock | 汇总：当前负重文本 |
| `Text_SummaryCount` | TextBlock | 汇总：物品数量 `12 / 20` |
| `Text_SummaryValue` | TextBlock | 汇总：携带物品价值 |
| `Text_SummaryEmpty` | TextBlock | 汇总：空格子数 |
| `Img_ItemIcon` | Image | 详情：物品大图标。默认隐藏，有物品时显示并设贴图 |
| `Text_IconHint` | TextBlock | 详情：图标占位文字「物品图标」，设了图标后隐藏 |
| `Text_ItemName` | TextBlock | 详情：物品名称 |
| `Text_ItemType` | TextBlock | 详情：物品类型 |
| `Text_ItemWeight` | TextBlock | 详情：单件重量 |
| `Text_ItemQuantity` | TextBlock | 详情：数量 |
| `Text_ItemDesc` | TextBlock | 详情：描述 |
| `Btn_Use` | Button | 接收「使用」点击 |
| `Btn_Equip` | Button | 接收「装备」点击 |
| `Btn_Discard` | Button | 接收「丢弃」点击 |

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `VB_Detail` | VerticalBox | 根布局 |
| `HB_Header` / `Text_DetailTitle` | HorizontalBox / TextBlock | 固定的 `DETAILS` 标题行 |
| `VB_EmptyState` / `VB_ItemState` | VerticalBox | 两个状态的根，由 `Switcher_Detail` 切换 |
| `Border_SummaryCard` / `VB_SummaryCard` | Border / VerticalBox | 汇总卡片底板与布局 |
| `Size_SummaryIcon` / `Img_SummaryIcon` | SizeBox / Image | 汇总卡图标位，当前留空未用 |
| `Text_SummaryTitle` / `Text_SummaryHint` | TextBlock | 汇总卡固定文案「背包状态」「选择一个物品以查看详细信息」 |
| `Text_Label_Weight` / `Text_Label_Count` / `Text_Label_Value` / `Text_Label_Empty` | TextBlock | 汇总行左侧固定标签 |
| `Size_IconFrame` / `Border_IconFrame` / `Fill_IconFrame` / `Overlay_Icon` | 容器 | 详情大图标框（185 高、图标 132×132） |
| `HB_Meta` / `HB_Actions` | HorizontalBox | 元信息行 / 按钮行布局 |
| `Text_SectionDesc` | TextBlock | 固定小标题 `DESCRIPTION` |
| `Text_BtnUse` / `Text_BtnEquip` / `Text_BtnDiscard` | TextBlock | 按钮上的固定文字（使用/装备/丢弃） |
| 所有 `Size_Divider_*` / `Divider_*` / `Spacer_*` | 容器 | 分隔线与占位 |

---

## 交互

| 节点 | 事件 | 程序要做什么 |
|---|---|---|
| `Btn_Use` | 左键单击 | 通知面板对当前选中物品执行「使用」 |
| `Btn_Equip` | 左键单击 | 通知面板对当前选中物品执行「装备」：目标槽由物品数据的 `EquipSlot` 决定（`None` 时按钮应隐藏或点击无效），面板转成一次「背包格 → 装备槽」的移动请求 |
| `Btn_Discard` | 左键单击 | 通知面板对当前选中物品执行「丢弃」 |

按钮只在详情态（`Switcher_Detail = 1`）可见；汇总态下整组按钮随 `VB_ItemState` 一起隐藏。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 汇总态 | `Switcher_Detail` | `0`，显示负重条 + 数量/价值/空格子 |
| 详情态 | `Switcher_Detail` | `1`，显示图标 + 名称/类型/重量/数量/描述 + 按钮 |
| 状态标签：未选中 | `Text_StateTag` | 「未选中状态」+ `#5FA05C` |
| 状态标签：已选中 | `Text_StateTag` | 「已选中」+ `#C9A968` |
| 详情无图标 | `Img_ItemIcon` / `Text_IconHint` | 图标隐藏，占位文字显示 |
| 详情有图标 | `Img_ItemIcon` / `Text_IconHint` | 图标显示，占位文字隐藏 |

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `WeightBar` | `WBP_WeightBar` | 设置汇总态的负重比例 | 见 [WBP_WeightBar.md](WBP_WeightBar.md) |
