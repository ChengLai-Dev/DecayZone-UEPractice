# 装备系统（M1）交接总览

| 项 | 值 |
|---|---|
| 阶段 | M1 装备系统 |
| 状态 | ◐ 进行中（程序侧与资产侧已实现，待 PIE 验收） |
| 涉及界面 | `WBP_EquipmentPanel`、`WBP_EquipmentSlot`、`WBP_CharacterStats`、`WBP_InventoryPanel`、`WBP_ItemDetail` |
| 涉及数据 | `DT_ItemData`、`UDZ_AttributeSet::Armor`、`UDZ_InventoryComponent::MaxWeight` |
| 相关文档 | [WBP_EquipmentPanel.md](WBP_EquipmentPanel.md)、[WBP_EquipmentSlot.md](WBP_EquipmentSlot.md)、[WBP_CharacterStats.md](WBP_CharacterStats.md)、[WBP_ItemDetail.md](WBP_ItemDetail.md) |

---

## 1. 本阶段目标

- 四个槽**可装可卸**：主武器、近战武器、护甲、背包。入口两个：拖拽、详情页「装备」按钮。
- 护甲影响 `Armor` 属性（减伤值），卸下后回落。
- 装备面板下方显示角色属性：生命、能量、护甲、负重。
- 装备状态在服务器权威、客户端只读复制数据。

**不做**：其余 4 个槽（头盔/副武器/胸挂/耳机）、武器伤害/射速/弹药（M2）、装备计入负重（M1 负重只算背包物品）、`Armor` 的减伤公式（M2 伤害管线接入时才消费）。

---

## 2. 资产清单

| 资产 | 路径 | 状态 |
|---|---|---|
| `WBP_EquipmentPanel` | `/Game/UI/Inventory/` | JSON 已改为 4 槽 + 属性区，**待重新导入** |
| `WBP_EquipmentSlot` | `/Game/UI/Inventory/` | 已完成，父类 `UDZ_EquipmentSlot`（编译通过，`Text_EquipName` / `Img_EquipIcon` 已绑定） |
| `WBP_CharacterStats` | `/Game/UI/Inventory/` | JSON 已建，**待导入 + 待重绑父类** `UDZ_CharacterStats` |
| `T_UI_Icon_Armor` | `/Game/Assets/ItemIcons/Runtime/` | 已导入，`TC_EditorIcon` / 无 Mip / `TEXTUREGROUP_UI` / sRGB |
| `T_UI_Icon_Rifle` | `/Game/Assets/ItemIcons/Runtime/` | 已导入，同上；45° 斜构图 |
| `DT_ItemData` 两行 | `/Game/Data/ItemData/` | 已完成：`NewRow_3`（防弹背心 itemId 4）、`NewRow_4`（突击步枪 itemId 5），图标已指到对应贴图 |

> 近战和背包槽目前没有可拖的示例物品，M1 用护甲和步枪验收即可。

---

## 3. 数据契约

### 3.1 `FDZ_ItemData` 字段

| 字段 | 类型 | 默认 | 语义 |
|---|---|---|---|
| `ItemType` | `EItemType` | `Normal` | 只用于 UI 分类显示 |
| `EquipSlot` | `EEquipSlot` | `None` | `None` = 不可装备 |
| `ArmorBonus` | `float` | 0 | 护甲减伤值 |
| `Weight` | `float` | 0 | 单件重量，负重汇总用 |

### 3.2 唯一判定规则

- **能不能装备，只看 `EquipSlot != None`**。
- `ItemType` 不参与任何玩法判断。
- `EItemUseType::Equip` 在 M1 不使用，留给 M6 统一整理。

### 3.3 槽位索引

装备容器的下标 = `EEquipSlot` 数值：`Armor = 1`、`Primary = 2`、`Melee = 4`、`Backpack = 5`。枚举 8 个值全部保留，只允许往后追加。

### 3.4 新增数据行（占位数值，可直接改）

| 名称 | `ItemType` | `EquipSlot` | `ArmorBonus` | `Weight` |
|---|---|---|---|---|
| 防弹背心 | `Equipment` | `Armor` | 25 | 4.0 |
| 突击步枪 | `Equipment` | `Primary` | 0 | 3.5 |

### 3.5 负重

| 项 | 定义 |
|---|---|
| 当前负重 | `Σ 背包物品 Weight × Count`（M1 不含已装备物品） |
| 上限 | `UDZ_InventoryComponent::MaxWeight`，`EditDefaultsOnly`，默认 40.0 |
| 归属 | `GetTotalWeight()` 放在组件里，纯派生、无状态，客户端可调用 |

---

## 4. 数据流

