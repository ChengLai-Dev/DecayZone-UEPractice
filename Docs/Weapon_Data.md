# 武器数据层设计（M2）

> 状态：字段已定稿，等待代码与资产落地
> 最后更新：2026-11-09

## 1. 存放方式

武器参数内嵌进 `FDZ_ItemData`，作为 `FDZ_WeaponParams` 子结构，不新建第二张表。

理由：项目已有 `UDZ_GameInstance::ItemMap`（`ItemId → FDZ_ItemData*`）与 `ItemHelper::GetItemData()` 这条唯一查询通路，内嵌字段随行指针一起返回，不需要任何新的查询基础设施，也不需要维护第二份 `ItemId` 对齐。

判定武器是枪还是近战，直接看 `FDZ_ItemData::EquipSlot`：`Primary` 为枪，`Melee` 为近战。不额外增加 `WeaponType` 字段。

## 2. 字段清单

| 字段 | 类型 | 语义 |
|---|---|---|
| `Weapon.Damage` | float | 单次命中伤害 |
| `Weapon.Range` | float | 近战 = 判定球半径；枪 = 射线长度 |
| `Weapon.AttackAngle` | float | 近战扇形总张角（度）；枪填 0，不参与判定 |
| `Weapon.AttackCooldown` | float | 两次攻击的最小间隔，秒 |
| `Weapon.AttackMontage` | `TSoftObjectPtr<UAnimMontage>` | 攻击蒙太奇 |
| `Weapon.bAutomatic` | bool | 枪是否按住连发；近战填 false |

非武器行（医疗包、药水、子弹、手雷）保留全默认值，不填。

### 2.1 时序不放进表里

攻击的时序（判定窗口从动画第几秒开到第几秒）**不进数据表**，由蒙太奇资产上的动画通知承载：

- 近战：`AnimNotifyState` 界定持续判定窗口，`NotifyBegin` 开、`NotifyEnd` 关
- 枪：`AnimNotify` 单点触发一次射线

这样改动画时窗口自动跟着走，不需要回表里重填秒数。

### 2.2 为什么窗口能放在服务器

`ACharacter` 构造函数把 mesh 的 `VisibilityBasedAnimTickOption` 设为 `AlwaysTickPose`（`Engine/Private/Character.cpp`）。该值的语义是「始终 tick 动画，但只在渲染时刷新骨骼变换」，因此服务器上蒙太奇会推进、动画通知会触发，只是 socket 位置不更新。

结论：**通知驱动窗口在服务器可用**；依赖 socket 世界位置的方案（剑碰撞体）在服务器不可用，本方案不涉及。

## 3. 近战判定规则

1. 以角色位置为球心、`Range` 为半径做一次球形重叠，通道用 `ECC_Pawn`。
2. 对每个候选目标计算「角色朝向」与「角色→目标方向」的夹角，超过 `AttackAngle / 2` 的剔除。
3. 夹角计算只在水平面（XY）进行，忽略 Z，避免坡地或浮空目标被判到扇形外。
4. 角色与目标的距离小于约 20cm 时跳过角度过滤直接判定命中——此时方向向量退化，点积无意义。
5. 窗口开始时记录一次角色朝向，整个窗口都用这个快照，避免挥砍中途转身扫到背后。
6. 维护一个本次攻击的已命中集合，命中后登记，每个目标每次攻击只结算一次；窗口结束清空该集合。
7. 窗口内每帧重复上述检测，命中后不停止检测。

因为持续检测需要每次攻击独立的状态，GA 的 `InstancingPolicy` 不能是默认的 `NonInstanced`（所有 actor 共用 CDO，状态会互相污染），应改为 `InstancedPerActor`。

## 4. 枪械判定规则

1. `AnimNotify` 触发一次射线检测，通道用 `ECC_Pawn`。
2. 起点建议取相机位置，方向取相机前向，保证准星所指即所中。
3. 命中对象若为 `ADZ_BaseCharacter` 且不是自己，结算伤害；否则视为打中场景。
4. 攻击间隔由 `AttackCooldown` 节流，蒙太奇只负责表现，允许被下一次攻击打断重播。

## 5. 碰撞通道注意事项

`BaseEngine.ini` 中 `Pawn` 碰撞预设的定义为：

```
ObjectTypeName="Pawn", CustomResponses=((Channel="Visibility", Response=ECR_Ignore))
```

**Pawn 对 `Visibility` 通道是 Ignore**，因此用 `LineTraceSingleByChannel(ECC_Visibility)` 打不到角色胶囊，且不会有任何报错。近战重叠与枪械射线都使用 `ECC_Pawn`；场景静态物（BlockAll）对 `Pawn` 通道为 Block，所以子弹仍会被墙挡住，符合预期。

项目目前只注册了一个自定义通道 `Interactable`（`ECC_GameTraceChannel1`），未注册战斗专用通道。M2 不新增。

