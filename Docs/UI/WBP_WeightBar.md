# WBP_WeightBar 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Inventory/WBP_WeightBar` |
| 父类 | `UUserWidget`（未接 C++） |
| 源文件 | `UIWidgets/UI/Inventory/WBP_WeightBar.json` |
| 尺寸 | 宽随父级、高 9 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Bar_Fill` | Border | 金色填充段。程序改它在水平盒里的宽度比例 = 当前负重比例 |
| `Spacer_Rest` | Spacer | 剩余段。比例必须与 `Bar_Fill` 互补，否则总宽会错 |

> 两段都用 Fill 权重；只改其中一段会出现「填充变长但总宽不变」的错位。当前静态值是 `0.6125 / 0.3875`（24.5 / 40 KG）。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Root_Size` | SizeBox | 条高 9，固定 |
| `Bar_Track` | Border | 轨道底色 + 1px 边，纯装饰 |
| `HB_Bar` | HorizontalBox | 两段的排布容器 |

---

## 交互

无。纯显示控件，不接收事件。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 比例变化 | `Bar_Fill` + `Spacer_Rest` | 两段 Fill 权重之和恒为 1 |

---

## 含有的子控件

无。
