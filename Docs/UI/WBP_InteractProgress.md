# WBP_InteractProgress 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Interaction/WBP_InteractProgress` |
| 父类 | `UDZ_InteractProgress` |
| 源文件 | `UIWidgets/UI/Interaction/WBP_InteractProgress.json` |
| 尺寸 | 96×96 |
| 定位 | 锚点 (0.5, 0.5)，`alignment = (0.5, 0)`，`top = 56` → 与 `WBP_InteractPrompt` 同位置思路，实际按提示控件调过的偏移对齐 |
| 输入 | 根节点 `Visibility = SelfHitTestInvisible`，纯展示 |

> 这个控件服务**所有需要等待的交互**（搜刮、开门、拆解…），不是搜刮专用。交互时长为 0 的交互不显示它。

---

## 材质

| 资产 | 用途 |
|---|---|
| `/Game/UI/Materials/M_LootRing` | 环形进度材质（UI 域 / Translucent / Unlit），纯程序化，不用贴图 |
| `/Game/UI/Materials/MI_LootRing` | 填充环：`Color` = 金色，`Progress` = 0（运行时逐帧写） |
| `/Game/UI/Materials/MI_LootRing_BG` | 底环：`Color` = 暗色，`Progress` = 1（静态满环，只当刻度背景） |

材质参数：

| 参数 | 类型 | 默认 | 说明 |
|---|---|---|---|
| `Progress` | Scalar | 0 | 0..1，从 12 点方向**顺时针**增长；0 = 空，1 = 满 |
| `Color` | Vector | 白 | 环色；实例里已设成金色 `(0.584, 0.397, 0.138)` |
| `Radius` | Scalar | 0.44 | 环中线半径（UV 归一化），决定环的大小 |
| `Thickness` | Scalar | 0.03 | 环半宽（UV 归一化），≈6px @96 |

> 环的判定是 `1 - saturate(abs(dist - Radius) / Thickness)`，角度用 `atan2` 归一化后与 `Progress` 比较，所以边缘是软过渡、不需要额外贴图。

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Img_RingFill` | Image | 取一次动态材质（建议 `NativeConstruct` 里缓存），每帧 `SetScalarParameterValue("Progress", Alpha)`，`Alpha` 是**已完成比例** |
| `Text_Time` | TextBlock | 写剩余秒数，`FString::Printf(TEXT("%.1f"), Remaining)`；当前默认 `0.0` |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `RootCanvas` | CanvasPanel | 全屏根，只做锚点定位 |
| `Size_Ring` | SizeBox | 固定 96×96 |
| `Overlay_Ring` | Overlay | 三层叠放：底环 / 填充环 / 数字 |
| `Img_RingBG` | Image | 静态底环，引 `MI_LootRing_BG` |

---

## 交互

无。纯显示控件，不接受任何输入。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 开始读条 | 整个 Widget | `SetVisibility(Visible)`；可加 `RingIn` 时间轴（100ms，缩放 0.9 → 1.0） |
| 进度推进 | `Img_RingFill` 的动态材质 | `Progress` = **已完成比例**（`1 - Remaining / Duration`） |
| 倒计时 | `Text_Time` | `Text` = 剩余秒数，一位小数，过 0 要 clamp |
| 完成 / 取消 | 整个 Widget | `SetVisibility(Collapsed)`；可加 `RingOut`（淡出 80ms） |

---

## 含有的子控件

无。

> 依赖：`WBP_InteractProgress` 的 `Img_RingFill` / `Img_RingBG` 直接引用 `MI_LootRing` / `MI_LootRing_BG`，改材质名或移动目录时必须同步改 JSON 里的 `Brush.ResourceObject`。