```
拖拽 / 按钮
  → UDZ_InventoryPanel::OnItemMoveRequested（既有）
  → ADZ_PlayerController::HandleItemMoveRequested（既有）
  → ServerMoveItem（既有 RPC，ContainerComp 传 nullptr）
  → 服务器 ItemHelper::MoveItem：校验槽位 → 背包 ↔ 装备
  → UDZ_InventoryComponent 装备数组变更
      → OnEquipmentChanged（复制回客户端）
      → 服务器重算 Armor 基础值
  → Controller::RefreshUIAll
      → BackpackPanel / LootPanel / EquipmentPanel / CharacterStats / StatusBars
```

```
属性区
  → RefreshUIAll
     → InventoryPanel::SetCharacterStats(7 个值)
        → EquipmentPanel::SetCharacterStats(...)
           → CharacterStats 的四个 setter
```

原则：**客户端不判定结果，只发请求；数据只从复制值读取。**

---

## 5. 程序侧接口（已实现）

### 5.1 属性

- ✅ `UDZ_AttributeSet` 的 `Armor` 已有 `ATTRIBUTE_ACCESSORS_BASIC` 与 `OnRep_Armor`。
- ✅ 服务器侧重算走 GE，不用 `SetNumericAttributeBase`：`ADZ_PlayerCharacter::RecalculateArmor()` 按 `Σ ArmorBonus` 用 SetByCaller Apply `GE_EquipmentArmor`。

### 5.2 存储

- ✅ `UDZ_InventoryComponent` 固定长度 8 的装备数组（`TArray<int32>`），下标 = `EEquipSlot`。
- ✅ 复制 + `OnEquipmentsChanged` 委托（`FOnEvent`）。
- ✅ 读写接口：`GetEquipments` / `SetEquipments` / `ClearEquipment`（全部服务器权威）。
- ✅ `MaxWeight`、`GetMaxWeight()`、`GetTotalWeight()`、`GetTotalArmor()`、`CanAddWeight()`。

### 5.3 规则

- ✅ `ItemHelper::MoveItem` 增加 `Backpack → Equipment`、`Equipment → Backpack`、`Loot → Equipment`、`Equipment → Loot`。
- ✅ 校验：目标槽必须是 `Armor` / `Primary` / `Melee` / `Backpack` 之一，且物品 `EquipSlot` 与目标槽一致。
- ✅ 目标槽非空时与背包格交换。
- ✅ 校验失败时两边都不动，返回 false。
- ✅ 负重检查只拦净增分支，见 5.6。

### 5.4 界面

- ✅ `UDZ_EquipmentSlot` 继承 `UDZ_ItemSlotBase`，实现 `SetItemInfo` / `SetEmpty` / `CreateDragVisualWidget`。
- ✅ `UDZ_EquipmentPanel` 绑定 4 个槽 + `CharacterStats`，实现 `SetEquipmentInfo` / `SetCharacterStatsInfo`，聚合 `OnItemDrop`。
- ✅ `UDZ_CharacterStats` 新建，4 个 `BindWidget` + 4 个 setter。
- ✅ `UDZ_InventoryPanel` / `UDZ_LootPanel` 绑定 `EquipmentPanel`，转发 `OnItemDrop` 与 `SetCharacterStatsInfo`。
- ✅ `ADZ_PlayerController::RefreshUIAll` 补装备区与属性区刷新。
- ✅ `OnRep_PlayerState` 补 `Armor` 的委托，护甲变化触发 `RefreshUIAll`。

### 5.5 编辑器内动作

已完成：

1. ✅ `WBP_EquipmentSlot` 父类改 `UDZ_EquipmentSlot`（编译通过，两个 BindWidget 均绑定成功）。
2. ✅ `DT_ItemData` 已含 `itemType` / `slot` / `armorBonus` 三列，现有 4 行默认值正常。
3. ✅ 两张图标已导入并按既有设置配好（`TC_EditorIcon` / 无 Mip / `TEXTUREGROUP_UI` / sRGB），已保存。
4. ✅ `WBP_CharacterStats` 已生成，父类重绑为 `UDZ_CharacterStats`，4 个 `BindWidget` 全部绑定成功。
5. ✅ `WBP_EquipmentPanel` 已生效为 4 槽 + 属性区结构，父类 `UDZ_EquipmentPanel`。
6. ✅ `GE_EquipmentArmor` 已建：`/Game/AbilitySystem/GameplayEffects/`，Infinite + `Armor` + `AddBase` + SetByCaller（tag `SetByCaller.Armor`）。
7. ✅ `BP_DZ_PlayerCharacter` 的 `EquipmentArmorGEClass` 已指向 `GE_EquipmentArmor`（配在 Character 上，与 `InitializeGEClass` / `EnergyRegenGEClass` 并列）。

