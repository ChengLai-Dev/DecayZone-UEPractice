# WBP_EquipmentPanel 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_EquipmentPanel` |
| 父类 | `UUserWidget`（未接 C++，建议 `UDZ_EquipmentPanel`） |
| 源文件 | `UIWidgets/UI/Inventory/WBP_EquipmentPanel.json` |
| 尺寸 | 宽随父级、高随父级（整列容器） |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `EquipSlot_Helmet` | `WBP_EquipmentSlot` | 头盔槽：设置图标、名称、选中 |
| `EquipSlot_Armor` | `WBP_EquipmentSlot` | 护甲槽 |
| `EquipSlot_Primary` | `WBP_EquipmentSlot` | 主武器槽 |
| `EquipSlot_Secondary` | `WBP_EquipmentSlot` | 副武器槽 |
| `EquipSlot_Melee` | `WBP_EquipmentSlot` | 近战武器槽 |
| `EquipSlot_Backpack` | `WBP_EquipmentSlot` | 背包槽 |
| `EquipSlot_ChestRig` | `WBP_EquipmentSlot` | 胸挂槽 |
| `EquipSlot_Headset` | `WBP_EquipmentSlot` | 耳机槽 |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。装备槽内部节点见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md)。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Panel_Equipment` | Border | 整列底板 + 15px 内边距，纯装饰 |
| `VB_Equipment` | VerticalBox | 列内布局 |
| `Text_EquipTitle` | TextBlock | 固定标题 `EQUIPMENT` |
| `Size_Divider_Equip` / `Divider_Equip` | SizeBox / Border | 分隔线 |
| `Row_Equip_1` … `Row_Equip_4` | HorizontalBox | 四行装备排布 |
| `Cell_Helmet` / `Cell_Armor` / `Cell_Primary` / `Cell_Secondary` / `Cell_Melee` / `Cell_Backpack` / `Cell_ChestRig` / `Cell_Headset` | VerticalBox | 单个装备单元 |
| `Text_Label_Helmet` / `Text_Label_Armor` / `Text_Label_Primary` / `Text_Label_Secondary` / `Text_Label_Melee` / `Text_Label_Backpack` / `Text_Label_ChestRig` / `Text_Label_Headset` | TextBlock | 固定槽位标签（头盔/护甲/主武器…） |

---

## 交互

无。纯显示控件，本身不接收事件。

> 装备槽单击行为见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md)；当前为「待定」，由持有方决定查看详情/卸下/不响应。

---

## 状态

本控件没有独立状态节点，所有表现都落在 8 个装备槽上：

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 空槽 | 装备槽的 `Img_EquipIcon` + `Text_EquipName` | 图标隐藏，文字「未装备」 |
| 已装备 | 装备槽的 `Img_EquipIcon` + `Text_EquipName` | 图标显示，文字为装备名 |
| 选中 | 装备槽的 `Border_Selected` | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `EquipSlot_Helmet` | `WBP_EquipmentSlot` | 设置头盔图标/名称/选中 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Armor` | `WBP_EquipmentSlot` | 设置护甲图标/名称/选中 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Primary` | `WBP_EquipmentSlot` | 设置主武器图标/名称/选中 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Secondary` | `WBP_EquipmentSlot` | 设置副武器图标/名称/选中 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Melee` | `WBP_EquipmentSlot` | 设置近战武器图标/名称/选中 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Backpack` | `WBP_EquipmentSlot` | 设置背包图标/名称/选中 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_ChestRig` | `WBP_EquipmentSlot` | 设置胸挂图标/名称/选中 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Headset` | `WBP_EquipmentSlot` | 设置耳机图标/名称/选中 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
