# WBP_SettlementItemSlot 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Settlement/WBP_SettlementItemSlot` |
| 父类 | `UDZ_SettlementItemSlot`（C++ 已实现） |
| 源文件 | `UIWidgets/UI/Settlement/WBP_SettlementItemSlot.json` |
| 尺寸 | 88×88 固定 |
| 使用处 | `WBP_SettlementPanel` 的 `WrapBox_Items` 内，运行时动态生成 |
| 性质 | **只读**。不可拖拽、不可点击、不可拿取 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Img_ItemIcon` | `Image` | 设置 `FDZ_ItemData::Icon`；有图标时 `Visible`，无图标保持 `Collapsed` |
| `Text_Quantity` | `TextBlock` | 写 `xN`；`N <= 1` 时设为 `Collapsed` |

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Root_Size` | SizeBox | 88×88 |
| `Border_Outline` / `BG_Slot` | Border | 1px 描边 + 格子底 |
| `Overlay_Slot` | Overlay | 图标与数量角标的叠放容器 |

---

## 交互

无。这是纯展示格，**不要继承 `UDZ_ItemSlotBase`**，也不要绑定拖拽委托。结算界面里任何拖拽都会破坏「已经带出」这个语义。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 有图标 | `Img_ItemIcon` | `Visible`，填充格子（四周留 8px） |
| 无图标 | `Img_ItemIcon` | 保持 `Collapsed`，只显示底色 |
| 数量 > 1 | `Text_Quantity` | `Visible`，显示 `xN` |
| 数量 = 1 | `Text_Quantity` | `Collapsed` |
| 失败结算 | `BG_Slot` | 可由父面板统一把描边/底色降饱和度，表示物品已丢失 |

---

## 已实现的接口

| 接口 | 参数 | 说明 |
|---|---|---|
| `SetItem` | `int32 ItemId`、`int32 Count` | 内部用 `ItemHelper::GetItemData` 查图标；`Count <= 1` 隐藏角标 |

---

## 注意事项

- 格子尺寸固定 88×88，父级 `WrapBox` 的 `InnerSlotPadding` 已设 `(10, 10)`（X=水平、Y=垂直），不要重复加 margin。
- 悬停显示详情（`WBP_ItemDetail`）**本期不做**。如果后续要加，走 `UDZ_UserWidget` 的 `OnWidgetLeftClicked` / hover 事件，别在格子里自己写命中检测。
