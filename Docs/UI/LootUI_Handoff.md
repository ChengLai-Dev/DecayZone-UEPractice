# 搜刮 UI 交接总览

这份文档只记录**控件组成、C++ 拆分建议、拖拽数据流、阶段 B 的替换步骤**。每个 WBP 的节点清单/事件/状态仍以各自的交互文档为准。

---

## 1. 当前资产

| 资产 | 作用 | 父类 |
|---|---|---|
| `/Game/UI/Inventory/WBP_InventoryPanel` | 背包界面三栏组合（已换成共用栏位） | `UDZ_InventoryPanel` |
| `/Game/UI/Inventory/WBP_EquipmentPanel` | 左栏装备区（8 槽） | `UUserWidget`（待接 C++） |
| `/Game/UI/Inventory/WBP_BackpackPanel` | 中栏背包区（20 格 + 负重） | `UUserWidget`（待接 C++） |
| `/Game/UI/Loot/WBP_LootRow` | 战利品行 | `UUserWidget`（建议继承 `UDZ_UserWidget`） |
| `/Game/UI/Loot/WBP_LootListPanel` | 右栏战利品列表 | `UUserWidget`（待接 C++） |
| `/Game/UI/Loot/WBP_LootPanel` | 搜刮界面三栏组合 | `UUserWidget`（待接 C++） |

`WBP_InventoryPanel` 已把 `Panel_Equipment` / `Panel_Backpack` 替换成 `EquipmentPanel` / `BackpackPanel` 实例；`UDZ_InventoryPanel` 里原来的 `Grid_Backpack` 直接绑定已注释，后续由 `UDZ_BackpackPanel` 接管。

---

## 2. 控件组成

```
WBP_InventoryPanel                 （已替换）
├─ EquipmentPanel  → WBP_EquipmentPanel
├─ BackpackPanel   → WBP_BackpackPanel
└─ Panel_Detail
   └─ ItemDetail   → WBP_ItemDetail

WBP_LootPanel
├─ EquipmentPanel  → WBP_EquipmentPanel
├─ BackpackPanel   → WBP_BackpackPanel
└─ LootListPanel   → WBP_LootListPanel
   └─ ScrollBox_LootList
      └─ WBP_LootRow ×N
```

三栏宽度：左 `0.25` / 中 `0.45` / 右 `0.30`，栏间距 `11px`。中栏 `45%` 是为了让 `4×128 + 间距` 的背包网格完整放下。

---

## 3. C++ 拆分建议

| WBP | 建议类 | 职责 | 主要节点 |
|---|---|---|---|
| `WBP_EquipmentPanel` | `UDZ_EquipmentPanel`（可选） | 8 个装备槽的显示/刷新 | `EquipSlot_*` |
| `WBP_BackpackPanel` | `UDZ_BackpackPanel` | 20 格背包、负重文本/条、格子事件上报 | `Grid_Backpack`、`Text_WeightValue`、`WeightBar` |
| `WBP_LootRow` | `UDZ_LootRow : UDZ_UserWidget` | 行数据、悬停、拖影、「拿取」按钮 | `Img_ItemIcon`、`Text_ItemName`、`Text_Quantity`、`Text_ItemWeight`、`Frame_RowBG`、`Btn_Take` |
| `WBP_LootListPanel` | `UDZ_LootListPanel` | 容器名/件数/关闭、行动态创建、空容器提示、行拖拽与拿取上报 | `Text_ContainerName`、`Text_LootCount`、`Btn_Close`、`ScrollBox_LootList`、`Text_EmptyHint` |
| `WBP_LootPanel` | `UDZ_LootPanel` | 组合三栏、容器数据、移入背包（落格 / 自动选格）、刷新 | `EquipmentPanel`、`BackpackPanel`、`LootListPanel` |
| `WBP_InventoryPanel` | `UDZ_InventoryPanel`（瘦身） | 只组合装备/背包/详情，处理选中联动 | `EquipmentPanel`、`BackpackPanel`、`ItemDetail` |
| `WBP_ItemSlot` | `UDZ_ItemSlot`（现有） | 共用格子；需要补数量显示与拖放悬停 | `Img_ItemIcon`、`Border_Selected`、`Text_Quantity` |
| `WBP_WeightBar` | `UDZ_WeightBar`（可选） | 封装 `SetPercent` | `Bar_Weight` |

