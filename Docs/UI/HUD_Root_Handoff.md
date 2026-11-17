# 常驻 HUD 根（WBP_HUD）交接说明

把分散的常驻 HUD 元素收进一个根控件（方案 A：设计期子控件）。节点级细节见 `Docs/UI/WBP_HUD.md`，三个子控件各自见 `WBP_StatusBars.md` / `WBP_AbilityBar.md` / `WBP_RaidTime.md`。

## 1. 为什么改

原来的形态是「一个 HUD 元素 = 一个 `AddToViewport`」：

- 每个 WBP 自己写一套 `RootCanvas` 锚点，全局布局只存在于文档里，只能 PIE 后才知道有没有撞；
- ZOrder 靠 `DA_UIConfig.ZOrderMap` 手工维护，漏配静默变 0；
- 没有「整屏 HUD 隐藏」的入口；
- 以后加 SafeZone / DPI 缩放没有统一作用域。

改成一个 HUD 根后：布局集中、层级退化成树序、整体显隐一个开关，仍不引入 CommonUI 的迁移成本。

## 2. 资产清单

| 资产 | 变化 |
|---|---|
| `/Game/UI/HUD/WBP_HUD` | **新增**，根 canvas + 三个子控件 |
| `/Game/UI/HUD/WBP_StatusBars` | 压平：根从 `RootCanvas` 改为 `Size_Status`，尺寸 340×64 |
| `/Game/UI/HUD/WBP_AbilityBar` | 压平：根从 `RootCanvas` 改为 `Size_AbilityBar`，尺寸 140×64 |
| `/Game/UI/HUD/WBP_RaidTime` | 压平：根从 `RootCanvas` 改为 `Size_RaidTime`，自适应尺寸；父类 `UDZ_RaidTime` |
| `/Game/UI/HUD/WBP_Tips` | 不动（瞬发、多实例、必须压在最上层） |
| `/Game/UI/Interaction/WBP_InteractPrompt`、`WBP_InteractProgress` | 不动（本轮范围外） |

三个子控件的根都补了 `Visibility = SelfHitTestInvisible`（原来 `WBP_StatusBars` 根是默认 `Visible`，其实会挡鼠标）。

## 3. 生命周期与数据流

```
UUIManager::TickControllers        // 遍历 Controllers，HUD 与它的子 controller 都在这个数组里
  ├─ UDZ_HUDController::Initialize
  │    ├─ CreateWidget<UDZ_HUDLayout>(PC, UIConfig->HUDClass)
  │    ├─ CreateController<UDZ_StatusBarsController>()   → SetStatusBarsWidget(Layout->GetStatusBars())
  │    ├─ CreateController<UDZ_AbilityBarController>()   → SetAbilityBarWidget(Layout->GetAbilityBar())
  │    ├─ CreateController<UDZ_RaidTimeController>()     → SetRaidTimeWidget(Layout->GetRaidTime())
  │    │    └─ setter 内建 VM → SetViewModelByClass → 首推秒数
  │    └─ Layout->Open()                  // AddToViewport 一次（bResident）
  │
  ├─ UDZ_StatusBarsController::Tick（无）、UDZ_AbilityBarController::Tick（0.1s 刷新）、
  │  UDZ_RaidTimeController::Tick（推秒数）—— 由 TickControllers 统一驱动，不需要 HUD 转发
  │
  └─ 对外只通过 facade：RaidUI->HUDController->RefreshStatusBars() / RefreshAbilityBar() / HideAbilityBar()

UUIManager::DestroyControllers     // 同样是扁平遍历
  ├─ 子 controller Shutdown：只清引用
  └─ UDZ_HUDController::Shutdown：RemoveFromParent 整棵 HUD
```

三个子 controller 通过 `UUIManager::CreateController` 创建（无参），随后由 HUD 控制器调用各自的 setter 注入控件：`SetStatusBarsWidget` / `SetAbilityBarWidget` / `SetRaidTimeWidget`。因为走同一个工厂，它们天然登记在 `Controllers` 数组里：Tick 与 Shutdown 都由管理器统一驱动，HUD 控制器**不重写 Tick**，也不手写转发。

子 controller 的 `Shutdown` 只清指针、不 `RemoveFromParent`；HUD 根由 `UDZ_HUDController::Shutdown` 负责移除。**约定：注入的控件由提供方销毁**，controller 只负责驱动。

## 4. 代码改动

**新增**

| 文件 | 说明 |
|---|---|
| `Public/UI/Widgets/HUD/DZ_HUDLayout.h` + `.cpp` | HUD 根；三个 `BindWidget` + getter；构造函数 `bResident = true` |
| `Public/UI/Controllers/DZ_HUDController.h` + `.cpp` | 创建 HUD 根；通过管理器工厂创建三个子 controller 并注入 widget；facade：`RefreshStatusBars` / `RefreshAbilityBar` / `HideAbilityBar` |
| `Public/UI/Widgets/HUD/DZ_RaidTime.h` + `.cpp` | 计时器空壳类（供 `BindWidget` 类型 + controller 强类型引用） |
| `Public/UI/Controllers/DZ_RaidTimeController.h` + `.cpp` | MVVM 驱动：`SetRaidTimeWidget` 接收控件 → 建 VM → 注入 → Tick 推秒数 |
| `Public/UI/ViewModel/DZ_RaidTimeViewModel.h` + `.cpp` | `FText TimeText`（FieldNotify）+ `SetElapsedSeconds` + `FormatElapsedSeconds` |

