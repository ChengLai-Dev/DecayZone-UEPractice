# WBP_ItemSlot 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_ItemSlot` |
| 父类 | `UDZ_ItemSlot` |
| 源文件 | `UIWidgets/UI/Inventory/WBP_ItemSlot.json` |
| 尺寸 | 128×114 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Root_Size` | SizeBox | 格子根节点，整个 128×114 都是鼠标命中区 |
| `Border_Selected` | Border | 选中框（2px）。选中刷 `#C9A968`，未选中刷 `#2E3C44` |
| `Img_ItemIcon` | Image | 物品图标。默认隐藏，有物品时显示并设贴图 |
| `Text_Quantity` | TextBlock | 右下角数量文本 |

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Size_ItemIcon` | SizeBox | 图标显示区 92×92，固定 |
| `Fill_Slot` | Border | 空槽底色，纯装饰 |
| `Overlay_Slot` | Overlay | 图标与数量的叠放容器 |

---

## 交互

| 节点 | 事件 | 程序要做什么 |
|---|---|---|
| `Root_Size`（格子本体） | 左键单击 | 通知面板「这个格子被点了」，选中逻辑归面板 |

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 空槽 | `Img_ItemIcon` | 隐藏，只显示空槽底 |
| 有物品 | `Img_ItemIcon` | 显示 + 指定贴图 |
| 选中 | `Border_Selected` | 2px `#C9A968` |
| 未选中 | `Border_Selected` | 2px `#2E3C44` |

---

## 含有的子控件

无。
