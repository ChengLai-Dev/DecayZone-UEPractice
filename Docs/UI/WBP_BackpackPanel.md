# WBP_BackpackPanel 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_BackpackPanel` |
| 父类 | `UUserWidget`（未接 C++，建议 `UDZ_BackpackPanel`） |
| 源文件 | `UIWidgets/UI/Inventory/WBP_BackpackPanel.json` |
| 尺寸 | 宽随父级、高随父级（整列容器） |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Grid_Backpack` | UniformGridPanel | 20 个背包格的容器，按 `ItemSlot_` 前缀收集 |
| `Text_WeightValue` | TextBlock | 负重文本 `24.5 / 40 KG` |
| `WeightBar` | `WBP_WeightBar` | 负重条比例 |
| `ItemSlot_1` … `ItemSlot_20` | `WBP_ItemSlot` | 20 个背包格：设置物品、数量、选中、拖放落点 |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。格子内部节点见 [WBP_ItemSlot.md](WBP_ItemSlot.md)。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Panel_Backpack` | Border | 整列底板 + 15px 内边距，纯装饰 |
| `VB_Backpack` | VerticalBox | 列内布局 |
| `HB_BackpackHeader` | HorizontalBox | 标题行布局 |
| `Text_BackpackTitle` | TextBlock | 固定标题 `BACKPACK` |
| `Spacer_BackpackHeader` | Spacer | 把负重文本推到右侧 |
| `Size_Divider_Backpack` / `Divider_Backpack` | SizeBox / Border | 分隔线 |

---

## 交互

| 节点 | 事件 | 程序要做什么 |
|---|---|---|
| `ItemSlot_*` | 左键单击 | 通知持有方「选中了哪个格子」；是否选中、是否联动详情由持有方决定 |
| `ItemSlot_*` | 左键拖拽 | 通知持有方「开始拖拽 / 落在哪个格子」；跨栏拖放（战利品 → 背包）由持有方判断 |
| `WeightBar` | 无 | 只读显示，程序改比例即可 |

> 建议由 `UDZ_BackpackPanel` 统一收集 20 个格子并向持有方发事件；这样 `WBP_InventoryPanel` 和 `WBP_LootPanel` 都不需要自己遍历 `Grid_Backpack`。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 负重变化 | `Text_WeightValue` | `当前重量 / 上限 KG` |
| 负重变化 | `WeightBar` 的 `Bar_Weight` | `Percent = 当前重量 / 上限`；见 [WBP_WeightBar.md](WBP_WeightBar.md) |
| 空格 / 有物品 / 选中 / 有数量 | `ItemSlot_*` | 见 [WBP_ItemSlot.md](WBP_ItemSlot.md) |
| 拖拽悬停 | `ItemSlot_*` 的 `Border_Selected` | 刷亮金色，表示可放下；离开或松手恢复 |

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `ItemSlot_1` … `ItemSlot_20` | `WBP_ItemSlot` | 逐个设置物品/数量/选中，接收点击与拖放 | 见 [WBP_ItemSlot.md](WBP_ItemSlot.md) |
| `WeightBar` | `WBP_WeightBar` | 设置负重比例 | 见 [WBP_WeightBar.md](WBP_WeightBar.md) |
