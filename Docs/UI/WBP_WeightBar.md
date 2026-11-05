# WBP_WeightBar 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_WeightBar` |
| 父类 | `UUserWidget`（未接 C++，建议 `UDZ_WeightBar`） |
| 源文件 | `UIWidgets/UI/Inventory/WBP_WeightBar.json` |
| 尺寸 | 宽随父级、高 9 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Bar_Weight` | ProgressBar | 设置 `Percent`（0..1）= 当前负重 / 上限；`FillColorAndOpacity` 已设为金色；`WidgetStyle` 用引擎默认 |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Root_Size` | SizeBox | 根高度固定 9、宽度随父级，纯布局 |

---

## 交互

无。纯显示控件。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 比例变化 | `Bar_Weight` | `Percent` = `FMath::Clamp(当前重量 / 上限, 0, 1)` |
| 超重（可选） | `Bar_Weight` 的 `FillColorAndOpacity` | 需要时改成红色；默认保持金色 |

---

## 含有的子控件

无。

> 样式说明：当前用 `ProgressBar` 的默认 `WidgetStyle`，JSON 里不写 `WidgetStyle`。以后要做纯色/透明背景的扁平样式，可以给 JSON 顶层加 `"structMode": "object"`，把 `WidgetStyle` 写成对象（见 `ue-umg` 技能的 `json-bridge.md`），或在 `UDZ_WeightBar` 的 C++ 里设置 `FProgressBarStyle`。