前置检查：确认 `BP_DZ_Zombie` 的胶囊碰撞预设仍为 `Pawn`，若已改为自定义预设，上述两条判定会静默失效。

## 6. 伤害与减伤（元属性方案）

### 6.1 结构

伤害不直接写 `Health`，而是写入一个只存在于服务器的**元属性** `IncomingDamage`，由 `UDZ_AttributeSet::PostGameplayEffectExecute` 统一折算后扣 `Health`。

```
伤害 GE ──> IncomingDamage（只加不减）
                    │
        PostGameplayEffectExecute（服务器）
                    │
        1. 读本次伤害
        2. 按 Armor 折算
        3. 从 Health 扣掉
        4. IncomingDamage 归零
        5. 判定死亡
```

治疗类 GE（M6 药水、医疗包）直接写 `Health`，天然不经过这条通路，不会被减伤。

### 6.2 元属性声明要点

- 名字：`IncomingDamage`，`FGameplayAttributeData`，用 `ATTRIBUTE_ACCESSORS_BASIC` 生成访问器
- **不加入 `GetLifetimeReplicatedProps`**：纯服务器中转值，复制出去无意义
- 默认值 0

### 6.3 折算规则

```
最终伤害 = 本次伤害 × (1 - Clamp(Armor, 0, 80) / 100)
```

- `Armor` 属性存的是 `Σ ArmorBonus`（`25` 表示减伤 25%），由 `ADZ_PlayerCharacter::RecalculateArmor()` 写入
- **封顶 80%**：夹的是折算时用的值，不是 `Armor` 属性本身。属性说真话（可显示 100），减伤自己保底
- 多件护甲**相加**：保持 `UDZ_InventoryComponent::GetTotalArmor()` 现有求和语义，不改为相乘

### 6.4 为什么不用 `PreAttributeChange`

`PreAttributeChange` 拿不到伤害原始值与来源 GE，且对代码里任何直接写入也会触发，把减伤写进去会连回血一起减。`PostGameplayEffectExecute` 只在 GE 执行后触发，能拿到 `Data.EvaluatedData` 与 `Data.EffectSpec`，且修改属性的安全性最高。

### 6.5 死亡判定位置

`PostGameplayEffectExecute` 扣完血后判定 `Health <= 0`，但**不在此处直接写死亡逻辑**，而是通知角色侧处理（避免 `AttributeSet` 耦合具体角色类型）。角色侧需要一个「已死亡」标志位，防止尸体被重复命中时重复触发。

## 7. 数值参考

`GE_EnemyInitialize` 实测配置：`MaxHealth = 100`，`Health = 100`。下表按此反推。

| 武器 | Damage | Range | AttackAngle | AttackCooldown | bAutomatic |
|---|---|---|---|---|---|
| 近战（`Sword_Slash`，1.29s） | 20（5 刀砍死） | 180 | 100 | 1.35 | false |
| 步枪（`Gun_Shoot`，0.75s） | 10（10 发打死） | 5000 | 0 | 0.15 | true |

**`AttackCooldown` 必须 ≥ 蒙太奇时长**，否则下一次攻击会打断仍在播放的挥砍：
- `Sword_Slash` 实测 1.2917 秒 → 冷却取 1.35
- `Gun_Shoot` 实测 0.75 秒 → 连发时 0.15 的间隔会打断，这是有意的（自动武器按射速覆盖动画），但需确认手感

`AttackAngle` 取值参考：徒手/匕首 60–90，单手剑 90–120，双手重武器 120–180。

以上防弹背心（`ArmorBonus` 25）为例，丧尸打玩家时伤害 × 0.75。

## 8. M2 范围外

以下明确不做，不预留字段：

- 弹药消耗、换弹、弹匣容量
- 霰弹枪扩散（届时可复用 `AttackAngle`）
- 武器静态网格与手持表现
- 命中反馈（顿帧、音效、粒子）

## 9. 待办分工

| 项 | 内容 | 归属 |
|---|---|---|
| 1 | `FDZ_WeaponParams` 结构体、内嵌进 `FDZ_ItemData` | 代码 |
| 2 | `AttributeSet` 新增 `PostGameplayEffectExecute` | 代码 |
| 3 | `IA_Attack` 输入、`UDZ_InputConfig` 加字段、`IMC_Default` 绑定 | 待确认 |
| 4 | 近战 GA、枪械 GA | 代码 |
| 5 | 伤害 GameplayEffect 资产 | 待确认 |
| 6 | `Sword_Slash`、`Gun_Shoot` 包成 AnimMontage 并加通知 | 资产 |
| 7 | `DT_ItemData` 增加两行武器数据、6 行旧数据行名规范化 | 资产 |

"代码"指由项目作者本人编写；"资产"指可由 AI 侧产出。