### 5.6 护甲与负重的实现约定

**职责分层**

- `UDZ_InventoryComponent` 是纯数据组件：只管 `Items` / `Equipments` / 负重 / 护甲汇总，**不依赖 GAS**。
- 数据变更通过 `OnEquipmentsChanged` 广播，组件不知道谁在订阅、拿来干什么。
- GE 的施加归 ASC 的拥有者 `ADZ_PlayerCharacter`，与 `InitializeGEClass` / `EnergyRegenGEClass` 同一处。

**护甲**

- `UDZ_InventoryComponent::GetTotalArmor()`：纯派生，遍历 `Equipments` 求 `Σ ArmorBonus`，和 `GetTotalWeight()` 对称。
- 广播点三个：`SetEquipments`、`ClearEquipment`、`MoveItem` 的「背包 → 装备、槽已占用」交换分支。
- `ADZ_PlayerCharacter::RecalculateArmor()`：先移除旧 GE，再按 `GetTotalArmor()` 用 SetByCaller Apply 新的 `GE_EquipmentArmor`；开头 `HasAuthority()` 判断，客户端不算。
- 调用时机：`BeginPlay` 绑 `OnEquipmentsChanged`；`PossessedBy` 末尾算一次（覆盖 M8 开局带入装备）；之后每次委托触发。
- 只有一个 `FActiveGameplayEffectHandle`，不做「每件装备一个 Handle」。
- **`Armor` 的 BaseValue 只允许装备系统写。任何 GE 都不许带 `Armor` 修改器。** M5 护盾技能用独立属性或 `DamageModifier`，不复用 `Armor`，否则双算。

**负重**

- 只拦「净增加背包重量」的操作：Loot → Backpack、Equipment → Backpack、换装时旧装备回背包。
- 超重一律整次操作失败，不做部分成功、不做惩罚。
- 整堆判定，不拆分。
- `GetTotalWeight()` 每次遍历求和，不做增量缓存。
- `MaxWeight` 通过 `GetMaxWeight()` 读取，不直接读字段（留给「背包槽加负重上限」的口子）。
- 比较用 `<= MaxWeight + KINDA_SMALL_NUMBER`，避免浮点边界误判。

---

## 6. 当前未完成

| 项 | 状态 |
|---|---|
| C++ 全部实现 | ✅ 已完成并编译通过 |
| 两个 WBP 的导入与重绑 | ✅ 已完成 |
| `GE_EquipmentArmor` 与 Character 配置 | ✅ 已完成 |
| **PIE 验收** | ⬜ 待跑第 7 节验收表 |
| 空槽拖拽高亮 | M1 不做 |
| 头盔 / 副武器 / 胸挂 / 耳机槽 | 枚举保留，UI 不再显示 |
| 重量显示小数位 | `FDZ_CharacterStatsInfo` 目前全 `int32`，重量会被截断（7.5 显示 7）。要 1 位小数得把 `Weight` / `MaxWeight` 改 `float` 并改 `SetTextValue` 的格式化 |

### 6.1 编辑器侧备注（给写 C++ 的你）

- `DT_ItemData` 的 `icon` 列通过 `set_rows` 写入时必须传**路径字符串**（`/Game/Assets/ItemIcons/Runtime/T_UI_Icon_Armor.T_UI_Icon_Armor`），传 `{"refPath": ...}` 会被静默写成 `None`。
- `WBP_EquipmentSlot` 现在继承 `UDZ_EquipmentSlot`（当前继承链是 `UDZ_UserWidget`）。按契约再改成继承 `UDZ_ItemSlotBase` 时，WBP 不需要任何改动。
- `WBP_EquipmentPanel` 的 `VB_Equipment` 里，`Spacer_Fill` 负责把 `CharacterStats` 压到底部；不要把它改成固定高度，否则属性区会跟着内容飘到中间。

---

## 7. 验收

| 场景 | 期望 |
|---|---|
| 打开背包 | 左侧 4 个槽 + 下方 CHARACTER 四行，数值非空 |
| 背包拖护甲到护甲槽 | 槽显示图标+名称，背包原格清空，护甲行 = 25 |
| 护甲槽拖回背包 | 槽恢复「未装备」，护甲行 = 0 |
| 选护甲点「装备」 | 与拖拽结果一致 |
| 药水拖到护甲槽 | 拒绝，两边都不动 |
| 步枪拖到主武器槽 | 占槽成功，护甲行不变 |
| 捡起 / 丢弃物品 | 负重行跟着变 |
| 双人 PIE | 双方看到对方装备与属性正确 |
| 反复开关背包 | 装备状态不丢、属性不重复叠加 |
