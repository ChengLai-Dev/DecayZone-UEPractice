# 撤离结算 UI 交接总览

这份文档记录**控件组成、数据契约、程序需实现的接口、当前未完成项**。每个 WBP 的节点清单/事件/状态以各自的交互文档为准。

---

## 1. 当前资产

| 资产 | 作用 | 父类 |
|---|---|---|
| `/Game/UI/Settlement/WBP_SettlementPanel` | 撤离结算主面板 | `UUserWidget`（待接 C++，建议 `UDZ_SettlementPanel`） |
| `/Game/UI/Settlement/WBP_SettlementStatCard` | 数值卡片（总价值 / 击杀丧尸 / 击杀玩家） | `UUserWidget`（建议 `UDZ_SettlementStatCard`） |
| `/Game/UI/Settlement/WBP_SettlementItemSlot` | 战利品格（只读，图标 + 数量） | `UUserWidget`（建议 `UDZ_SettlementItemSlot`） |

源 JSON 在 `UIWidgets/UI/Settlement/`，由 WidgetJsonBridge 重建。
**这三个资产尚未在编辑器里生成**，需要打开编辑器触发一次 JSON → WBP 重建。

---

## 2. 控件组成

```
WBP_SettlementPanel
├─ VB_Header
│  ├─ Text_Title          撤离成功 / 撤离失败
│  └─ Text_Subtitle       本次行动用时 mm:ss
├─ (分隔线)
├─ HB_Stats
│  ├─ StatCard_Value      → WBP_SettlementStatCard
│  ├─ StatCard_EnemyKills → WBP_SettlementStatCard
│  └─ StatCard_PlayerKills→ WBP_SettlementStatCard
├─ (分隔线)
├─ VB_Loot
│  ├─ HB_LootHeader
│  │  ├─ Text_SectionTitle   「带出物品」
│  │  └─ Text_LootCount      共 N 件
│  ├─ ScrollBox_Items
│  │  └─ Grid_Items          UniformGridPanel，每行 5 列
│  │     └─ WBP_SettlementItemSlot ×N（运行时生成）
│  └─ Text_EmptyHint         无物品提示（默认 Collapsed）
└─ HB_Footer
   ├─ Text_Hint
   └─ Btn_Confirm
```

统计区三张卡等宽 Fill，卡间距 16px。战利品区占满剩余高度，超过 10 格自动竖向滚动。

---

## 3. 数据契约

结算数据目前**只存在于设计层**，代码侧尚未定义。建议结构（名字你定，字段别少）：

| 字段 | 类型 | 来源 / 谁算 |
|---|---|---|
| `bSucceeded` | `bool` | GA 的 `bSucceeded` |
| `ElapsedSeconds` | `float` | 进局时间 → 撤离时间的差值，服务端算 |
| `TotalValue` | `int32` | 背包 + 装备槽物品价值求和，服务端算 |
| `EnemyKills` | `int32` | PlayerState 计数 |
| `PlayerKills` | `int32` | PlayerState 计数 |
| `Items` | `TArray<FDZ_ItemStack>` | 服务端背包快照 |

`FDZ_ItemStack` 已存在（`ItemId` + `Count`），直接复用。

### 数据流

```
[服务端]
撤离 GA 判定完成 (bSucceeded)
  → 组装结算数据（读背包 / 读 PlayerState 计数 / 算价值）
  → ClientOpenSettlementPanel(Data)        ← 签名要从无参改成带结构体
[客户端]
收到 RPC → 创建/打开 WBP_SettlementPanel → 逐字段填充
点击「确认」→ 关闭面板
```

> **必须在服务端组装**。客户端不能自己算价值或击杀数，遵循项目底线「UI 只读本地复制数据，不自行造数据、不自行判定结果」。

---

## 4. 程序需实现的接口清单

### 3.1 UI 类

| 类 | 接口 | 说明 |
|---|---|---|
| `UDZ_SettlementPanel` | `SetSettlementInfo(const FDZ_SettlementInfo&)` | 一次性填充标题、用时、三张卡、物品网格、空态 |
| `UDZ_SettlementStatCard` | `SetStat(FText Value, FText Label)` | 同时设值 + 标签 |
| `UDZ_SettlementItemSlot` | `SetItem(UTexture2D* Icon, int32 Count)` | 有图标才 `Visible`；`Count <= 1` 隐藏角标 |
| `UDZ_SettlementPanel` | `OnConfirmClicked` 委托 | 「确认」按钮点击后向外抛，由 Controller 决定后续 |

### 3.2 数据与统计

| 位置 | 要做什么 |
|---|---|
| `FDZ_ItemData` | **新增 `Value`（int32）**，并在 `DT_ItemData` 填值 |
| `ADZ_PlayerState` | 新增 `EnemyKills` / `PlayerKills`（Replicated int32），服务端在致死伤害里累加 |
| 击杀归属 | `AttributeSet::PostGameplayEffectExecute` 或 `Death()` 里取原凶，按目标类型分流 |
| 局内用时 | PlayerState 或 GameState 记开始时间，结算时相减 |
| `ADZ_PlayerController` | `ClientOpenSettlementPanel` 改签名，带结算数据 |
| `UDZ_UIConfig` | 新增 `SettlementPanelClass`（`TSubclassOf<UDZ_SettlementPanel>`），并填 `DA_UIConfig` |
| `UDZ_UIConfig` | 新增 `SettlementItemSlotClass`（`TSubclassOf<UDZ_SettlementItemSlot>`），用于动态生成战利品格 |

> 战利品格的类引用必须从 `UIConfig` 拿，不要硬编码 `/Game/UI/Settlement/WBP_SettlementItemSlot`，否则改资产路径就得改代码。

### 3.3 打开 / 关闭

| 动作 | 建议做法 |
|---|---|
| 打开 | 参照 `LootPanel` 的 `OpenWidget` / 输入模式切换；结算界面应独占鼠标，不允许角色移动 |
| 关闭 | 只关面板；是否传送回大厅 / 退出由程序另接，界面层不处理 |
| 重复打开 | 已打开时不重复创建，只更新数据 |

---

## 5. 当前未完成项

| 项 | 状态 | 归属 |
|---|---|---|
| 三个 WBP 资产生成 | ☐ 需在编辑器触发 JSON 重建 | 你 |
| `FDZ_ItemData::Value` + 数据表填值 | ☐ 未开始 | 你 |
| 击杀统计（PlayerState + 致死归属） | ☐ 未开始 | 你 |
| 局内用时记录 | ☐ 未开始 | 你 |
| `ClientOpenSettlementPanel` 改签名并组装数据 | ☐ 未开始 | 你 |
| `UDZ_UIConfig` 加 `SettlementPanelClass` | ☐ 未开始 | 你 |
| 撤离成功后角色去哪（留在原地 / 传送 / 退出） | ☐ 未定 | 待你决定 |
| 死亡是否也弹结算面板 | ☐ 未定 | 待你决定 |
| 撤离区一次性（防重复撤离） | ☐ 未开始 | 你 |

---

## 6. 已知限制

- **物品格没有悬停详情**：本期不做，格子不继承 `UDZ_ItemSlotBase`，避免误触发拖拽。
- **总价值口径未定**：目前 UI 只负责显示一个数，是否把装备槽里穿着的护甲/武器算进去由程序决定。
- **失败态的视觉降级**：UI 已留出空间（`Text_SectionTitle` 改文案 + 格子降透明度），但没有做专门的"丢失"样式，需要的话再补一层红色描边。
- **10 格是 Designer 占位**：运行时必须 `ClearChildren` 重建，否则会出现占位格与真实数据混在一起。
