# WBP_InteractPrompt 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Interaction/WBP_InteractPrompt` |
| 父类 | `UUserWidget`（未接 C++） |
| 源文件 | `UIWidgets/UI/Interaction/WBP_InteractPrompt.json` |
| 尺寸 | 内容自适应（键帽最小 40×40 + 文案块 + 6px 间距） |
| 定位 | 锚点 (0.5, 0.5)，`alignment = (0.5, 0)`，`left = 220` / `top = 120` → 屏幕中心右下方 |
| 输入 | 根节点 `Visibility = SelfHitTestInvisible`，纯展示，不抢鼠标/焦点 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Text_Key` | TextBlock | 写入键位显示文字。当前默认 `E`；要做改键联动就由程序写 `InputAction` 的实际按键名 |
| `Text_Action` | TextBlock | 写入动作文案，如 `搜刮` / `开门` / `拾取`。默认值是占位，运行时覆盖 |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `RootCanvas` | CanvasPanel | 全屏根，只做锚点定位 |
| `Size_Prompt` | SizeBox | AutoSize 容器，决定整体定位 |
| `HB_Prompt` | HorizontalBox | 键帽 + 文案块横排 |
| `Size_Key` | SizeBox | 键帽最小 40×40（内容更长时自动撑开） |
| `Border_KeyOutline` | Border | 键帽 1px 描边（`#4A5A63`），靠 Padding=1 露出来 |
| `Border_KeyFill` | Border | 键帽底色（`#2E3C44`），内边距 8/4 |
| `Size_Action` | SizeBox | 文案块最小高 40，与键帽等高对齐 |
| `Border_ActionOutline` | Border | 文案块描边，颜色同键帽 |
| `Border_ActionFill` | Border | 文案块底色，左右内边距 14 |

---

## 交互

无。纯显示控件，不接收点击。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 出现 / 消失 | 整个 Widget | `SetVisibility(Visible / Collapsed)`；如需动画，在蓝图里加 `PromptIn` / `PromptOut` 时间轴（淡入 + 上浮 6px / 淡出） |
| 键位变化 | `Text_Key` | 按当前输入映射写入按键名 |
| 动作文案变化 | `Text_Action` | 由被指向的 actor 提供 |
| 不可交互（灰化，可选） | `Text_Action` 的 `ColorAndOpacity` | 改成次要色 `(0.291771, 0.366253, 0.407240)` |

> 与 `WBP_LootProgress` 共用同一套锚点与 `top = 56`，切换时是原地替换，不会跳位。

---

## 含有的子控件

无。

> 样式说明：键帽没有用贴图，靠「外框 Border + Padding 1 + 内层 Border」做出 1px 描边；JSON 里 `UBorder` 的 `Background` 不在同步白名单内，只能写 `BrushColor`，所以圆角需要后续在编辑器里给 `Background` 设 `DrawAs=RoundedBox`（或换贴图）。
