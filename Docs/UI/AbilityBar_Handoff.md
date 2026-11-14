# 技能栏（M5）交接总览

M5「护盾 + 加速」的 HUD 部分。本文记录资产清单、数据分层、程序接口与当前未完成项；每个 WBP 的节点细节见：

- `Docs/UI/WBP_AbilityBar.md`
- `Docs/UI/WBP_AbilitySlot.md`

> **前置事实**：M5 的玩法侧仍是零。`Source/` 下没有 `GA_Shield` / `GA_Sprint`，没有 Cost/Cooldown GE，`DA_StartupAbilities` 里也没有对应能力，`Content/Data/PlayerInput/` 下没有技能 IA。
>
> 本次交付的是**界面 + 数据通路 + 刷新代码**：图标与键位会正常显示；冷却、能量不足变暗、生效描边在 GA 落地前不会有数据，这是预期状态。

---

## 1. 资产清单

| 资产 | 路径 | 类型 | 说明 |
|---|---|---|---|
| `WBP_AbilityBar` | `/Game/UI/HUD/` | Widget Blueprint | 底部居中技能栏，父类 `UDZ_AbilityBar` |
| `WBP_AbilitySlot` | `/Game/UI/HUD/` | Widget Blueprint | 单槽，父类 `UDZ_AbilitySlot` |
| `T_UI_Icon_Skill_Shield` | `/Game/Assets/SkillIcons/Runtime/` | Texture2D | 512²，透明底，TC_EditorIcon / UI 组 / 无 mip |
| `T_UI_Icon_Skill_Sprint` | `/Game/Assets/SkillIcons/Runtime/` | Texture2D | 同上 |
| `M_CooldownRing` | `/Game/UI/Materials/` | Material | UI 域、半透明、标量参数 `Percent` |
| `MI_CooldownRing` | `/Game/UI/Materials/` | MaterialInstanceConstant | 父材质同上，`Percent=0` |
| `DT_SkillData` | `/Game/Data/Ability/` | DataTable | 技能定义，行结构 `FDZ_SkillData` |

JSON 源：`UIWidgets/UI/HUD/WBP_AbilityBar.json`、`UIWidgets/UI/HUD/WBP_AbilitySlot.json`。
图标源图与导入脚本：`Content/Assets/_Source/UI/Icons/`（`SkillIconsImport.py`）。

### 为什么没有复用 `M_LootRing`

项目里已有 `M_LootRing`（参数 `Radius` / `Thickness` / `Progress` / `Color`），它画的是**细环描边**。冷却遮罩需要的是**填充扇形**，两者几何不同，改参数改不出来。所以新建了 `M_CooldownRing`。

---

## 2. 数据分层

| 层 | 承载 | 位置 | 可变性 |
|---|---|---|---|
| 技能定义 | `FDZ_SkillData`：SkillId、AbilityTag、显示名、图标 | `DT_SkillData` 的 `SkillId` 列 = 技能 ID | 静态数据 |
| 槽位默认配置 | `FDZ_AbilitySlotLayout`：键位文案、默认技能 ID | `DA_UIConfig.AbilitySlots` | 静态数据 |
| 运行时映射 | 槽位索引 → 技能 ID | `ADZ_PlayerController::SlotSkillIds` | 运行时可变，本期不持久化 |

**技能 ID 是 `FDZ_SkillData` 里的 `int32 SkillId`，不是行名，也不是 AbilityTag。** 行名（`Shield` / `Sprint`）只做可读标签，重命名不影响映射；Tag 以后改了也不影响身份。键位属于槽位，技能换槽后键位不变。

---

## 3. 控件结构

```
WBP_AbilityBar                              /Game/UI/HUD/
└─ RootCanvas (SelfHitTestInvisible)
   └─ Size_AbilityBar (140×64, 底部居中, 距底 36px)
      └─ HB_Slots
         ├─ AbilitySlot_1  → WBP_AbilitySlot
         └─ AbilitySlot_2  → WBP_AbilitySlot

WBP_AbilitySlot                             /Game/UI/HUD/
└─ Root_Size (64×64)
   └─ Border_Frame (2px)
      └─ Border_Active (青绿, 默认 Collapsed)
         └─ Fill_Slot (3px)
            └─ Overlay_Slot
               ├─ Img_Icon
               ├─ Img_CooldownMask (Brush = MI_CooldownRing)
               ├─ Border_Disabled
               ├─ Text_Cooldown
               └─ Text_Key
```

