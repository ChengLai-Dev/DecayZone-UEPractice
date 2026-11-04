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
| `Grid_Backpack` | UniformGridPanel | 20 个背包格的容器，按 `ItemSlot_` 前缀收集 |
| `Text_WeightValue` | TextBlock | 中栏负重文本 `24.5 / 40 KG` |
| `Text_TotalValue` | TextBlock | 底栏总价值 |

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `RootCanvas` / `Size_Root` | CanvasPanel / SizeBox | 根与居中尺寸 |
| `Border_Outline` / `BG_Panel` | Border | 1px 描边 + 面板底 |
| `VB_Main` / `HB_Regions` | VerticalBox / HorizontalBox | 三栏与底栏的主布局 |
| `Panel_Equipment` / `Panel_Backpack` / `Panel_Detail` | Border | 三个区域的底板 |
| `VB_Equipment` / `VB_Backpack` / `VB_Detail` | VerticalBox | 区域内部布局 |
| `Text_EquipTitle` / `Text_BackpackTitle` | TextBlock | 固定标题 `EQUIPMENT` / `BACKPACK` |
| `Row_Equip_1..4` / `Cell_*` | HorizontalBox / VerticalBox | 装备区 2×4 排布 |
| `Text_Label_*`（8 个） | TextBlock | 装备槽类型标签（头盔/护甲/主武器…），固定文案 |
| `HB_BackpackHeader` / `Spacer_BackpackHeader` | HorizontalBox / Spacer | 背包标题行布局 |
| `Panel_Footer` / `HB_Footer` / `Spacer_Footer` | 容器 | 底栏布局 |
| `Text_Label_Total` / `Text_EscHint` | TextBlock | 固定文案「总价值」「[ESC] 关闭」 |
| 所有 `Size_Divider_*` / `Divider_*` | 容器 | 分隔线 |

---

## 交互

| 触发来源 | 事件 | 程序要做什么 |
|---|---|---|
| 背包格（见 [WBP_ItemSlot.md](WBP_ItemSlot.md)） | 格子被左键点击 | 1) 遍历 20 格：被点的格子刷金框，其余刷回灰框（保证同时最多一个选中）；2) 该格有物品 → 右侧 `ItemDetail` 切到详情态并填入该物品；3) 该格为空 → 按待定方案处理（取消选中或忽略） |
| 详情按钮（见 [WBP_ItemDetail.md](WBP_ItemDetail.md)） | 使用 / 装备 / 丢弃 | 对当前选中物品执行对应操作，并把结果反映到格子与详情 |
| 面板空白处 | 左键单击 | **待定**：取消选中 / 无响应 |
| `ESC` | 按键 | 关闭面板（按键不在 UI 节点上） |

按钮在没有选中物品时不可见（详情处于汇总态），无需额外置灰逻辑。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 打开 / 关闭 | 面板整体 | 由外部加到视口 / 移除 |
| 选中哪个格子 | 子控件的 `Border_Selected` | 见 [WBP_ItemSlot.md](WBP_ItemSlot.md) |
| 详情显示什么 | 子控件的 `Switcher_Detail` | 见 [WBP_ItemDetail.md](WBP_ItemDetail.md) |

面板本身没有独立的状态节点，选中与详情表现都落在子控件上。

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `ItemSlot_1` … `ItemSlot_20` | `WBP_ItemSlot` | 20 个背包格，逐个设置物品/数量/选中 | 见 [WBP_ItemSlot.md](WBP_ItemSlot.md) |
| `EquipSlot_Helmet` / `EquipSlot_Armor` / `EquipSlot_Primary` / `EquipSlot_Secondary` / `EquipSlot_Melee` / `EquipSlot_Backpack` / `EquipSlot_ChestRig` / `EquipSlot_Headset` | `WBP_EquipmentSlot` | 按槽位设置装备图标与名称 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `WeightBar` | `WBP_WeightBar` | 设置中栏负重比例 | 见 [WBP_WeightBar.md](WBP_WeightBar.md) |
| `ItemDetail` | `WBP_ItemDetail` | 切换汇总/详情态、填物品数据 | 见 [WBP_ItemDetail.md](WBP_ItemDetail.md) |
