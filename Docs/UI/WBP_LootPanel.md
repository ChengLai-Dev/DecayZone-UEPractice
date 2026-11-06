# WBP_LootPanel 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Loot/WBP_LootPanel` |
| 父类 | `UUserWidget`（未接 C++，建议 `UDZ_LootPanel`） |
| 源文件 | `UIWidgets/UI/Loot/WBP_LootPanel.json` |
| 尺寸 | 1408×858，屏幕居中 |

---

## 需要程序接的节点

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `EquipmentPanel` | `WBP_EquipmentPanel` | 左栏装备区；设置 8 个装备槽 |
| `BackpackPanel` | `WBP_BackpackPanel` | 中栏背包区；设置 20 格物品、负重文本/条 |
| `LootListPanel` | `WBP_LootListPanel` | 右栏战利品区；设置容器名/件数、动态生成行 |

> 节点名一律写对象名（`GetName()` / JSON 的 `name` 字段）。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `RootCanvas` / `Size_Root` | CanvasPanel / SizeBox | 根与 1408×858 居中尺寸 |
| `Border_Outline` / `BG_Panel` | Border | 1px 描边 + 面板底 |
| `VB_Main` / `HB_Regions` | VerticalBox / HorizontalBox | 三栏与底栏的主布局 |
| `Panel_Footer` / `HB_Footer` | Border / HorizontalBox | 底栏布局 |
| `Spacer_Footer` | Spacer | 把关闭提示推到右侧 |
| `Text_Hint` | TextBlock | 固定提示 `拖拽物品到背包，或点「拿取」` |
| `Text_CloseHint` | TextBlock | 固定文案 `[L] 关闭` |

---

## 交互

| 触发来源 | 事件 | 程序要做什么 |
|---|---|---|
| 外部 | 打开搜刮 | 把本面板加入视口，切到 `GameAndUI` + 显示鼠标；若背包面板已打开先关闭 |
| `LootListPanel` 的 `Btn_Close` | 左键单击 | 关闭面板：从视口移除，恢复 `GameOnly` + 隐藏鼠标 |
| `LootListPanel` 的行 | 左键拖拽 | 开始拖拽；拖影复制当前行；松手时判断落点 |
| `LootListPanel` 的行内 `Btn_Take` | 左键单击 | 整叠移入背包：先并入同物品格，再找第一个空格；成功则移除该行 |
| `LootListPanel` 的行内 `Btn_Take` | 左键单击 + Shift | 只移入 1 件：该行数量减 1，减到 0 再移除行 |
| `BackpackPanel` 的背包格 | 收到拖放 | 空格 → 放入该物品和数量；同物品 → 数量相加；不同物品 → 忽略；落在面板空白处 → 忽略 |
| 移入成功（拖放 / 按钮） | — | 1) 从 `LootListPanel` 移除该行（按钮取 1 件时改为减数量）；2) 刷新件数与空容器提示；3) 刷新 `BackpackPanel` 负重文本/条 |
| 背包满 / 超重 | — | 本期不做反馈：操作不生效，行保留，界面无提示 |
| 容器变空 | — | 只显示「容器已空」，**不自动关闭**面板 |

> 拖拽的源是 `WBP_LootRow`，落点是 `WBP_BackpackPanel` 里的 `WBP_ItemSlot`。行只负责开始拖拽，落点判断和数量合并归本面板。
> 「拿取」按钮与拖拽必须走**同一条**移入背包的处理（目标格可为空 → 自动选格），不要各写一套规则。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 打开 / 关闭 | 面板整体 | 由外部加入视口 / 移除 |
| 容器有物品 | `LootListPanel` | 列表显示、件数为总数量 |
| 容器空 | `LootListPanel` | 列表清空、显示「容器已空」 |
| 负重变化 | `BackpackPanel` | 负重文本与负重条比例同步 |
| 拖拽悬停 | `BackpackPanel` 内的格子 | 格子边框刷亮金色，表示可放下 |

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `EquipmentPanel` | `WBP_EquipmentPanel` | 设置 8 个装备槽的图标/名称/选中 | 见 [WBP_EquipmentPanel.md](WBP_EquipmentPanel.md) |
| `BackpackPanel` | `WBP_BackpackPanel` | 设置 20 格物品/数量/负重；接收拖放落点 | 见 [WBP_BackpackPanel.md](WBP_BackpackPanel.md) |
| `LootListPanel` | `WBP_LootListPanel` | 设置容器名/件数；动态生成战利品行；空容器提示 | 见 [WBP_LootListPanel.md](WBP_LootListPanel.md) |