---

## 4. 数据流

```
DA_UIConfig.AbilitySlots
  → 开局复制为 UDZ_AbilityBarController 的 SlotSkillIds（槽位 → SkillId）

UDZ_AbilityBarController（UIManager 持有，0.1s 节流）
  → SlotSkillIds[i] → DT_SkillData 按 SkillId 查行 → AbilityTag
  → ASC->FindAbilitySpecByTag(AbilityTag)
       ├─ 冷却：Spec->Ability->GetCooldownTimeRemainingAndDuration(...)
       ├─ 可用：Spec->Ability->CheckCost(...)
       └─ 生效：Spec->IsActive()
  → UDZ_AbilityBar::SetEmpty / SetKeyText / SetIcon / SetCooldown / SetAvailable / SetActive
  → UDZ_AbilitySlot 对应 setter

SetSlotSkill(槽位, SkillId)
  → 改写 SlotSkillIds（目标技能已在别的槽则自动交换）
  → 重推两个槽
```

> UE 5.8 的 ASC 已没有 `GetCooldownRemainingForTag`。冷却必须从技能自己的 AbilitySpec 取，旧文档里的写法已作废。

冷却比例口径：`Percent = 剩余 / 总时长`，`0` 表示就绪。材质里 `Percent=1` 是整圆遮满。

**UI 不自己计时，也不自己判定能不能放。** 时间与可用性都来自 ASC 与复制过的属性值。

---

## 5. 程序接口

### 5.1 类

| 类 | 基类 | 职责 |
|---|---|---|
| `UDZ_AbilitySlot` | `UDZ_UserWidget` | 单槽显示，只认原语 setter |
| `UDZ_AbilityBar` | `UDZ_UserWidget` | 两槽分发，`MaxSlots = 2` |
| `FDZ_SkillData` | `FTableRowBase` | 技能定义行（含 `SkillId`） |
| `FDZ_AbilitySlotLayout` | `USTRUCT` | 槽位默认配置 |
| `UDZ_UIControllerBase` | `UObject` | 功能控制器基类：解析并缓存 `GameInstance` / `UIConfig`，供子类直接使用 |
| `UDZ_AbilityBarController` | `UDZ_UIControllerBase` | 槽位映射、0.1s 刷新、换槽入口 |
| `UDZ_StatusBarsController` | `UDZ_UIControllerBase` | 生命 / 能量状态条的创建与刷新 |
| `UDZ_InteractController` | `UDZ_UIControllerBase` | 交互进度状态、Prompt 与读条、攻击与移动锁查询 |
| `UDZ_InventoryController` | `UDZ_UIControllerBase` | 背包面板：开关、刷新、面板内拖拽事件 |
| `UDZ_LootController` | `UDZ_UIControllerBase` | 搜刮面板：打开、刷新、拖拽与拿取事件 |
| `UDZ_SettlementController` | `UDZ_UIControllerBase` | 结算面板：接收快照并显示 |
| `UUIManager` | `ULocalPlayerSubsystem` | 协调者：持有控制器、ZOrder、输入模式、面板栈 |

源码目录：

```
Public/UI/
├─ DZ_UserWidget.h
├─ UIManager.h
├─ FDZ_InteractTracker.h
├─ Controllers/
│  ├─ DZ_UIControllerBase.h
│  ├─ DZ_AbilityBarController.h
│  ├─ DZ_StatusBarsController.h
│  └─ DZ_InteractController.h
└─ Widgets/
   ├─ HUD/
   ├─ Interaction/
   ├─ Item/
   ├─ Inventory/
   ├─ Loot/
   └─ Settlement/
```

