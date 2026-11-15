# 局外系统（M8）交接总览

| 项 | 值 |
|---|---|
| 阶段 | M8 局外库存 + 装备带入 |
| 状态 | ◐ 进行中（界面 JSON 已产出，C++ / 存档 / 流程全部未做） |
| 涉及界面 | `WBP_StashPanel`（新建，单文件扁平结构） |
| 复用控件 | `WBP_ItemSlot`、`WBP_EquipmentSlot` |
| 相关文档 | [WBP_StashPanel.md](WBP_StashPanel.md)、[WBP_ItemSlot.md](WBP_ItemSlot.md)、[WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| 前置依赖 | `EItemContainer` 扩展、`UDZ_SaveGame`、大厅关卡 |

---

## 1. 目标与不做

**目标**

- 局外有一份跨会话保留的仓库，关掉游戏再打开还在。
- 出发前把仓库里的装备和随身物品整理好，点「出发」进入 Raid。
- 撤离成功 → 带出的东西进仓库；死亡 → 带进去的那批没了，仓库里没带的还在。

**不做**

- 商人、交易行、制造、任务
- 仓库扩容 / 多存档槽 / 多角色
- 局外组队与匹配服务
- 局内角色身上的装备外观

---

## 2. 形态：局外是纯界面，不是一张联网地图

```
启动 → Lobby.umap（本地运行，不连任何服务器）
        └ UI：WBP_StashPanel 盖在 3D 场景上
        └ 点「出发」→ ClientTravel 到 Raid 专服
Raid 专服 → 现有的 Startup 关卡玩法
        └ 撤离 / 死亡 → 结算面板 → 确认 → 断开专服、回本地大厅
```

**为什么不做「可行走的大厅」**：只要局外连着一台服务器，就是共享世界，单人无法先回大厅（UE 的客户端世界必须与所在服务器一致）。局外放在本地，这个问题从根上消失，同时省掉一套大厅服。

代价：仓库存在客户端本地，服务器不信任它。见第 6 节。

---

## 3. 资产清单

| 资产 | 路径 | 状态 |
|---|---|---|
| `WBP_StashPanel` | `/Game/UI/Stash/` | 资产已生成（父类还是 `UUserWidget`，待 Reparent 到 `UDZ_StashPanel`） |
| `WBP_ItemSlot` | `/Game/UI/Inventory/` | 复用，无需改动 |
| `WBP_EquipmentSlot` | `/Game/UI/Inventory/` | 复用，无需改动 |
| `Lobby.umap` | `/Game/Maps/` | 你负责摆，我未参与 |

C++ 侧已落地：

| 文件 | 内容 |
|---|---|
| `UI/Widgets/Stash/DZ_StashPanel.h/.cpp` | 面板类：收集 36 个格子、4 个装备槽，转发拖拽与出发事件 |
| `UI/Controllers/DZ_StashController.h/.cpp` | 控制器：创建面板、`Open/Close/Toggle/Refresh`、处理拖拽、转发出发请求 |
| `Game/DZ_GameInstance.h/.cpp` | 仓库/背包/装备三份数据、移动规则、`OnStashChanged` |
| `Data/Item/FDZ_ItemSlotHandle.h` | `EItemContainer` 追加 `Stash` / `LoadoutBag` / `LoadoutEquipment` |
| `Data/UI/DZ_UIConfig.h` | 新增 `StashWidgetClass` |
| `UI/UIManager.h/.cpp` | 注册 `StashController` |

`WBP_StashPanel` 是**单文件扁平结构**（没有拆子 WBP），因为局外界面是一次性的，拆开反而多三个 C++ 类。整个界面共 84 个节点，其中 30 个仓库格 + 6 个背包格是 `WBP_ItemSlot` 实例。

---

## 4. 数据契约

### 4.1 仓库（Stash）

| 项 | 定义 |
|---|---|
| 结构 | 扁平的 `TArray<FDZ_ItemStack>`，不分背包 / 装备 |
| 容量 | 30 格 |
| 重量 | 无限制（局外不做负重） |
| 排序 | 不保留格位语义，按槽位下标存；合并时自动堆叠到已有格 |

### 4.2 出发配置（Loadout）

| 项 | 定义 |
|---|---|
| 装备 | 4 个槽：`EEquipSlot` 的 `Armor` / `Primary` / `Melee` / `Backpack` |
| 随身背包 | 6 格，独立的 `TArray<FDZ_ItemStack>` |
| 归属 | 逻辑上属于仓库，是「已经挑出来准备带走」的部分 |
| 出发时 | 从仓库**扣除**，写进「本次行动」字段 |

> 为什么是 6 格：右栏宽 420，2 列 × 3 行能排满。最初谈的是 5 格，要回 5 格就删掉 `BagSlot_6` 并把容量常量改成 5，其余不用动。

### 4.3 存档

| 项 | 定义 |
|---|---|
| 类型 | `USaveGame` 子类，单槽 |
| 槽名 | 固定字符串，例如 `DecayZoneStash`，`UserIndex = 0` |
| 存什么 | 仓库物品列表、出发配置、版本号 |
| 不存什么 | 数据表指针、Actor 引用、UI 状态、当前是否在局内 |
| 读档 | `UGameInstance::Init` 时读一次；读不到就建空仓库 |
| 存盘 | 标脏 + 三个时点落盘：出发前、结算后、大厅 `EndPlay` |

**读档必须做版本与合法性校验**

- 版本号不认识 → 丢弃整份存档，建空仓库（不要尝试迁移）。
- 物品 Id 在 `DT_ItemData` 里查不到 → 丢掉这一格并打 Warning。改表名 / 删行以后老存档不能把游戏搞崩。
- `Equipments` 长度与 `EEquipSlot::None` 不一致 → 补齐或截断到合法长度。

---

## 5. 程序要实现什么

### 5.1 枚举扩展（必须先做）

`EItemContainer` 现在只有 `Backpack` / `Equipment` / `Loot`，要加三个：

| 新值 | 语义 |
|---|---|
| `Stash` | 仓库格子 |
| `LoadoutBag` | 出发用的随身背包格子 |
| `LoadoutEquipment` | 局外的四个装备槽 |

**为什么装备槽不复用 `Equipment`**：局内角色也有一份 `Equipments`，两者是不同数组。共用同一个枚举值，就得在每个操作里按「当前在大厅还是局内」分流，跨阶段刷新时极容易读错数组。

### 5.2 存档与仓库数据

| 位置 | 内容 | 状态 |
|---|---|---|
| `UDZ_GameInstance` | 仓库 / 随身背包 / 装备三份数组，容量 30 / 6 / 4 | ✅ 已写 |
| `UDZ_GameInstance::MoveStashItem` | 六类容器组合的移动与堆叠规则 | ✅ 已写 |
| `UDZ_GameInstance::OnStashChanged` | 每次改动后广播，供存档层标脏 | ✅ 已写 |
| `USaveGame` 子类 | 仓库列表、出发配置、版本号 | ⬜ 未写 |
| 读档 / 存盘 | `Init` 里读、`OnStashChanged` 标脏后在三个时点落盘 | ⬜ 未写 |
| 大厅 Controller | `Open/Close/Toggle/Refresh` 已就绪 | ✅ 已写 |

### 5.3 移动规则

| 来源 → 目标 | 规则 |
|---|---|
| 仓库 → 随身背包 | 堆叠到同物品格，或占用空格 |
| 仓库 → 装备槽 | 物品 `EquipSlot` 必须匹配目标槽 |
| 随身背包 → 装备槽 | 同上 |
| 装备槽 → 仓库 / 背包 | 允许 |
| 装备槽 ↔ 装备槽 | **不允许**（先简化，避免交换分支） |
| 仓库 ↔ 仓库 | 允许交换格位 |

堆叠上限沿用 `ItemHelper::GetMaxStack`：装备 1，其余不限制。判定失败时两边都不动。

### 5.4 出发流程

1. 校验：装备槽里没有非法物品；背包格子里的物品 Id 都合法。
2. 从仓库扣除装备与随身背包中的物品。
3. 存盘。
4. 把出发配置写进「本次行动」暂存字段（随 `ClientTravel` 带过去，或连上后由 `Server RPC` 上传）。
5. 断开大厅 / `ClientTravel` 到 Raid 服。

### 5.5 局内出生

- 服务器拿到该玩家的出发配置后，写进角色的 `UDZ_InventoryComponent`。
- 必须触发 `OnEquipmentsChanged`，否则护甲不会重算。
- 写入时机放在 `PossessedBy` 之后、ASC 初始化完成之后；`BeginPlay` 里更早写入会漏掉护甲重算。

### 5.6 结算与回大厅

`UDZ_SettlementController` 目前没有监听 `UDZ_SettlementPanel::OnConfirmRequested`（这个委托是空的）。这里接上：

| 结果 | 动作 |
|---|---|
| 撤离 | 把结算快照里的 `Items` 合并回本地仓库 → 存盘 → 断开 → 回大厅 |
| 死亡 | 不回写（出发时已扣除）→ 存盘 → 断开 → 回大厅 |

结算快照 `FDZ_SettlementInfo::Items` 已经是服务端拼好的「这一局带出的全部物品」，直接合并即可，不需要额外 RPC。

### 5.7 界面类

建议一个类：`UDZ_StashPanel`，BindWidget 全集见 [WBP_StashPanel.md](WBP_StashPanel.md)。

**不要多声明 BindWidget** —— 插件导入时会校验父类的 BindWidget 是否都能在树里找到，多声明一个就让整次导入失败。

在 `NativeOnInitialized` 里必须做完这两件事，否则拖拽拿不到正确的槽位：

| 对象 | 要做的事 |
|---|---|
| `Grid_Stash` 下的 30 个 `ItemSlot_*` | 逐个 `SetSlotInfo(EItemContainer::Stash, 序号)`，并订阅 `OnWidgetDragEnded` / `OnWidgetLeftClicked` |
| `Grid_LoadoutBag` 下的 6 个 `BagSlot_*` | 同上，容器用 `EItemContainer::LoadoutBag` |
| 4 个 `EquipSlot_*` | `SetSlotInfo(EItemContainer::LoadoutEquipment, (int32)EEquipSlot)`，订阅 `OnWidgetDragEnded` |

收集格子照 `UDZ_BackpackPanel` 的写法按名字前缀扫描（`ItemSlot_` / `BagSlot_`），但**要加越界保护**：数量以代码里的容量常量为准，不要直接按传入数组长度索引。

---

## 6. 已知限制（必须写进 README / 面试说明）

| 限制 | 说明 |
|---|---|
| 仓库在客户端 | 局外是本地运行，存档在本机。客户端可以改档，服务器无法验证。正式项目应改为后端存档，由 Raid 服按玩家 ID 拉取 |
| 断线即损失 | 局内掉线 = 与死亡同价：出发配置已在本地扣除，服务器不会返还 |
| 无组队 / 匹配 | 局外没有服务器世界，组队需要另做大厅服务 |
| 面板高度固定 | 两块卡片 940 高，屏幕高度低于 1000 会裁切（1080p 起正常） |
| 撤离回写包含出发装备 | 「带进去又带出来」不亏不赚，符合直觉；不要额外返还 |

---

## 7. 当前未完成

| 项 | 状态 |
|---|---|
| `WBP_StashPanel.json` | ✅ 已产出 |
| 编辑器同步生成资产 | ✅ 已生成 |
| 父类 `UDZ_StashPanel` | ✅ 已写，⬜ 资产待 Reparent |
| `EItemContainer` 三个新值 | ✅ 已加 |
| 面板 / 控制器 / 数据层 | ✅ 已写 |
| 存档 / 读档 | ⬜ 未写 |
| 出发 / 回大厅流程 | ✅ C++ 已接（见第 10 节），⬜ 蓝图与关卡待配 |
| 结算确认接点 | ✅ 已接 |
| `UIConfig` 填 `StashWidgetClass` | ⬜ 待你在编辑器里指定 |
| `Lobby.umap` 与场景 | ⬜ 待建 |
| HUD 在大厅的隐藏 | ✅ 用 UIManager 拆类解决，不需要运行时判断 |

---

## 10. 出发 / 回大厅链路（C++ 已落地）

```
Lobby.umap（BP_LobbyGameMode）
  └ PC = BP_LobbyPlayerController（父类 ADZ_LobbyPlayerController）
       └ UDZ_LobbyUIManager：只装配 StashController，并打开仓库面板
       └ 点出发 → PC::StartRaid()
            → UDZ_GameInstance::TakeDepartureLoadout()  从仓库扣到 PendingLoadout
            → OpenLevel(RaidMapName)
Startup（BP_GameMode）
  └ PC = BP_DZ_PlayerController（父类 ADZ_RaidPlayerController）
       └ UDZ_RaidUIManager：HUD / 交互 / 背包 / 搜刮 / 结算
  └ 角色 OnAbilitySystemInitialized → ApplyPendingLoadout() 写入装备与背包
  └ 撤离 / 死亡 → 结算面板 → 确认
       → 撤离：UDZ_GameInstance::DepositRaidResult(物品, 装备) 合并回仓库
       → 死亡：不回写
       → OpenLevel(LobbyMapName)
```

### 关键约束

| 约束 | 原因 |
|---|---|
| `BP_LobbyGameMode` 的 `PlayerControllerClass` 必须填 `BP_LobbyPlayerController` | 它是局外 PC，负责创建 `UDZ_LobbyUIManager` |
| `PlayerStateClass` 必须是 `ADZ_PlayerState` | `ADZ_PlayerController::OnRep_PlayerState` 里没有判空 |
| 大厅关卡里放一台打了 `LobbyCamera` 标签的 `CameraActor` | GameMode 在 `PostLogin` 里按 tag 找它设视角 |
| `BP_DZ_GameInstance` 里填 `RaidMapName` / `LobbyMapName` | 填 `Startup` / `Lobby`，别在 C++ 硬编码 |
| `ADZ_LobbyGameMode::DefaultPawnClass = nullptr` | 大厅不生成角色，避免 Possession 把 GAS 全套跑起来 |

### 局外 / 局内 UI 分流

没有任何"是不是局外"的运行时判断，分流靠两层类：

| 层 | 局外 | 局内 |
|---|---|---|
| GameMode | `ADZ_LobbyGameMode` | `BP_GameMode` |
| PlayerController | `ADZ_LobbyPlayerController` | `ADZ_RaidPlayerController` |
| UI 管理器 | `UDZ_LobbyUIManager`（只持 `StashController`） | `UDZ_RaidUIManager`（持 6 个局内 controller） |

三个 PC 的关系：

```
ADZ_PlayerController（基类，只管 UI 生命周期）
  ├ UUIManager* UIManager
  ├ BeginPlay / EndPlay / Tick / GetUIManager
  └ virtual UUIManager* CreateUIManager()
       ├ ADZ_RaidPlayerController   → UDZ_RaidUIManager
       └ ADZ_LobbyPlayerController  → UDZ_LobbyUIManager，BeginPlay 里打开仓库
```

`UUIManager` 是纯容器：只提供 `Controllers` 数组、`CreateController<T>()`、开关控件与输入模式，不声明任何具体 controller 成员。控件通过 `ADZ_PlayerController::GetUIManager()` 拿到管理器。

> 局外 PC 是独立的 C++ 类，不继承局内 PC，所以它没有 Enhanced Input 绑定，也不需要 `InputConfig` —— 大厅界面的鼠标交互由 UMG 自己处理。

> **蓝图侧**：`BP_DZ_PlayerController` 的父类必须指向 `ADZ_RaidPlayerController`（不是基类），否则局内 PC 会变成空基类，表现为"进游戏什么都不响应"。



---

## 8. 验收

界面层（不依赖 C++，编辑器同步后就能看）：

| 场景 | 期望 |
|---|---|
| 打开资产 | 左 30 格 / 右 4 装备槽 + 6 背包格，居中，中间留空 |
| 1920×1080 | 两张卡片完整显示，四周与中间透出场景 |
| 缩小窗口 | 卡片保持定宽定高、垂直居中，不随窗口拉伸变形 |

数据层（C++ 接上后）：

| 场景 | 期望 |
|---|---|
| 塞 3 件物品到仓库 → 重启编辑器 | 3 件还在 |
| 把护甲拖到护甲槽 | 仓库少一件、槽里显示图标与名称 |
| 把药水拖到护甲槽 | 拒绝，两边都不动 |
| 携带装备出发 | 进局后身上就是那批东西，护甲数值生效 |
| 撤离成功 | 回大厅后仓库多了本局带出的物品 |
| 死亡 | 回大厅后仓库里没带的还在，带进去的没了 |
| 存档里塞一个不存在的 ItemId | 读档时丢弃该格、打 Warning，游戏不崩 |

---

## 9. 写 C++ 前请先看的三个既有坑

1. **`UDZ_BackpackPanel::SetBackpackPanel` 没有越界保护** —— 按传入数组长度循环直接索引 `ItemSlots[i]`。30 格喂给 5 槽的面板会崩。新面板不要照抄这个写法。
2. **`UDZ_InventoryComponent::ClearAll()` 直接 `Items.Empty()` + `Equipments.Empty()`** —— 清空后所有 `IsSlotValid` 全返回 false，组件等于半废。要重置请逐格置空。
3. **服务器改 Replicated 数组不会触发自己身上的 `OnRep`** —— Listen Server 下服务器侧也要手动刷一次 UI。
