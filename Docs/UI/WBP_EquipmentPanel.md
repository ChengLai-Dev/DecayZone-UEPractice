# WBP_EquipmentPanel 交互文档

背包界面左侧的装备列：上方 4 个装备槽，下方角色属性区。

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_EquipmentPanel` |
| 父类 | `UDZ_EquipmentPanel`（已重绑，编译通过） |
| 源文件 | `UIWidgets/UI/Inventory/WBP_EquipmentPanel.json` |
| 尺寸 | 宽随父级（占背包面板 28%）、高随父级 |

---

## 1. 结构

```
Panel_Equipment (Border)
└─ VB_Equipment
   ├─ Text_EquipTitle      "EQUIPMENT"
   ├─ Size_Divider_Equip → Divider_Equip
   ├─ Row_Equip_1
   │  ├─ Cell_Primary   → Text_Label_Primary "主武器" + EquipSlot_Primary
   │  └─ Cell_Melee     → Text_Label_Melee   "近战武器" + EquipSlot_Melee
   ├─ Row_Equip_2
   │  ├─ Cell_Armor     → Text_Label_Armor   "护甲" + EquipSlot_Armor
   │  └─ Cell_Backpack  → Text_Label_Backpack "背包" + EquipSlot_Backpack
   ├─ Spacer_Fill         吃掉剩余高度，把属性区压到底部
   └─ CharacterStats      WBP_CharacterStats（见 WBP_CharacterStats.md）
```

原来是 4 行 8 槽，M1 精简为 2 行 4 槽，腾出的空间给属性区。

---

## 2. 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `EquipSlot_Primary` | `UDZ_EquipmentSlot` | 主武器槽：设置图标/名称、拖放目标 |
| `EquipSlot_Melee` | `UDZ_EquipmentSlot` | 近战武器槽：同上 |
| `EquipSlot_Armor` | `UDZ_EquipmentSlot` | 护甲槽：同上 |
| `EquipSlot_Backpack` | `UDZ_EquipmentSlot` | 背包槽：同上 |
| `CharacterStats` | `UDZ_CharacterStats` | 属性区：转发刷新 |

## 3. 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Panel_Equipment` | Border | 整列底板 + 15px 内边距，纯装饰 |
| `VB_Equipment` | VerticalBox | 列内布局 |
| `Text_EquipTitle` | TextBlock | 固定标题 `EQUIPMENT` |
| `Size_Divider_Equip` / `Divider_Equip` | SizeBox / Border | 分隔线 |
| `Row_Equip_1` / `Row_Equip_2` | HorizontalBox | 两行装备排布 |
| `Cell_*` | VerticalBox | 单个装备单元 |
| `Text_Label_*` | TextBlock | 固定槽位标签 |
| `Spacer_Fill` | Spacer | 布局占位 |

---

## 4. 交互

| 触发 | 在哪 | 程序要做什么 |
|---|---|---|
| 装备槽拖入 / 拖出 | 槽内部 | 槽广播 `OnWidgetDragEnded(Source, Target)`，面板聚合成 `OnItemDrop(From, To)` |
| 面板对外事件 | `UDZ_EquipmentPanel::OnItemDrop` | 由 `WBP_InventoryPanel` 在 `NativeOnInitialized` 里接住，转成自身的 `OnItemMoveRequested`，最终由 Controller 发 Server RPC |

**客户端不判断"这件物品能不能放进这个槽"**，只负责把来源句柄和目标句柄交上去；合法性由服务器校验，失败则两边都不动。

---

## 5. 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 空槽 | 装备槽的 `Img_EquipIcon` + `Text_EquipName` | 图标隐藏，文字「未装备」 |
| 已装备 | 装备槽的 `Img_EquipIcon` + `Text_EquipName` | 图标显示，文字为装备名 |
| 属性变化 | `CharacterStats` 内部 | 见 [WBP_CharacterStats.md](WBP_CharacterStats.md) |

---

## 6. 槽位索引约定

`FDZ_ItemSlotHandle.SlotIndex` 在装备容器下直接使用 `EEquipSlot` 的数值：

| `EEquipSlot` | 数值 | M1 |
|---|---|---|
| `Helmet` | 0 | — |
| `Armor` | 1 | 接线 |
| `Primary` | 2 | 接线 |
| `Secondary` | 3 | — |
| `Melee` | 4 | 接线 |
| `Backpack` | 5 | 接线 |
| `ChestRig` | 6 | — |
| `Headset` | 7 | — |
| `None` | 8 | 不可装备标记 |

> 枚举 8 个值全部保留，只是 UI 不再显示其中 4 个。以后加回槽位只需补 JSON 节点，不动枚举和已存数据。
> 新增枚举值时只能往后追加，不能插队，否则已存的槽位索引会整体错位。

---

## 7. 程序侧完成情况（M1）

1. ✅ `UDZ_EquipmentPanel` 声明 4 个槽 + `CharacterStats` 的 `BindWidget`。
2. ✅ `SetEquipmentInfo(const TArray<int32>& Equipments)`：按 `EEquipSlot` 数值取下标，刷 4 个槽。
3. ✅ `SetCharacterStatsInfo(const FDZ_CharacterStatsInfo&)`：转发给 `CharacterStats`。
4. ✅ `NativeOnInitialized` 里把 4 个槽的 `OnWidgetDragEnded` 接到 `HandleEquipSlotDrop`，聚合成 `OnItemDrop`。
5. ✅ `UDZ_InventoryPanel` / `UDZ_LootPanel` 已有 `EquipmentPanel` 的 `BindWidget`，接住 `OnItemDrop` 与 `SetCharacterStatsInfo`。
6. ✅ `WBP_EquipmentPanel` 父类已重绑为 `UDZ_EquipmentPanel`。

---

## 8. 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `EquipSlot_Primary` | `WBP_EquipmentSlot` | 设置图标/名称、拖放目标 | [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Melee` | `WBP_EquipmentSlot` | 同上 | [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Armor` | `WBP_EquipmentSlot` | 同上 | [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `EquipSlot_Backpack` | `WBP_EquipmentSlot` | 同上 | [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `CharacterStats` | `WBP_CharacterStats` | 转发属性刷新 | [WBP_CharacterStats.md](WBP_CharacterStats.md) |
