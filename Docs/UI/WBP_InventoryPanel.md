# WBP_InventoryPanel 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_InventoryPanel` |
| 父类 | `UDZ_InventoryPanel` |
| 源文件 | `UIWidgets/UI/Inventory/WBP_InventoryPanel.json` |
| 尺寸 | 1408×858，屏幕居中 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `EquipmentPanel` | `WBP_EquipmentPanel` | 左栏装备区；设置 8 个装备槽 |
| `BackpackPanel` | `WBP_BackpackPanel` | 中栏背包区；设置 20 格物品、负重文本/条、接收格子事件 |
| `ItemDetail` | `WBP_ItemDetail` | 右栏详情；切换汇总/详情态、填物品数据 |
| `Text_TotalValue` | TextBlock | 底栏总价值 |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `RootCanvas` / `Size_Root` | CanvasPanel / SizeBox | 根与居中尺寸 |
| `Border_Outline` / `BG_Panel` | Border | 1px 描边 + 面板底 |
| `VB_Main` / `HB_Regions` | VerticalBox / HorizontalBox | 三栏与底栏的主布局 |
| `Panel_Detail` | Border | 右栏底板 + 15px 内边距，纯装饰 |
| `Panel_Footer` / `HB_Footer` / `Spacer_Footer` | 容器 | 底栏布局 |
| `Text_Label_Total` / `Text_EscHint` | TextBlock | 固定文案「总价值」「[ESC] 关闭」 |

> `Grid_Backpack`、`Text_WeightValue`、20 个 `ItemSlot_*` 已迁到 [WBP_BackpackPanel.md](WBP_BackpackPanel.md)；8 个 `EquipSlot_*` 已迁到 [WBP_EquipmentPanel.md](WBP_EquipmentPanel.md)。

---

## 交互

| 触发来源 | 事件 | 程序要做什么 |
|---|---|---|
| `BackpackPanel` 的格子（见 [WBP_ItemSlot.md](WBP_ItemSlot.md)） | 格子被左键点击 | 1) 遍历背包格：被点的格子刷金框，其余刷回灰框（保证同时最多一个选中）；2) 该格有物品 → `ItemDetail` 切到详情态并填入该物品；3) 该格为空 → 按待定方案处理（取消选中或忽略） |
| `BackpackPanel` 的格子 | 拖拽开始 / 拖放结束 | 背包面板只上报事件；跨栏拖放（战利品 → 背包）由 `WBP_LootPanel` 处理，本面板不处理战利品行 |
| `ItemDetail` 的按钮（见 [WBP_ItemDetail.md](WBP_ItemDetail.md)） | 使用 / 装备 / 丢弃 | 对当前选中物品执行对应操作，并把结果反映到格子与详情 |
| `EquipmentPanel` | 装备槽点击 | 当前不接；见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) 的「待定」 |
| 面板空白处 | 左键单击 | **待定**：取消选中 / 无响应 |
| `ESC` | 按键 | 关闭面板（按键不在 UI 节点上） |

按钮在没有选中物品时不可见（详情处于汇总态），无需额外置灰逻辑。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 打开 / 关闭 | 面板整体 | 由外部加到视口 / 移除 |
| 选中哪个背包格 | `BackpackPanel` 内的 `Border_Selected` | 见 [WBP_ItemSlot.md](WBP_ItemSlot.md) |
| 详情显示什么 | `ItemDetail` 的 `Switcher_Detail` | 见 [WBP_ItemDetail.md](WBP_ItemDetail.md) |
| 负重变化 | `BackpackPanel` 的 `Text_WeightValue` / `WeightBar` | 见 [WBP_BackpackPanel.md](WBP_BackpackPanel.md) |

面板本身没有独立的状态节点，选中、负重与详情表现都落在子控件上。

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `EquipmentPanel` | `WBP_EquipmentPanel` | 设置 8 个装备槽的图标/名称/选中 | 见 [WBP_EquipmentPanel.md](WBP_EquipmentPanel.md) |
| `BackpackPanel` | `WBP_BackpackPanel` | 设置 20 格物品/数量/负重；接收格子点击与拖放 | 见 [WBP_BackpackPanel.md](WBP_BackpackPanel.md) |
| `ItemDetail` | `WBP_ItemDetail` | 切换汇总/详情态、填物品数据 | 见 [WBP_ItemDetail.md](WBP_ItemDetail.md) |