**控制器基类约定**：`Initialize` 内部解析并缓存 `GameInstance` / `UIConfig`，子类第一行调 `Super::Initialize` 后直接用这两个成员，不要再自己 `Cast<UDZ_GameInstance>`。Widget 的创建与释放仍由子类直接调 `CreateWidget` / `RemoveFromParent`——引擎已在 `CreateWidget` 内部处理空 Owner 与空 Class（空 Class 会打 `LogUMG` 错误并返回 null），不再包一层。子类 `Initialize` 的固定形态是：判 `UIConfig` → `CreateWidget` → 判空 → 自己的数据初始化与 `Open()`。

**控制器访问约定**：控制器实例就是 `UUIManager` 上的公有成员（`AbilityBarController` / `StatusBarsController` / `InteractController` / `InventoryController` / `LootController` / `SettlementController`），没有 getter。PC 侧直接 `UIManager->XxxController->Method()`，不判控制器为空。只有三类路径判 `UIManager` 是否有效：客户端 RPC（如 `ClientInteractFinished`）、`BeginPlay`（`GetSubsystem` 结果）、`EndPlay`（清理）。另有 `IsLocalController()` 守卫须保留在可能被服务端调用到的入口（`BeginInteract` / `CancelInteract` / `Interact`），因为 `ADZ_InteractableActor::NotifyAutoInteractStart` 是 `BlueprintCallable`，蓝图可能在服务端触发。

### 5.2 `UDZ_AbilitySlot`

| 方法 | 行为 |
|---|---|
| `SetKeyText(const FText&)` | 键位角标 |
| `SetIcon(UTexture2D*)` | 图标，空则隐藏；相同图标不重建 Brush |
| `SetCooldown(float, float)` | 扇形遮罩 + 整秒倒计时（`>0.5s` 才显示） |
| `SetAvailable(bool)` | 变暗层 |
| `SetActive(bool)` | 激活描边 |
| `SetEmpty(bool)` | 空槽状态；为空时以上四个状态 setter 不生效 |

### 5.3 `UDZ_AbilityBar`

| 方法 | 行为 |
|---|---|
| `SetKeyText` / `SetIcon` / `SetCooldown` / `SetAvailable` / `SetActive` / `SetEmpty` | 与槽同名，多一个 `int32 SlotIndex`；越界忽略 |

### 5.4 控制器与 PlayerController

| 类 / 方法 | 说明 |
|---|---|
| `UDZ_AbilityBarController` | 持有槽位映射、创建技能栏、0.1s 刷新、`SetSlotSkill` / `GetSlotSkill` / `Hide` |
| `UDZ_StatusBarsController` | 创建并常驻状态条，`Refresh()` 从 PlayerState 的 ASC 读数推送 |
| `UDZ_InteractController` | 持有 `FDZ_InteractTracker`、Prompt 与读条 Widget；`IsBlockingAttack()` 供攻击与移动锁判定 |
| `UUIManager::InitializeControllers` | 由 `ADZ_PlayerController::BeginPlay` 调用，依次创建三个控制器 |
| `UUIManager::TickControllers` | 由 `ADZ_PlayerController::Tick` 调用 |
| `ADZ_PlayerController::SetSlotSkill(int32, int32)` | 转发给控制器，`BlueprintCallable` |
| `ADZ_PlayerController::GetSlotSkill(int32) const` | 转发给控制器，`BlueprintPure` |

### 5.5 配置

1. `UDZ_UIConfig`：`AbilityBarClass`、`AbilitySlots`。
2. `UDZ_GameInstance`：`SkillDataTable`（与 `ItemDataTable` 同一模式，Init 时缓存成 `int32 → FDZ_SkillData`）。
3. `DA_UIConfig` 挂 `WBP_AbilityBar_C`、两条槽位默认配置，`ZOrderMap` 加 `0`。
4. `BP_DZ_GameInstance` 的 Class Defaults 里把 `SkillDataTable` 指向 `DT_SkillData`。
5. `UUIManager` 在 `ADZ_PlayerController::BeginPlay` 里初始化控制器，技能栏由 `UDZ_AbilityBarController` 创建并常驻；结算面板打开时控制器隐藏技能栏。
6. 冷却材质写在 `WBP_AbilitySlot.json` 的 `Img_CooldownMask` Brush 里，C++ 不持有材质引用。

---

## 6. 当前完成状态

