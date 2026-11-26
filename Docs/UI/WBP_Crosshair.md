# WBP_Crosshair 交互文档

屏幕中心的静态准心。挂在 `WBP_HUD` 下，不单独创建、不响应用户输入。

## 1. 资产

| 项 | 值 |
|---|---|
| 资产路径 | `/Game/UI/HUD/WBP_Crosshair` |
| JSON 源 | `UIWidgets/UI/HUD/WBP_Crosshair.json` |
| 贴图 | `/Game/UI/Textures/T_Crosshair` |
| 父类 | `UUserWidget`（默认） |
| 设计尺寸 | `FillScreen` |

## 2. 控件树

```
RootCanvas (CanvasPanel, FillScreen)
└─ Img_Crosshair (Image, 64×64, 锚点与轴心都在屏幕中心)
   Brush = T_Crosshair，DrawAs = Image
```

`Img_Crosshair` 是 `isVariable`，程序可以通过名字取到。

## 3. 程序要接的节点

| 节点 | 类型 | 用途 |
|---|---|---|
| `Img_Crosshair` | `UImage` | 瞄准时收拢或隐藏（由程序决定用哪个） |

没有按钮、没有事件、没有 BindWidget 以外的依赖。

## 4. 界面需要的数据

无。准心本身不显示数值。

## 5. 状态与表现

| 情况 | 表现 |
|---|---|
| 默认 | 显示，64×64，屏幕正中 |
| 瞄准时是否隐藏 / 收拢 | 待定，由我决定 |
| 动态散布（连发时张开） | 未包含，当前只有一张静态图 |

## 6. 布局约束

- 控件自身铺满屏幕，`Visibility = HitTestInvisible`，不挡鼠标。
- 准心贴图固定 64×64，锚点 `(0.5, 0.5)`、轴心 `(0.5, 0.5)`。
- 在 `WBP_HUD` 里作为 `Crosshair` 子节点，铺满全屏，`ZOrder = 10`。

## 7. 未完成项

- 瞄准状态的收拢/隐藏还没有接。
- 动态散布与命中反馈没有做。
