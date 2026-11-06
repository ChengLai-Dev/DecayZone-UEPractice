# WBP_LootRow 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Loot/WBP_LootRow` |
| 父类 | `UDZ_LootRow : UDZ_UserWidget` |
| 源文件 | `UIWidgets/UI/Loot/WBP_LootRow.json` |
| 尺寸 | 宽随父级、高 64 |

> 本控件**没有选中态**。选中态原来用于联动详情栏，但搜刮面板没有详情栏，该状态没有消费方，已移除。行的操作只有两种：拖拽、点「拿取」按钮。

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Img_ItemIcon` | Image | 物品图标。默认隐藏，有图标时显示并设贴图 |
| `Text_ItemName` | TextBlock | 物品名称，过长自动省略号 |
| `Text_Quantity` | TextBlock | 数量文本 `xN` |
| `Text_ItemWeight` | TextBlock | 本行总重量 `N.N KG` |
| `Frame_RowBG` | Border | 行底色；默认 / 悬停两种颜色 |
| `Btn_Take` | Button | 「拿取」按钮，接收单击 |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Root_Size` | SizeBox | 行高 64，整行命中区与拖拽源 |
| `Overlay_Row` | Overlay | 行底色与内容的叠放容器 |
| `HB_Row` | HorizontalBox | 图标 / 名称 / 数量 / 重量 / 按钮的排版 |
| `Size_Icon` | SizeBox | 图标显示区 44×44 |
| `VB_Info` | VerticalBox | 名称与提示文字 |
| `Text_ItemHint` | TextBlock | 固定提示「拖拽 / 点拿取」 |
| `Size_Quantity` | SizeBox | 数量列宽 44 |
| `Size_Weight` | SizeBox | 重量列宽 64 |
| `Size_Take` | SizeBox | 拿取列宽 44 |
| `Text_Take` | TextBlock | 按钮上的固定文字「拿取」 |

---

## 列宽（与 `WBP_LootListPanel` 的表头手工对齐）

| 列 | 宽 | 内边距 |
|---|---|---|
| 图标 | 44 | 行内左 14 |
| 名称 | Fill | 左 10 |
| 数量 | 44 | — |
| 重量 | 64 | 左 6 |
| 拿取 | 44 | 左 6、行内右 14 |

改任何一列都必须同步 `WBP_LootListPanel` 的 `HB_Columns`，否则表头与行错位。

---

## 交互

| 节点 | 事件 | 程序要做什么 |
|---|---|---|
| 整行（`Root_Size`） | 左键拖拽 | 开始拖拽；拖影复制当前行内容；松手落在背包格时由持有方（`WBP_LootPanel`）决定是否放入 |
| 整行（`Root_Size`） | 左键单击 | 不做任何事（无选中态） |
| 整行（`Root_Size`） | 鼠标移入 / 移出 | 切换 `Frame_RowBG` 的悬停色 |
| `Btn_Take` | 左键单击 | 把本行**整叠**移入背包 |
| `Btn_Take` | 左键单击 + 按住 Shift | 只移入 **1 件**；数量减 1、行保留，减到 0 再移除行 |

> 1. 按钮必须吞掉鼠标按下事件，不能带动整行拖拽。
> 2. 按钮与拖拽必须走**同一条**「移入背包」逻辑（见 [LootUI_Handoff.md](LootUI_Handoff.md) 第 4 节），不要各写一套。
> 3. 背包满 / 超重时当前**不做任何反馈**（点了没反应、行保留），是本期有意留的口子。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 默认 | `Frame_RowBG` | 深色底 |
| 悬停 | `Frame_RowBG` | 略亮的底色 |
| 无图标 | `Img_ItemIcon` | 隐藏 |
| 有图标 | `Img_ItemIcon` | 显示 + 指定贴图 |
| 数量 | `Text_Quantity` | `xN`；单件也显示 `x1` |

---

## 含有的子控件

无。
