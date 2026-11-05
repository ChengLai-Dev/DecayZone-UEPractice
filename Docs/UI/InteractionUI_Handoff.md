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

### 3.1 焦点 → 提示（已有骨架）

```
ADZ_PlayerCharacter::Tick
└─ HighlightInteractable()
   ├─ FindTargetInteractableActor() 选出朝向最正的重叠物
   └─ 焦点变化时：
      ├─ OnFocusInteractableActorChanged.Broadcast(Target)   ← 已实现
      └─ SetActorHighlight(新, true) / (旧, false)            ← 已实现

ADZ_PlayerController::HandleInteractableActorChanged(AActor*)
   ├─ OnPossess 里已 AddDynamic 绑定                          ← 已实现
   └─ 函数体为空                                              ← 未实现
```

### 3.2 按键 → 读条（未实现）

```
InteractAction.Started
└─ 焦点 actor 存在且 CanInteract()
   ├─ Prompt 隐藏
   ├─ Progress 显示，按 GetInteractDuration() 计时
   ├─ 每帧写 Img_RingFill 的 Progress + Text_Time
   └─ 完成 / 取消 → OnInteractCompleted / OnInteractCancelled → Progress 隐藏
```

---

## 4. 程序侧现状

| 文件 | 已有什么 | 缺什么 |
|---|---|---|
| `UDZ_InteractPrompt` | `BindWidget` 的 `Text_Key` / `Text_Action` | 设文案、显示、隐藏的对外方法 |
| `UDZ_InteractProgress` | `BindWidget` 的 `Img_RingFill` / `Text_Time`、`StartCountDownSeconds` / `CancelCountDown` | 进度方向（现在传的是剩余比例）、完成帧收尾、动态材质判空 |
| `UDZ_UIConfig` | `InteractPromptClass` 字段 | `InteractProgressClass`、`LootPanelClass` 还没加 |
| `ADZ_PlayerController` | `InteractPrompt` 成员、`BeginPlay` 里 `CreateWidget`、`OnPossess` 绑定 | 展示逻辑、`AddToViewport`、`OnUnPossess` 解绑 |
| `ADZ_PlayerCharacter` | 焦点变化广播、`GetHighlightActor()` | 焦点判定方向与门槛（见 5.3） |
| `IDZ_Interactable` | 6 个接口函数（含 `GetInteractPromptText`） | — |
| `ADZ_InteractableActor` | 已实现接口，有 `InteractDuration` / `InteractPromptText` | `OnInteractStarted/Completed/Cancelled` 仍是空默认实现，留给蓝图子类 |

---

## 5. 当前断点（按优先级）

1. **提示显示逻辑还差收尾**：`HandleInteractableActorChanged` 里已有 `AddToViewport` / 写文案，但每次焦点变化都重复 `AddToViewport`（引擎会警告），应改成常驻 + 切 Visibility。
2. **交互入口还没接**：`InteractAction` 未绑定，"按 E 开始 / 再按取消 / 时长为 0 直接完成"三条分支都还没写。
3. **焦点判定会选中背后的物体**：`FindTargetInteractableActor` 用的是角色朝向（受 `bOrientRotationToMovement` 影响，其实是移动方向），且没有点积门槛。应改成相机/控制朝向 + 门槛。
4. **`WBP_InteractPrompt` / `WBP_InteractProgress` 的父类**：需要重挂到 `UDZ_InteractPrompt` / `UDZ_InteractProgress`，否则 `CreateWidget<T>` 运行时会拿到 null。
5. **读条逻辑未接**：`UDZ_InteractProgress` 有计时和回调，但进度方向用的是剩余比例（环会倒退），且完成帧没有提前 return。

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
