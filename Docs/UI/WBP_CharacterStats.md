# WBP_CharacterStats 交互文档

背包界面左下角的角色属性区，显示生命、能量、护甲、负重。

## 1. 资产

| 项 | 值 |
|---|---|
| 资产路径 | `/Game/UI/Inventory/WBP_CharacterStats` |
| JSON 源 | `UIWidgets/UI/Inventory/WBP_CharacterStats.json` |
| 父类 | `UDZ_CharacterStats`（**待建**；未建前导入时先用 `UserWidget`，之后重绑） |
| 设计尺寸 | `Desired` |
| 挂载位置 | `WBP_EquipmentPanel` 的 `VB_Equipment` 末尾，`CharacterStats` 节点 |

## 2. 控件树

```
Panel_Stats (Border, Padding 12, 底色 #04070A 之上的一层)
└─ VB_Stats (VerticalBox)
   ├─ Text_StatsTitle      "CHARACTER"
   ├─ Size_Divider_Stats (SizeBox h=1, top 9 / bottom 11)
   │  └─ Divider_Stats (Border 1px)
   ├─ Row_Health (HorizontalBox, bottom 7)
   │  ├─ Text_Label_Health  "生命"      静态
   │  ├─ Spacer_Health      Fill
   │  └─ Text_HealthValue   "100 / 100"
   ├─ Row_Energy (bottom 7)
   │  ├─ Text_Label_Energy  "能量"      静态
   │  ├─ Spacer_Energy      Fill
   │  └─ Text_EnergyValue   "100 / 100"
   ├─ Row_Armor (bottom 7)
   │  ├─ Text_Label_Armor   "护甲"      静态
   │  ├─ Spacer_Armor       Fill
   │  └─ Text_ArmorValue    "0"
   └─ Row_Weight
      ├─ Text_Label_Weight  "负重"      静态
      ├─ Spacer_Weight      Fill
      └─ Text_WeightValue   "0.0 / 40.0"
```

## 3. 程序要绑定的节点

| 节点 | 类型 | 用途 |
|---|---|---|
| `Text_HealthValue` | `UTextBlock` | `当前 / 上限` |
| `Text_EnergyValue` | `UTextBlock` | `当前 / 上限` |
| `Text_ArmorValue` | `UTextBlock` | 护甲值，整数 |
| `Text_WeightValue` | `UTextBlock` | `当前 / 上限`，1 位小数 |

`Text_StatsTitle` 与四个 `Text_Label_*` 是静态文案，不需要代码写入。

## 4. 数据流

| 数据 | 来源 | 权威侧 |
|---|---|---|
| 生命 | `UDZ_AttributeSet::Health` / `MaxHealth` | 服务器复制 |
| 能量 | `UDZ_AttributeSet::Energy` / `MaxEnergy` | 服务器复制 |
| 护甲 | `UDZ_AttributeSet::Armor` | 服务器复制 |
| 负重 | `UDZ_InventoryComponent::GetTotalWeight()` / `MaxWeight` | 当前重量由复制的背包物品派生 |

```
ADZ_PlayerController::RefreshUIAll
  → InventoryPanel::SetCharacterStats(...)
     → EquipmentPanel::SetCharacterStats(...)
        → CharacterStats::SetHealthInfo / SetEnergyInfo / SetArmorInfo / SetWeightInfo
```

**不在 UI 里查数据表，不自己判定规则。** 负重汇总由组件提供，UI 只负责显示。

## 5. 显示格式

| 行 | 格式 | 示例 |
|---|---|---|
| 生命 | `%.0f / %.0f` | `120 / 120` |
| 能量 | `%.0f / %.0f` | `80 / 100` |
| 护甲 | `%.0f` | `25` |
| 负重 | `%.1f / %.1f` | `12.5 / 40.0` |

## 6. 程序侧未完成项

1. 新建 `UDZ_CharacterStats : UDZ_UserWidget`，声明上面 4 个 `BindWidget`。
2. 实现 `SetHealthInfo` / `SetEnergyInfo` / `SetArmorInfo` / `SetWeightInfo`，签名与 `UDZ_StatusBars` 保持一致。
3. `UDZ_EquipmentPanel` 增加 `CharacterStats` 的 `BindWidget` 与转发方法。
4. `UDZ_InventoryPanel` 增加 `EquipmentPanel` 的 `BindWidget` 与转发方法。
5. `UDZ_InventoryComponent` 增加 `MaxWeight`（`EditDefaultsOnly`，默认 40）与 `GetTotalWeight()`。
6. `ADZ_PlayerController::RefreshUIAll` 里补一次 `SetCharacterStats` 调用。
7. `Armor` 属性变化要能触发刷新：`OnRep_PlayerState` 里加 `Armor` 的委托。
8. `UDZ_AttributeSet` 的 `Armor` 还缺 `ATTRIBUTE_ACCESSORS_BASIC` 与 `OnRep_Armor` 实现。

## 7. 验收

| 场景 | 期望 |
|---|---|
| 打开背包 | 属性区四行都有值，生命/能量与 HUD 一致 |
| 装上护甲 | 护甲行变成 25 |
| 卸下护甲 | 护甲行回落 0 |
| 捡起 / 丢弃物品 | 负重行跟着变 |
| 受伤 / 回能 | 生命、能量行与 HUD 同步 |
| 属性为 0 | 显示 `0`，不留空 |
