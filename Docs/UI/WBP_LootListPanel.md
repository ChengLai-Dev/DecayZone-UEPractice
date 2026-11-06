# WBP_LootListPanel 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Loot/WBP_LootListPanel` |
| 父类 | `UUserWidget`（未接 C++，建议 `UDZ_LootListPanel`） |
| 源文件 | `UIWidgets/UI/Loot/WBP_LootListPanel.json` |
| 尺寸 | 宽随父级、高随父级（整列容器） |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Text_ContainerName` | TextBlock | 容器名，例如「补给箱」 |
| `Text_LootCount` | TextBlock | 容器内总件数 `N 件` |
| `Btn_Close` | Button | 接收关闭点击 |
| `ScrollBox_LootList` | ScrollBox | 战利品行容器。运行时清空设计器占位行，按数据创建 `WBP_LootRow` |
| `Text_EmptyHint` | TextBlock | 空容器提示「容器已空」，默认隐藏 |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。行内部节点见 [WBP_LootRow.md](WBP_LootRow.md)。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Panel_Loot` | Border | 整列底板 + 15px 内边距，纯装饰 |
| `VB_Loot` | VerticalBox | 列内布局 |
| `HB_Header` | HorizontalBox | 容器名 / 件数 / 关闭按钮的标题行 |
| `Spacer_Header` | Spacer | 把件数和关闭推到右侧 |
| `Text_Close` | TextBlock | 关闭按钮上的固定文字 `X` |
| `Size_Divider_Header` / `Divider_Header` | SizeBox / Border | 标题下分隔线 |
| `HB_Columns` / `Spacer_Columns` | HorizontalBox / Spacer | 列标题行布局 |
| `Text_ColItem` / `Text_ColQty` / `Text_ColWeight` | TextBlock | 固定列标题「物品」「数量」「重量」 |
| `Size_QtyHeader` | SizeBox | 数量列标题宽度 44，对齐行的 `Size_Quantity` |
| `Size_WeightHeader` | SizeBox | 重量列标题宽度 64，对齐行的 `Size_Weight` |
| `Size_TakeHeader` | SizeBox | 拿取列占位 44，对齐行的 `Size_Take`；留空不放文字 |
| `Size_Divider_Columns` / `Divider_Columns` | SizeBox / Border | 列标题下分隔线 |
| `Overlay_List` | Overlay | 列表与空容器提示的叠放容器 |
| `LootRow_1` … `LootRow_5` | `WBP_LootRow` | 设计器占位行，运行时清空重建 |

---

## 交互

| 节点 | 事件 | 程序要做什么 |
|---|---|---|
| `Btn_Close` | 左键单击 | 通知持有方（`WBP_LootPanel`）关闭搜刮面板 |
| `WBP_LootRow` | 左键拖拽结束 | 把「哪一行 → 落在哪个格子」上报给持有方；本控件不判断落点 |
| `WBP_LootRow` 内的 `Btn_Take` | 左键单击 | 上报「这一行整叠拿取」给持有方 |
| `WBP_LootRow` 内的 `Btn_Take` | 左键单击 + Shift | 上报「这一行拿取 1 件」给持有方 |
| `ScrollBox_LootList` | 滚轮 / 拖动滚动条 | 默认滚动，无需额外处理 |

> 跨控件流程（移入背包、成功后移除行 / 减数量、刷新件数/空提示）写在持有方 [WBP_LootPanel.md](WBP_LootPanel.md)。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 有物品 | `ScrollBox_LootList` + `Text_EmptyHint` | 列表显示；空提示隐藏；`Text_LootCount` = 总件数 |
| 空容器 | `ScrollBox_LootList` + `Text_EmptyHint` | 列表清空；空提示显示；`Text_LootCount` = `0 件` |

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `ScrollBox_LootList` 中的行 | `WBP_LootRow` | 按容器数据动态创建；每个行设置图标/名称/数量/重量并绑定拖拽与「拿取」按钮 | 见 [WBP_LootRow.md](WBP_LootRow.md) |
