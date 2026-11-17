# WBP_HUD 交互文档

常驻 HUD 的根控件：整屏只创建一次，内部承载状态条、技能栏、局内计时器。子控件的**位置与层级集中在这里**，各子控件自己只管内容和尺寸。

## 1. 资产

| 项 | 值 |
|---|---|
| 资产路径 | `/Game/UI/HUD/WBP_HUD` |
| JSON 源 | `UIWidgets/UI/HUD/WBP_HUD.json` |
| 父类 | `UDZ_HUDLayout`（继承 `UDZ_UserWidget`，`bResident = true`） |
| 设计尺寸 | `Desired` |
| 创建者 | `UDZ_HUDController::Initialize` → `UDZ_UIConfig::HUDClass` |
| 层级 | `ZOrderMap` 未配置时默认 0；面板类控件在其之上，`WBP_Tips`（`AddToViewport(999)`）在最上层 |

## 2. 控件树

```
RootCanvas (CanvasPanel, SelfHitTestInvisible)
├─ StatusBars  → WBP_StatusBars   锚点 (0,1)   轴心 (0,1)   偏移 left=40,  top=-40   bAutoSize
├─ AbilityBar  → WBP_AbilityBar   锚点 (0.5,1) 轴心 (0.5,1) 偏移 left=0,   top=-36   bAutoSize
└─ RaidTime    → WBP_RaidTime     锚点 (1,0)   轴心 (1,0)   偏移 left=-28, top=18    bAutoSize
```

- 三个子控件都是**设计期子控件**（JSON 里 `class` 指向各自 WBP 的 `_C` 类），不是运行时 `CreateWidget`。
- 点锚点 + `bAutoSize`：槽位大小跟随子控件期望尺寸；子控件内部不再有 `RootCanvas`，尺寸由各自根 `SizeBox` 决定。
- 层级由控件树顺序决定（`StatusBars` → `AbilityBar` → `RaidTime`），三者不重叠，顺序只影响理论上的遮挡。

## 3. BindWidget 契约

`UDZ_HUDLayout` 用 `BindWidget` 拿子控件引用，**节点名必须与 C++ 成员名完全一致**：

| 节点名 | C++ 成员 | 类型 | 用途 |
|---|---|---|---|
| `StatusBars` | `StatusBars` | `UDZ_StatusBars*` | `UDZ_StatusBarsController` 刷新生命/能量 |
| `AbilityBar` | `AbilityBar` | `UDZ_AbilityBar*` | `UDZ_AbilityBarController` 刷新槽位 |
| `RaidTime` | `RaidTime` | `UDZ_RaidTime*` | `UDZ_RaidTimeController` 注入 ViewModel |

改名、删节点都会让 WBP 导入失败（插件会列出缺失项），这是有意的保护。

## 4. 显示方式

- **常驻一次**：`UDZ_HUDController` 创建后调 `Open()`，`bResident = true` 走 `UUIManager::OpenWidget(widget, true)`，不进 `CurrentActivateWidgets`，**不参与输入模式切换**。
- **子 controller**：`UDZ_HUDController` 通过 `UUIManager::CreateController` 创建三个子 controller 并注入对应子控件；Tick 与 Shutdown 由管理器统一驱动。对外只暴露 facade（`RefreshStatusBars` / `RefreshAbilityBar` / `HideAbilityBar`），不暴露子 controller 指针。
- **整体显隐**：`UDZ_HUDLayout::Hide()` / `Show()` 即可一键藏起整屏 HUD（截图、过场、死亡观战的入口）。
- **输入**：根 canvas 与三个子控件根都是 `SelfHitTestInvisible`，不拦鼠标。
- **销毁**：`UDZ_HUDController::Shutdown` → `RemoveFromParent`，整棵树一起销毁；其它 controller 只清引用。

## 5. 加一个常驻元素的步骤

1. 新 WBP 的 JSON 根节点用 `SizeBox`/`Border` 等**非 Canvas** 容器（位置交给 HUD 根，别在子控件里再写一套锚点）。
2. 在 `WBP_HUD.json` 的 `children` 里加一个节点，`class` 写新 WBP 的 `_C` 路径，槽位写 CanvasPanelSlot + `bAutoSize`。
3. `UDZ_HUDLayout` 加一个同名 `BindWidget` 成员 + getter。
4. 新 controller 从 `GetHUDLayout()` 取子控件，在 `UDZ_RaidUIManager::CreateControllers` 里注册（HUD 根之后）。
5. 更新本文件与对应 WBP 文档。

**不进 HUD 根的**：`WBP_Tips`（瞬发、多实例、要压在所有 UI 之上）、`WBP_InteractPrompt` / `WBP_InteractProgress`（本轮未迁，位置在屏幕中下，牵动交互流程）。

## 6. 验收

| 场景 | 期望 |
|---|---|
| 进局 | 左下状态条、底部技能栏、右上计时器同时出现，位置与改动前一致 |
| 窗口缩放 | 三个元素保持各自的边距（左 40 / 底 36、40 / 右 28、上 18） |
| 打开背包 / 搜刮面板 | HUD 仍在，不被遮挡；输入模式正常切到 `GameAndUI` |
| 鼠标点 HUD 区域 | 不拦截输入，不显示手型光标 |
| `Hide()` HUD 根 | 三个元素一起消失；`Show()` 一起回来 |
| 退出局内 / 切关卡 | HUD 整棵销毁，无残留、无 ensure |
