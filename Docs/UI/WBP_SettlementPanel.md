# WBP_SettlementPanel 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Settlement/WBP_SettlementPanel` |
| 父类 | `UDZ_SettlementPanel`（C++ 已实现） |
| 源文件 | `UIWidgets/UI/Settlement/WBP_SettlementPanel.json` |
| 尺寸 | 1408×858，屏幕居中 |
| 由谁打开 | 撤离流程（服务端判定成功/失败后，客户端 RPC 打开） |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Text_Title` | `TextBlock` | 成功 → `撤离成功` + 金色；失败 → `撤离失败` + 暗红 |
| `Text_Subtitle` | `TextBlock` | 写 `本次行动用时 mm:ss` |
| `StatCard_Value` | `WBP_SettlementStatCard` | 传 `TotalValue`，标签 `总价值` |
| `StatCard_EnemyKills` | `WBP_SettlementStatCard` | 传 `EnemyKills`，标签 `击杀丧尸` |
| `StatCard_PlayerKills` | `WBP_SettlementStatCard` | 传 `PlayerKills`，标签 `击杀玩家` |
| `Text_LootCount` | `TextBlock` | 写 `共 N 件`，N = `Items` 总数量（不是格子数） |
| `WrapBox_Items` | `WrapBox` | **动态生成** `WBP_SettlementItemSlot`；先 ClearChildren 再按 `Items` 逐个 `AddChild`，自动换行，不需要算行列 |
| `ScrollBox_Items` | `ScrollBox` | 物品超过 10 格时自动滚动，无需额外设置 |
| `Text_EmptyHint` | `TextBlock` | 开局 `Collapsed`；`Items` 为空时改 `Visible`，同时把 `WrapBox_Items` 设为 `Collapsed` |
| `Btn_Confirm` | `Button` | **点击 → 关闭结算面板**（切回 `GameOnly` + 隐藏鼠标；是否传送/退出由服务端决定） |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `RootCanvas` / `Size_Root` | CanvasPanel / SizeBox | 根与 1408×858 居中尺寸 |
| `Border_Outline` / `BG_Panel` | Border | 1px 描边 + 面板底 |
| `VB_Main` | VerticalBox | 主纵向布局 |
| `VB_Header` / `HB_Stats` / `HB_LootHeader` / `HB_Footer` | 布局容器 | 标题、统计、列表头、底栏 |
| `Size_Divider_*` / `Divider_*` | SizeBox / Border | 三条 1px 分隔线 |
| `Text_SectionTitle` | TextBlock | 固定文案 `带出物品` |
| `Text_Hint` | TextBlock | 固定提示，文案可按需改 |
| `Spacer_LootHeader` / `Spacer_Footer` | Spacer | 撑开左右两端 |
| `Size_Confirm` / `Text_Confirm` | SizeBox / TextBlock | 按钮尺寸与文案 |
| `ItemSlot_1` … `ItemSlot_10` | 子控件 | 仅 Designer 占位，运行时应全部清掉后重建 |

---

## 交互

| 触发来源 | 事件 | 程序要做什么 |
|---|---|---|
| 外部 | 撤离判定完成 | 把本面板加入视口，切 `GameAndUI` + 显示鼠标；调用数据设置接口填充全部字段 |
| 外部 | 玩家死亡（若也弹结算） | 同上，`bSucceeded=false`，物品区显示「已丢失」语义（把 `Text_SectionTitle` 改文案 + 格子降透明度） |
| `Btn_Confirm` | 左键单击 | 关闭面板：从视口移除，恢复 `GameOnly` + 隐藏鼠标 |
| 面板外区域 | 点击 | **不响应**，必须点「确认」关闭（结算界面不做点击穿透） |

> 打开时机注意：`ClientOpenSettlementPanel` 是 Client RPC，调用点必须在服务端且玩家 Pawn 仍有效。如果撤离成功要立刻销毁/隐藏角色，先发 RPC 再处理角色。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 撤离成功 | `Text_Title` | `撤离成功`，颜色 `0.584/0.397/0.138`（金） |
| 撤离失败 | `Text_Title` | `撤离失败`，颜色 `0.522/0.122/0.122`（暗红） |
| 有物品 | `WrapBox_Items` / `Text_LootCount` | 网格可见并填充，件数为总数量 |
| 无物品 | `WrapBox_Items` / `Text_EmptyHint` | 网格 `Collapsed`，提示 `Visible` |
| 物品超过两行 | `ScrollBox_Items` | 出现竖向滚动条 |

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `StatCard_Value` / `StatCard_EnemyKills` / `StatCard_PlayerKills` | `WBP_SettlementStatCard` | 设置数值与标签 | 见 [WBP_SettlementStatCard.md](WBP_SettlementStatCard.md) |
| `ItemSlot_*` | `WBP_SettlementItemSlot` | 设置物品图标与数量 | 见 [WBP_SettlementItemSlot.md](WBP_SettlementItemSlot.md) |

---

## 视觉规格

| 项 | 值 |
|---|---|
| 面板底 | `0.0075 / 0.0123 / 0.0160`，α 0.98 |
| 描边 | `0.0273 / 0.0452 / 0.0578`，1px |
| 卡片底 | `0.0152 / 0.0242 / 0.0307` |
| 标题字号 | 40 Bold 居中 |
| 副标题 / 按钮字号 | 14.3 Bold |
| 数值卡字号 | 34 Bold 居中 |
| 卡片标签 / 列表头字号 | 12.1 Bold |
| 字体 | Roboto Bold（与全项目一致） |
