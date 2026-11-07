# WBP_EquipmentSlot 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_EquipmentSlot` |
| 父类 | `UDZ_EquipmentSlot`（已重绑，编译通过） |
| 源文件 | `UIWidgets/UI/Inventory/WBP_EquipmentSlot.json` |
| 尺寸 | 宽随父级（依赖父级 `VerticalBoxSlot` 的 `HAlign_Fill`）、高 136 |

---

## M1 范围

M1 只显示 4 个槽：主武器（`Primary`）、近战武器（`Melee`）、护甲（`Armor`）、背包（`Backpack`），全部接线。头盔 / 副武器 / 胸挂 / 耳机 4 个槽不再出现在界面上，但 `EEquipSlot` 枚举值保留。

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Img_EquipIcon` | Image | 装备图标。默认隐藏，有装备时显示并设贴图 |
| `Text_EquipName` | TextBlock | 装备名。空槽时显示「未装备」 |
| `Border_Selected` | Border | 选中框（2px）。**M1 不使用**，保留给后续「点击查看详情」 |

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Root_Size` | SizeBox | 槽位根节点，整槽命中区，也是拖放目标区域 |
| `Size_Icon` | SizeBox | 图标显示区 92×92，居中，固定 |
| `Fill_Slot` | Border | 槽底色，纯装饰 |
| `Spacer_Content` | Spacer | 把装备名压到底部 |
| `VB_Content` | VerticalBox | 图标在上、名称在下的布局容器 |

---

## 交互

| 触发 | 事件 | 程序要做什么 |
|---|---|---|
| 背包道具拖到本槽并松手 | `NativeOnDrop` → 父类广播 `OnWidgetDragEnded(Source, this)` | 把「来源句柄 + 本槽句柄」交给面板聚合，最终由 Controller 发 Server RPC |
| 从本槽按下左键拖出 | 父类 `NativeOnDragDetected`（需 `SetDragEnabled(true)`） | 拖拽视觉复用 `WBP_ItemSlot`；空槽不允许拖 |
| 左键单击 | `OnWidgetLeftClicked` | **M1 不处理**，保留 |

> 槽位不是 `UDZ_ItemSlot`，但要能和背包/搜刮面板互拖，所以 `UDZ_EquipmentSlot` 继承 `UDZ_ItemSlotBase` 以获得 `FDZ_ItemSlotHandle`（`Container = Equipment`，`SlotIndex = EEquipSlot` 数值）。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 空槽 | `Img_EquipIcon` + `Text_EquipName` | 图标 `Collapsed`，文字「未装备」 |
| 已装备 | `Img_EquipIcon` + `Text_EquipName` | 图标 `Visible` 并设贴图，文字为装备名 |
| 不可放置（拖拽悬停） | 无 | **M1 不做高亮反馈**；放下时由服务器校验并拒绝 |

---

## 程序侧待实现（M1）

1. `UDZ_EquipmentSlot` 改继承 `UDZ_ItemSlotBase`。
2. 新增 `SetItemInfo(const FDZ_ItemStack&)` / `SetEmpty()`，内部只改上面两个节点。
3. 构造函数或 `NativeOnInitialized` 里 `SetDragEnabled(true)`，并实现 `CreateDragVisualWidget()`（复用 `WBP_ItemSlot`）。
4. `NativeOnDrop` 先调父类（触发 `OnWidgetDragEnded`），空槽时返回 false 不拦截。
5. ✅ 已完成：`WBP_EquipmentSlot` 父类已改为 `UDZ_EquipmentSlot` 并编译保存。

---

## 含有的子控件

无。
