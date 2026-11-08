# WBP_CharacterStats 交互文档

背包界面左下角的角色属性区，显示生命、能量、护甲、负重。

## 1. 资产

| 项 | 值 |
|---|---|
| 资产路径 | `/Game/UI/Inventory/WBP_CharacterStats` |
| JSON 源 | `UIWidgets/UI/Inventory/WBP_CharacterStats.json` |
| 父类 | `UDZ_CharacterStats`（已重绑并编译通过，4 个 `BindWidget` 均绑定成功） |
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
  → InventoryPanel::SetCharacterStatsInfo(FDZ_CharacterStatsInfo)
     → EquipmentPanel::SetCharacterStatsInfo(...)
        → CharacterStats::SetHealth / SetEnergy / SetArmor / SetWeight
```

**不在 UI 里查数据表，不自己判定规则。** 负重汇总由组件提供，UI 只负责显示。

## 5. 显示格式

| 行 | 格式 | 示例 |
|---|---|---|
| 生命 | `%.0f / %.0f` | `120 / 120` |
| 能量 | `%.0f / %.0f` | `80 / 100` |
| 护甲 | `%.0f` | `25` |
| 负重 | `%.1f / %.1f` | `12.5 / 40.0` |

## 6. 程序侧完成情况

1. ✅ `UDZ_CharacterStats`：4 个 `BindWidget` + `SetHealth` / `SetEnergy` / `SetArmor` / `SetWeight`，私有 `SetTextValue` 统一格式化。
2. ✅ `UDZ_EquipmentPanel`：`CharacterStats` 的 `BindWidget` + `SetCharacterStatsInfo(const FDZ_CharacterStatsInfo&)`。
3. ✅ `UDZ_InventoryPanel` / `UDZ_LootPanel`：`SetCharacterStatsInfo` 转发。
4. ✅ `UDZ_InventoryComponent`：`MaxWeight`、`GetMaxWeight()`、`GetTotalWeight()`、`CanAddWeight()`。
5. ✅ `ADZ_PlayerController::RefreshUIAll`：组装 `FDZ_CharacterStatsInfo` 并下发；`OnRep_PlayerState` 已加 `Armor` 委托，属性变化统一走 `RefreshUIAll`。
6. ✅ `UDZ_AttributeSet`：`Armor` 的 `ATTRIBUTE_ACCESSORS_BASIC` 与 `OnRep_Armor` 均已就绪。

### 已知偏差

- `FDZ_CharacterStatsInfo` 的 7 个字段目前全是 `int32`，`SetTextValue` 也按整数格式化，所以**负重会丢掉小数**（7.5 显示成 7），与第 5 节的 `%.1f` 不符。要修就把 `Weight` / `MaxWeight` 改 `float`，`SetWeight` 单独走一位小数格式化。

## 7. 验收

| 场景 | 期望 |
|---|---|
| 打开背包 | 属性区四行都有值，生命/能量与 HUD 一致 |
| 装上护甲 | 护甲行变成 25 |
| 卸下护甲 | 护甲行回落 0 |
| 捡起 / 丢弃物品 | 负重行跟着变 |
| 受伤 / 回能 | 生命、能量行与 HUD 同步 |
| 属性为 0 | 显示 `0`，不留空 |
