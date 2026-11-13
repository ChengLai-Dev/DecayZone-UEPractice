# WBP_SettlementStatCard 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Settlement/WBP_SettlementStatCard` |
| 父类 | `UDZ_SettlementStatCard`（C++ 已实现） |
| 源文件 | `UIWidgets/UI/Settlement/WBP_SettlementStatCard.json` |
| 尺寸 | 宽由父容器 Fill 决定，高固定 118 |
| 使用处 | `WBP_SettlementPanel` 的 `HB_Stats` 内，共用 3 份 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Text_StatValue` | `TextBlock` | 写数值，如 `12500` |
| `Text_StatLabel` | `TextBlock` | 写标签，如 `总价值` |

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `Root_Size` | SizeBox | 固定高 118 |
| `Border_Outline` / `BG_Card` | Border | 1px 描边 + 卡片底 |
| `VB_Card` | VerticalBox | 数值在上、标签在下 |

---

## 交互

无。纯展示，不响应鼠标。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 数值为 0 | `Text_StatValue` | 显示 `0`，**不隐藏**卡片，避免布局跳动 |
| 失败结算 | 整卡 | 保持原样式；是否需要降透明度由父面板统一控制 |

---

## 已实现的接口

| 接口 | 参数 | 说明 |
|---|---|---|
| `SetStat` | `int32 Value`、`const FText& Label` | 整数指标，内部 `FText::AsNumber` |
| `SetStat` | `float Value`、`const FText& Label` | 小数指标，内部保留 1 位小数 |

> 数值是否加千分位由调用方决定，本类不做格式化，直接显示传入文本。
