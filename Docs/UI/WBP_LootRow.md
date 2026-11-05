# WBP_LootRow 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Loot/WBP_LootRow` |
| 父类 | `UUserWidget`（未接 C++，建议 `UDZ_LootRow : UDZ_UserWidget`） |
| 源文件 | `UIWidgets/UI/Loot/WBP_LootRow.json` |
| 尺寸 | 宽随父级、高 64 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Img_ItemIcon` | Image | 物品图标。默认隐藏，有图标时显示并设贴图 |
| `Text_ItemName` | TextBlock | 物品名称 |
| `Text_Quantity` | TextBlock | 数量文本 `xN` |
| `Text_ItemWeight` | TextBlock | 本行总重量 `N.N KG` |
| `Frame_RowBG` | Border | 行底色；默认/悬停/选中三种颜色 |
| `Frame_Selected` | Border | 左侧 3px 选中金条，默认隐藏 |

> `Frame_RowBG` 和 `Frame_Selected` 都是对象名；不要按 Hierarchy 上显示的 `Border` 去找。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Root_Size` | SizeBox | 行高 64，整行命中区 |
| `Overlay_Row` | Overlay | 行底色、内容、选中条的叠放容器 |
| `HB_Row` | HorizontalBox | 图标 / 名称 / 数量 / 重量的排版 |
| `Size_Icon` | SizeBox | 图标显示区 44×44 |
| `VB_Info` | VerticalBox | 名称与提示文字 |
| `Text_ItemHint` | TextBlock | 固定提示「拖拽到背包」 |
| `Size_Quantity` | SizeBox | 数量列宽 56 |
| `Size_Weight` | SizeBox | 重量列宽 80 |
| `Size_SelectedBar` | SizeBox | 选中条 3×36 的定位容器 |

---

## 交互

| 节点 | 事件 | 程序要做什么 |
|---|---|---|
| 整行（`Root_Size`） | 左键单击 | 通知列表/面板「这一行被选中」；同一时间只保留一行选中 |
| 整行（`Root_Size`） | 左键拖拽 | 开始拖拽；拖影复制当前行内容；松手落在背包格时由持有方（`WBP_LootPanel`）决定是否放入 |
| 整行（`Root_Size`） | 鼠标移入 / 移出 | 切换 `Frame_RowBG` 的悬停色；选中行保持选中色 |

> 拖拽的落点处理不写在本控件：行只负责「开始拖」，落点、数量合并、从列表移除都由 `WBP_LootPanel` 和 `WBP_BackpackPanel` 处理。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 默认 | `Frame_RowBG` | 深色底 |
| 悬停 | `Frame_RowBG` | 略亮的底色 |
| 选中 | `Frame_Selected` + `Frame_RowBG` | 左侧 3px 金条显示；行底色加深 |
| 无图标 | `Img_ItemIcon` | 隐藏 |
| 有图标 | `Img_ItemIcon` | 显示 + 指定贴图 |
| 数量 | `Text_Quantity` | `xN`；单件也显示 `x1` |

---

## 含有的子控件

无。