| 项 | 状态 |
|---|---|
| 控件树 / JSON / 图标 / 冷却材质 | ✅ 已有 |
| `UDZ_AbilitySlot` / `UDZ_AbilityBar` | ✅ 已实现并编译通过 |
| `FDZ_SkillData` / `FDZ_AbilitySlotLayout` | ✅ 已实现 |
| `UDZ_UIControllerBase` / `UDZ_AbilityBarController` | ✅ 已实现并编译通过 |
| `UDZ_UIConfig` 两字段 | ✅ 已加 |
| `UDZ_GameInstance::SkillDataTable` + 缓存查询 | ✅ 已加 |
| Controller 创建/刷新/换槽入口/结算隐藏 | ✅ 已加 |
| `Ability.Player.Skill.Shield` / `.Sprint` 原生 Tag | ✅ 已声明 |
| `DT_SkillData` 两行数据（含 SkillId） | ✅ 已建 |
| WBP 父类重挂 / DA 配置 / ZOrder | ✅ 已配 |
| `GA_Shield` / `GA_Sprint`、Cost/Cooldown GE | ❌ 玩法侧 |
| `IA_Skill1` / `IA_Skill2` + IMC 映射 | ❌ 玩法侧 |
| 换槽界面 | ❌ 后续 |
| 映射持久化（M8） | ❌ 后续 |

---

## 7. 已知限制

1. **JSON 里图标不带贴图**。`Img_Icon` 在 Designer 里是空的，跟现有 `WBP_ItemSlot` 一致，图标由控制器的 `SetIcon` 喂入。空 `Image` 铺满后会画成整格灰色属预期。冷却遮罩的材质写在 `Img_CooldownMask` 的 Brush 里，不需要代码注入。
2. **键位角标两个槽默认都显示 `1`**。它们共用同一个 `WBP_AbilitySlot`，JSON 默认值只有一份，`2` 由 `Info.KeyText` 写入。
3. **技能栏宽度写死 140**。扩到四槽要同时改 `Size_AbilityBar` 的 `WidthOverride` 和 `AbilitySlots` 长度；`UDZ_AbilityBar` 目前只绑两个槽，超过会告警并忽略。
4. **冷却方向**。材质从 12 点方向顺时针遮罩；要逆时针把 `atan2` 的第一个参数取反，不用动 UI。
5. **换槽没有 UI、不持久化**。本期只提供 `SetSlotSkill` / `GetSlotSkill` 接口；配置界面与 M8 存档再接。
6. **已配置技能但 GA 未授予时整体变暗**。`FindAbilitySpecByTag` 找不到技能时按不可用处理（`Border_Disabled` 可见），这是预期反馈。**空槽不走这条路径**，`bEmpty` 会隐藏图标和所有遮罩，不会变暗。
7. **生效描边依赖 GA 实现方式**。只有 Spec 保持激活的持续型技能会让 `Border_Active` 亮；瞬发 + 时长 GE 不会。
8. **编辑器启动时的导入顺序问题**（非本次引入）：`WBP_SettlementPanel` / `WBP_SettlementStatCard` / `WBP_SettlementItemSlot` 曾因父类为空导致编译失败，已重新挂回 `DZ_SettlementPanel` / `DZ_SettlementStatCard` / `DZ_SettlementItemSlot` 并编译通过。若启动后再次出现同类报错，按同样方式重挂父类即可。

---

## 8. 验收步骤

1. 打开 `WBP_AbilityBar` Designer，确认两槽并排、间距 12、整体水平居中。
2. PIE 里确认技能栏出现在底部正中，距底约 36px。
3. 确认两个槽显示真实图标，左上角分别是 `1` / `2`，整体偏暗（无 GA，预期）。
4. 拉窗口宽度，确认始终水平居中。
5. 打开结算面板，确认技能栏隐藏。
6. GA 落地后：按 1 / 2 确认扇形遮罩、倒计时、能量不足变暗。
7. 调用 `SetSlotSkill(0, 2)` / `SetSlotSkill(1, 1)` 确认图标互换，冷却跟着技能走。
8. 把某个槽的 `SkillId` 设成 `-1`，确认该槽变空：图标隐藏、无变暗、无遮罩，键位保留。