建议让每个子面板自己向持有方发事件，而不是让 `UDZ_InventoryPanel` / `UDZ_LootPanel` 直接遍历子面板内部节点：

- `UDZ_BackpackPanel`：格子选中 / 拖拽开始 / 拖放结束。
- `UDZ_LootListPanel`：行拖拽开始 / 行拖拽结束 / 行「拿取」（整叠或 1 件）/ 关闭点击。
- `UDZ_LootPanel`：只负责把「战利品行 → 背包」的落点或自动选格、数量合并、行移除/减数量、负重刷新串起来。

---

## 4. 移入背包数据流（拖拽 / 按钮共用）

1. 入口有两条：行拖拽、行内 `Btn_Take` 单击（按住 Shift 只取 1 件）。
2. 两条入口都收敛到**同一个处理**：`把这一行移入背包(行, 目标格可空, 数量)`，不要各写一套。
3. 目标格由拖拽落点给出；按钮路径传空 → 先并入同物品格，再找第一个空格。
4. 数量：拖拽默认整叠；按钮默认整叠、Shift 为 1；落到同物品格则数量相加。
5. 取不走（背包满、超重）→ 本期不做反馈，不改数据，行保留。
6. 成功之后：
   - 整叠移入 → 从 `ScrollBox_LootList` 移除该行；
   - 取 1 件 → `Text_Quantity` 减 1，减到 0 再移除行；
   - 刷新 `Text_LootCount` 与 `Text_EmptyHint`；
   - 刷新 `Text_WeightValue` 与 `WeightBar` 比例。

> 现有的 `UDZ_UserWidget::NativeOnDrop` 是在**拖拽源**上广播 `OnWidgetDragEnded(Source, EndWidget)`，不是在落点格子上广播。绑定事件时注意这一点。

---

## 5. 数据字段现状

`FDZ_ItemData` 目前只有：

| 字段 | 用途 |
|---|---|
| `ItemId` | 物品 ID |
| `ItemName` | 名称 |
| `Weight` | 单件重量 |
| `Icon` | 图标 |

没有类型、描述、价值、堆叠上限。当前 UI 只显示 **图标 / 名称 / 数量 / 重量**；如果后续要加「类型/描述/价值」，需要先扩数据结构，再在 `WBP_LootRow` / `WBP_ItemDetail` 加节点。

---

## 6. 阶段 B：`WBP_InventoryPanel` 已替换

`WBP_InventoryPanel` 现在已经在 `HB_Regions` 里直接持有：

| 节点 | 资产 | `HorizontalBoxSlot` |
|---|---|---|
| `EquipmentPanel` | `WBP_EquipmentPanel` | `Size = 0.28`、右内边距 `11` |
| `BackpackPanel` | `WBP_BackpackPanel` | `Size = 0.45`、右内边距 `11` |
| `Panel_Detail` / `ItemDetail` | Border / `WBP_ItemDetail` | `Size = 0.27` |

`WBP_InventoryPanel.json` 和 `Docs/UI/WBP_InventoryPanel.md` 已同步；`Grid_Backpack`、`Text_WeightValue`、20 个 `ItemSlot_*` 归 `WBP_BackpackPanel`，8 个 `EquipSlot_*` 归 `WBP_EquipmentPanel`。

C++ 侧还需要你完成：

1. `UDZ_InventoryPanel` 增加 `EquipmentPanel` / `BackpackPanel` 的 `BindWidget`（类型分别是你的 `UDZ_EquipmentPanel` / `UDZ_BackpackPanel`，或先按 `UUserWidget` 接收）。
2. 把原来 `NativeConstruct` 里遍历 `Grid_Backpack` 的逻辑迁到 `UDZ_BackpackPanel`，由它向 `UDZ_InventoryPanel` 上报格子点击/拖放。
3. `UDZ_InventoryPanel` 只保留「选中哪一格 → 刷详情」的联动。

> 共用栏位现在是**单层 15px 内边距**（根 Border 的 Padding）。原 `WBP_InventoryPanel` 是「Border 15 + BorderSlot 15」的双层内边距，替换后内容比原来向外扩了 15px。若要保持原视觉，可在 `EquipmentPanel` / `BackpackPanel` 外面再包一层 15px 的 Border，或直接把栏位比例微调。
