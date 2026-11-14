# WBP_Tips 交互文档

屏幕上方（垂直 20% 处）的通用提示条，用于弹出一次性消息（拾取失败、操作被拒绝、系统提示等）。

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/HUD/WBP_Tips` |
| 父类 | `UDZ_Tips` |
| 源文件 | `UIWidgets/UI/HUD/WBP_Tips.json` |
| 设计尺寸 | `Desired` |
| 建议 ZOrder | 100（最高层，不被背包/搜刮界面遮挡） |

---

## 1. 结构

```
RootCanvas (CanvasPanel, SelfHitTestInvisible)
└─ Size_Tips (SizeBox, bAutoSize, 锚点 0.5/0.2, alignment 0.5/0.5)
   └─ Border_TipsOutline (Border, 1px 深色描边)
      └─ Border_TipsFill (Border, 20/10 内边距, 深色底)
         └─ Text_Tips (TextBlock, 20px Bold, 居中)
```

**位置**：`anchors.min/max.y = 0.2`（屏幕高度 20% 处）+ `alignment.y = 0.5`（控件以自身中心贴锚点），所以文字变长时上下对称扩展。水平保持 `anchors.x = alignment.x = 0.5` 居中。

根节点当前是默认 `Visibility`（`Visible`）。若要让提示条不拦截鼠标，需要在 JSON 里给 `RootCanvas` 或 `Size_Tips` 设 `SelfHitTestInvisible`——当前资产还没有设，属于待确认项（见 §6 验收）。

---

## 2. 需要程序接的节点

| 节点 | 类型 | 用途 |
|---|---|---|
| `Text_Tips` | `UTextBlock` | 显示提示文本 |

其余节点纯装饰，不需要接。

---

## 3. 交互

| 触发 | 程序要做什么 |
|---|---|
| 业务侧调用显示接口 | 设置 `Text_Tips` 文本，播放淡入 → 停留 → 淡出 |
| 停留结束 | 自动隐藏，不残留 |

**淡入淡出动画**：已由 `UIWidgets/UI/HUD/WBP_Tips.json` 的 `animations` 定义，导入后资产自带两条 `WidgetAnimation`：

| 动画 | 时长 | 作用 |
|---|---|---|
| `Anim_FadeIn` | 0.25s | `Size_Tips`：`RenderOpacity` 0→1；`RenderTransform.Translation` (0,24)→(0,0) |
| `Anim_FadeOut` | 0.3s | `Size_Tips`：`RenderOpacity` 1→0；`RenderTransform.Translation` (0,0)→(0,-12) |

程序侧直接 `PlayAnimation(Anim_FadeIn)` / `PlayAnimation(Anim_FadeOut)`，**不要再用 C++ 驱动 `SetRenderOpacity`**，否则会和资产动画互相覆盖。

---

## 4. 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 隐藏 | `Size_Tips` `RenderOpacity` | 0 |
| 显示中 | `Size_Tips` `RenderOpacity` | 0 → 1 → 0 |
| 文本更新 | `Text_Tips` | 调用方传入的文本 |

---

## 5. 实现状态

**已完成**

| 项 | 位置 |
|---|---|
| `UDZ_Tips` 类，两个动画回调绑定 | `Source/.../Public/UI/Widgets/HUD/DZ_Tips.h` + `.cpp` |
| `UDZ_UIConfig::TipsClass` 字段 | `Source/.../Public/Data/UI/DZ_UIConfig.h` |
| 创建入口 `UIHelper::ShowTips(WorldContext, Text, Duration)` | `Source/.../Private/Library/UIHelper.cpp` |

**待补**

1. **`Text_Tips` 没接**：`UDZ_Tips` 没有 `BindWidget` 的 `UTextBlock` 成员，`ShowTips` 也没调 `SetText`。当前屏幕上永远是 JSON 里的默认文本「提示消息」。
2. **Timer 句柄是局部变量**：`HandleFadeInComplete` 里用的是函数内 `FTimerHandle`，函数返回后无法取消。应改用成员 `HoldTimerHandle`。
3. **ZOrder 用了魔数**：`UIHelper::ShowTips` 里是 `AddToViewport(999)`，应改走 `UIHelper::GetZOrder`（并在 `DA_UIConfig.ZOrderMap` 里给 `WBP_Tips_C` 配值）。
4. **owner 不统一**：`UIHelper::ShowTips` 用 `UWorld` 作 owner，项目其他 Widget 都用 `PlayerController`。用 World 会让 Widget 活到世界销毁，PIE 里更容易被旧世界引用链拖住。
5. **`Duration` 无兜底**：调用方传 0.1 秒时，淡出会打断 0.25 秒的淡入。建议 `FMath::Max(Duration, Anim_FadeIn->GetEndTime())`。
6. **`DA_UIConfig` 配置**：`TipsClass` 是否已指向 `WBP_Tips`、`ZOrderMap` 是否已加条目，需在编辑器里核对。

---

## 5.1 动画时序

`UDZ_Tips` 当前实现走的是「回调」路线，两条动画各绑一个 `BindToAnimationFinished`：

```
ShowTips(Text, Duration)
  └─ PlayAnimation(Anim_FadeIn)         0.25s 淡入 + 下移 24px 上滑
       └─ [Anim_FadeIn Finished] → 起 Timer(Duration)
            └─ PlayFadeOutAnim()        PlayAnimation(Anim_FadeOut)
                 └─ [Anim_FadeOut Finished] → RemoveFromParent()
```

注意两点：

- 回调只能绑在 `NativeOnInitialized` 里绑一次。每调一次 `BindToAnimationFinished` 就往 `AnimationCallbacks` 里加一条，重复绑会让回调触发多次。
- `FWidgetAnimationDynamicEvent` 是零参数委托，所以两个动画必须绑到**两个不同的 UFUNCTION**，不能在同一个回调里靠参数区分。

---

## 6. 验收

| 场景 | 期望 |
|---|---|
| 调用显示接口 | 屏幕上方 20% 处淡入一条提示，停留后淡出消失 |
| 连续快速调用两次 | 两个独立实例在同一位置重叠，后出现的盖住前一个，各自按自己的时机消失 |
| 提示显示期间操作鼠标 | 不挡输入 |
| 打开背包 / 搜刮界面 | 提示仍在最上层 |
| 停止调用 | 停留时间到后自动消失，不残留 |

---

## 7. 已知限制

- 每次调用创建一个独立实例，**不排队、不合并**。同一位置同时出现两条时会互相覆盖（透明度叠加后更难读），需要收敛时再改成单例 + 队列。
- 文本长度不限制，超长会撑宽整个提示条。要换行得给 `Text_Tips` 设 `AutoWrapText` 和 `WrapTextAt`。
- 没有音效。需要的话在显示接口里加 `UGameplayStatics::PlaySound2D`。
