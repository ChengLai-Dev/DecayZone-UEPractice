# 交互提示 UI 交接总览

这份文档记录**资产、数据流、程序侧接口、以及当前还没接完的部分**。单控件级别的"程序要接哪个节点"见各自文档：

- `Docs/UI/WBP_InteractPrompt.md`
- `Docs/UI/WBP_InteractProgress.md`

---

## 1. 当前资产

| 资产 | 作用 | C++ 父类 | 状态 |
|---|---|---|---|
| `/Game/UI/Interaction/WBP_InteractPrompt` | 准星下方的按键提示 | `UDZ_InteractPrompt`（已建，**父类是否已重挂待确认**） | 界面完成，逻辑未接 |
| `/Game/UI/Interaction/WBP_InteractProgress` | 圆形读条 + 倒计时数字（服务所有需等待的交互） | `UDZ_InteractProgress`（已建，父类待重挂） | 界面完成，逻辑未接 |
| `/Game/UI/Materials/M_LootRing` | 环形进度材质（UI 域 / Translucent / Unlit） | — | 完成 |
| `/Game/UI/Materials/MI_LootRing` | 填充环（金色，`Progress` 运行时写） | — | 完成 |
| `/Game/UI/Materials/MI_LootRing_BG` | 底环（暗色，`Progress` 固定 1） | — | 完成 |

源 JSON 在 `UIWidgets/UI/Interaction/`。

---

## 2. 控件组成

```
WBP_InteractPrompt
└─ RootCanvas（全屏，SelfHitTestInvisible）
   └─ Size_Prompt（AutoSize）
      └─ HB_Prompt
         ├─ Size_Key 最小 40×40
         │  └─ Border_KeyOutline（1px 描边）
         │     └─ Border_KeyFill
         │        └─ Text_Key      "E"
         └─ Size_Action
            └─ Border_ActionOutline（1px 描边）
               └─ Border_ActionFill
                  └─ Text_Action  "搜刮"

WBP_InteractProgress
└─ RootCanvas（全屏，SelfHitTestInvisible）
   └─ Size_Ring 96×96
      └─ Overlay_Ring
         ├─ Img_RingBG    ← MI_LootRing_BG
         ├─ Img_RingFill  ← MI_LootRing
         └─ Text_Time     "0.0"
```

**两个 WBP 的锚点与偏移保持一致**：`anchors (0.5, 0.5)`、`alignment (0.5, 0)`、`left 220` / `top 120`。
切换提示与读条时是原地替换，不要分别给它们设不同偏移。

---

## 3. 数据流

### 3.1 焦点 → 提示

```
ADZ_PlayerCharacter::Tick
└─ HighlightInteractable()
   ├─ FindTargetInteractableActor() 选出朝向最正的重叠物
   └─ 焦点变化时：
      ├─ OnFocusInteractableActorChanged.Broadcast(Target)
      └─ SetActorHighlight(新, true) / (旧, false)

ADZ_PlayerController::HandleInteractableActorChanged(AActor*)   ← AcknowledgePossession 里 AddDynamic
└─ UDZ_InteractController::OnFocusInteractableChanged(AActor*)
   ├─ 空 Actor → Prompt Close
   ├─ 非 Interactable → 忽略
   └─ Prompt Open + 写键位与文案（键位从 InputConfig 的 IMC 里读）
```

### 3.2 按键 → 读条

```
InteractAction.Started
└─ ADZ_PlayerController::InteractHighlightActor
   └─ UDZ_InteractController::Interact(焦点 Actor)
      ├─ 已在交互中：自动交互直接忽略，手动交互则 Cancel
      └─ BeginInteract：CanInteract 检查 → Tracker.Begin → Prompt 隐藏 / Progress 打开 → ServerBeginInteract

Tick（UDZ_InteractController）
└─ Tracker.Tick + Progress.SetProgress

结束
├─ 服务端 GA 完成 → ClientInteractFinished → Controller::EndInteract
└─ 再次按键 → CancelInteract → EndInteract + ServerCancelInteract
```

---

## 4. 程序侧现状

| 文件 | 已有什么 |
|---|---|
| `UDZ_InteractPrompt` | `BindWidget` 的 `Text_Key` / `Text_Action`；`Open` / `Close` / `SetVisibility` 继承自 `UDZ_UserWidget` |
| `UDZ_InteractProgress` | `BindWidget` 的 `Img_RingFill` / `Text_Time`、`SetProgress(Progress, Remaining)`、动态材质缓存 |
| `UDZ_UIConfig` | `InteractPromptClass` / `InteractProgressClass` / `LootPanelClass` |
| `UDZ_InteractController` | 持有 `FDZ_InteractTracker` 与两个 Widget；`Interact` / `BeginInteract` / `CancelInteract` / `EndInteract` / `UpdateProgress` / `OnFocusInteractableChanged` / `IsBlockingAttack` |
| `ADZ_PlayerController` | `BeginInteract` / `CancelInteract` / `EndInteract` / `NotifyInteractFinished` 保留为对外转发；Server RPC（`ServerBeginInteract` / `ServerCancelInteract` / `ClientInteractFinished`）仍在 PC |
| `ADZ_PlayerCharacter` | 焦点变化广播、`GetHighlightActor()` |
| `IDZ_Interactable` | 6 个接口函数（含 `GetInteractPromptText`） |
| `ADZ_InteractableActor` | 已实现接口；`NotifyAutoInteractStart` / `NotifyAutoInteractCancel` 直接调 PC 的 `BeginInteract` / `CancelInteract` |

> RPC 只能挂在 Actor 上，所以 `ServerBeginInteract` / `ServerCancelInteract` / `ClientInteractFinished` 永远留在 `ADZ_PlayerController`，控制器只通过持有的 PC 指针触发。
>
> `IsBlockingAttack()`（交互进行中且非自动交互）同时用于攻击拦截与 `IsMovementLocked`，这两个判定在控制器搬走后没有退化。

---

## 5. 当前断点（按优先级）

1. **重复 `AddToViewport`**：每次焦点变化都会 `Open()` 一次，引擎会告警。应改成常驻 + 切 Visibility（本轮只是把这段搬到控制器，行为未改）。
2. **焦点判定会选中背后的物体**：`FindTargetInteractableActor` 用的是角色朝向（受 `bOrientRotationToMovement` 影响，其实是移动方向），且没有点积门槛。应改成相机/控制朝向 + 门槛。
3. **打开面板期间提示未隐藏**：面板关闭后焦点没变不会自动重推，需要主动重推一次。

---

## 6. 约定

- 提示与读条都是**屏幕空间、常驻、只切 Visibility**；永远不切输入模式（输入模式只归面板类界面）。
- 打开背包/搜刮面板期间，提示必须隐藏；面板关闭后要**主动**把当前焦点重新推一次（焦点没变不会自动触发广播）。
- 文案与键位都由程序写入，JSON 里的 `E` / `搜刮` 是占位值。
- 键帽是直角：`UBorder.Background` 不在 WidgetJsonBridge 同步白名单内，只能写 `BrushColor`。圆角需在编辑器里改 Brush 的 `DrawAs = RoundedBox`。

---

## 7. 验收

| 场景 | 期望 |
|---|---|
| 走近交互物 | 物体描边亮起，准星下方出现 `[E] 搜刮` |
| 走开 | 提示与描边同时消失 |
| 在两个交互物之间移动 | 提示文案跟随切换，不闪烁 |
| 站定不动、背后有交互物 | **不**出现提示（改完 5.3 之后） |
| 开着提示打开背包 | 提示消失；关闭背包后重新出现 |
| 交互物被销毁 | 提示消失，不残留 |
