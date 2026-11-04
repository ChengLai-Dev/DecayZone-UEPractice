# WBP_EquipmentSlot 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_EquipmentSlot` |
| 父类 | `UUserWidget`（未接 C++） |
| 源文件 | `UIWidgets/UI/Inventory/WBP_EquipmentSlot.json` |
| 尺寸 | 宽随父级、高 136 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Border_Selected` | Border | 选中框（2px）。选中刷 `#C9A968`，未选中刷 `#2E3C44` |
| `Img_EquipIcon` | Image | 装备图标。默认隐藏，有装备时显示并设贴图 |
| `Text_EquipName` | TextBlock | 装备名。空槽时显示「未装备」 |

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Root_Size` | SizeBox | 槽位根节点，整槽命中区（暂不接事件，见交互） |
| `Size_Icon` | SizeBox | 图标显示区 44×44，固定 |
| `Fill_Slot` | Border | 槽底色，纯装饰 |
| `Spacer_Content` | Spacer | 把装备名压到底部 |
| `VB_Content` | VerticalBox | 图标在上、名称在下的布局容器 |

---

## 交互

| 节点 | 事件 | 程序要做什么 |
|---|---|---|
| `Root_Size`（槽位本体） | 左键单击 | **待定**：查看该装备详情 / 卸下 / 不响应 |

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 空槽 | `Img_EquipIcon` + `Text_EquipName` | 图标隐藏，文字「未装备」 |
| 已装备 | `Img_EquipIcon` + `Text_EquipName` | 图标显示，文字为装备名 |
| 选中 | `Border_Selected` | 2px `#C9A968` |
| 未选中 | `Border_Selected` | 2px `#2E3C44` |

> 槽位的**类型标签**（头盔 / 护甲 / 主武器…）不在本控件内，在 `WBP_InventoryPanel` 的 `Text_Label_*` 上，见 [WBP_InventoryPanel.md](WBP_InventoryPanel.md)。

---

## 含有的子控件

无。