**修改**

| 文件 | 改了什么 |
|---|---|
| `Public/UI/UIManager.h` + `.cpp` | `CreateController` 移到 public 供 HUD 控制器使用（保持无参，不引入可变参数模板） |
| `Public/UI/DZ_RaidUIManager.h` + `.cpp` | 只保留 `HUDController` + 四个面板 controller；不再持有 HUD 子 controller |
| `Public/UI/Controllers/DZ_UIControllerBase.h` + `.cpp` | 新增 `GetOwningManager()`（`Cast<UUIManager>(GetOuter())`） |
| `Private/UI/Controllers/DZ_StatusBarsController.cpp` | 新增 `SetStatusBarsWidget(UDZ_StatusBars*)` 接收注入；不重写 `Initialize`；Shutdown 只清引用 |
| `Private/UI/Controllers/DZ_AbilityBarController.cpp` | 新增 `SetAbilityBarWidget(UDZ_AbilityBar*)`，其余同上 |
| `Public/Data/UI/DZ_UIConfig.h` | 新增 `TSubclassOf<UDZ_HUDLayout> HUDClass`；`StatusBarsClass` / `AbilityBarClass` 保留但已不再使用（回滚用） |
| `Private/Player/DZ_RaidPlayerController.cpp` | 三处调用改走 HUD facade：`RefreshStatusBars` / `RefreshAbilityBar` / `HideAbilityBar` |
| `DecayZone.Build.cs` | 已在依赖里加 `ModelViewViewModel` |

## 5. 配置项（编辑器里要做）

| 项 | 要求 |
|---|---|
| `DA_UIConfig.HUDClass` | ✅ 已指向 `WBP_HUD`（`/Game/Data/UIConfig/DZ_UIConfig`） |
| `DA_UIConfig.ZOrderMap` | 面板类条目保持不变；`WBP_HUD` 未配（默认 0，HUD 根只需在面板之下）。`WBP_AbilityBar` 那条已经失效，保留只为回滚 |
| `WBP_HUD` 的 View Bindings | 本文件不涉及；`WBP_RaidTime` 自己配（见 `WBP_RaidTime.md` §3） |

## 6. 迁移时的注意事项

1. **`Open()` 语义**：子控件嵌进 HUD 根后**不能再调 `Open()`**，否则会被当成独立 viewport 控件再叠一层。子控件的显隐用 `Show()` / `Hide()`（内部是 `SetVisibility`）。
2. **输入模式**：HUD 子控件不进 `CurrentActivateWidgets`，`UUIManager::UpdateInputMode` 不受影响；以后若要某个 HUD 元素可点击（比如鼠标点技能栏），要单独设计，不能靠现在的栈。
3. **控件名即契约**：`StatusBars` / `AbilityBar` / `RaidTime` 三个名字与 `BindWidget` 绑定，改名会让 WBP 导入直接失败（插件会列出缺失项）。
4. **父子 JSON 的职责边界**：`WBP_HUD.json` 只写子节点的 `class` 和 `slot`，不展开子控件内部树；子控件内部树只在各自 JSON 里维护。
5. **过渡期**：压平子控件与 C++ 改引用必须在同一轮落地。只压平不改代码会出现「根是 SizeBox 却被 `AddToViewport`」的错乱显示。
6. **子 controller 不对外暴露**：外部只调 `UDZ_HUDController` 的 facade（`RefreshStatusBars` / `RefreshAbilityBar` / `HideAbilityBar`）。需要新能力时在 HUD 控制器上加方法，不要把子 controller 指针漏出去。
7. **Tick / Shutdown 由管理器驱动**：子 controller 通过 `UUIManager::CreateController` 创建，登记在 `Controllers` 数组里，`TickControllers` 和 `DestroyControllers` 都会覆盖到。不要给 HUD 控制器重写 `Tick` 再去转发子项，否则会出现双重 Tick。

## 7. 未完成项

| 项 | 归属 | 说明 |
|---|---|---|
| `WBP_RaidTime` 的 View Bindings | 你 | 见 `WBP_RaidTime.md` §3 |
| PIE 单机 / 双人验收 | 程序 | 见 §8 |
| 交互提示 / 进度条是否迁入 HUD 根 | 待定 | 本轮范围外 |
| `StatusBarsClass` / `AbilityBarClass` 删除 | 后续 | 稳定一轮后再删 |

## 8. 验收

| 场景 | 期望 |
|---|---|
| 进局 | 状态条、技能栏、计时器一次出现，位置与改动前一致 |
| 窗口缩放 | 三个元素边距稳定，不重叠 |
| 打开背包 / 搜刮 | HUD 仍在、不遮挡面板、输入模式正常 |
| `UDZ_HUDLayout::Hide()` | 三件一起消失，技能栏 controller 的刷新不报错 |
| 退出局内 | `Shutdown` 后无残留控件、无 ensure |
| 双人 PIE | 两个客户端的计时器、状态条数值都正确 |

## 9. 回滚

- 代码：`UDZ_StatusBarsController` / `UDZ_AbilityBarController` / `UDZ_RaidUIManager` / `DZ_UIConfig.h` 的改动可以用 `git diff` 单独还原；`StatusBarsClass` / `AbilityBarClass` 字段刻意保留，直接切回 `CreateWidget` + `Open()` 即可恢复旧行为。
- 资产：三个子控件的 JSON 压平不是必须的（旧的 `RootCanvas` 包裹仍能工作），但回滚时需要恢复 JSON 并让插件重新导入。
