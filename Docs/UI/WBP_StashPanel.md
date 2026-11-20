# WBP_StashPanel 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Stash/WBP_StashPanel` |
| 父类 | `UDZ_StashPanel`（资产已生成，需要在编辑器里 Reparent 一次） |
| 源文件 | `UIWidgets/UI/Stash/WBP_StashPanel.json` |
| 设计模式 | `FillScreen`；两块卡片定宽定高、垂直居中对齐屏幕中线 |
| 布局 | 左卡片 760×940（仓库）／右卡片 420×940（装备 + 随身背包 + 出发） |
| 中间留白 | 左右卡片之间不铺底板，**3D 场景从这个区域透出来** |

> 命名一律是对象名（`GetName()` / JSON 的 `name` 字段）。

---

## 布局与锚点（改布局前先看这里）

| 节点 | 锚点 | 偏移 | 含义 |
|---|---|---|---|
| `Panel_Stash` | `min=max=(0, 0.5)` | `left=48, top=-470, right=760, bottom=940` | 距屏幕左边缘 48，宽 760，垂直居中 |
| `Panel_Loadout` | `min=max=(1, 0.5)` | `left=-468, top=-470, right=420, bottom=940` | 距屏幕右边缘 48，宽 420，垂直居中 |
| `Text_EscHint` | `min=max=(0, 1)` | `left=48, top=-26, right=240, bottom=20` | 左下角，距底边 6px |

- 锚点重合时 `right` / `bottom` 表示**宽和高**，不是右下边界坐标。
- 两块卡片高度固定 940，需要屏幕高度 ≥ 1000 才不裁切（1080p 及以上的目标分辨率满足）。
- 中间区域不铺任何底板。**不要**在 `RootCanvas` 上加全屏半透明底，会把场景压暗。

### 左卡片竖向结构

```
Panel_Stash (760×940, Padding 15)
└─ VB_Stash
   ├─ HB_StashHeader   仓库 | Spacer | 商 店 | 0 / 30
   ├─ Size_Divider_Stash
   ├─ Grid_Stash       30 格，5 列 × 6 行，水平居中
   ├─ Spacer_StashFill 把底栏推到底
   ├─ Panel_StashFooter  总价值 $N | Spacer | 拖拽整理
   └─ HB_SellBar      未选中物品 | 售价 $0(隐藏) | [出 售]
```

`HB_SellBar` 是这次新增的，高度约 34（按钮 6+6 内边距 + 文字）。卡片原本有约 100 的竖向余量，加这一行不需要缩格子。

---

## 需要程序接的节点（建议 BindWidget 全集）

| 节点 | 类型 | 程序用它做什么 |
|---|---|---|
| `Grid_Stash` | UniformGridPanel | 30 个仓库格的容器；按 `ItemSlot_` 前缀收集 |
| `Text_StashCount` | TextBlock | 仓库占用文本 `已用 / 30` |
| `Text_TotalValue` | TextBlock | 仓库总价值 `$0` |
| `Grid_LoadoutBag` | UniformGridPanel | 6 个随身背包格的容器；按 `BagSlot_` 前缀收集 |
| `Text_BagCount` | TextBlock | 背包占用文本 `已放 / 6` |
| `EquipSlot_Primary` | `WBP_EquipmentSlot` | 主武器槽 |
| `EquipSlot_Melee` | `WBP_EquipmentSlot` | 近战武器槽 |
| `EquipSlot_Armor` | `WBP_EquipmentSlot` | 护甲槽 |
| `EquipSlot_Backpack` | `WBP_EquipmentSlot` | 背包槽 |
| `Btn_Start` | Button | 「出发」 |
| `Btn_Shop` | Button | 「商 店」入口，点开局外商店面板 |
| `Text_SellInfo` | TextBlock | 出售栏：选中物品名称与数量（`医疗包 ×3`） |
| `Text_SellPrice` | TextBlock | 出售栏：单件售价（`售价 $48`），无选中时 `Collapsed` |
| `Btn_Sell` | Button | 「出 售」，点一次卖 1 件；无选中或不可卖时禁用 |

> 这 14 个就是 C++ 里**可以**声明的 BindWidget 全集。**不要**额外声明别的 BindWidget —— 插件导入时找不到控件会让整次导入失败。

## 不需要接的节点

| 节点 | 类型 | 说明 |
|---|---|---|
| `RootCanvas` | CanvasPanel | 根 |
| `Panel_Stash` / `Panel_Loadout` | Border | 两张卡片底板 + 15px 内边距，纯装饰 |
| `VB_Stash` / `VB_Loadout` | VerticalBox | 卡片内纵向布局 |
| `HB_StashHeader` / `HB_StashFooter` / `HB_BagHeader` | HorizontalBox | 标题行 / 底栏 / 背包标题行布局 |
| `Text_StashTitle` / `Text_EquipTitle` / `Text_BagTitle` | TextBlock | 固定标题「仓库」「装备」「随身背包」 |
| `Text_Label_Value` / `Text_Hint_Stash` / `Text_Hint_Start` / `Text_EscHint` | TextBlock | 固定文案 |
| `Size_Divider_*` / `Divider_*` | SizeBox / Border | 分隔线 |
| `Spacer_*` | Spacer | 把标题右侧文本、底栏、出发按钮推到正确位置 |
| `Row_Equip_1` / `Row_Equip_2` / `Cell_*` | HorizontalBox / VerticalBox | 装备槽两行两列布局 |
| `Text_Label_Primary` / `_Melee` / `_Armor` / `_Backpack` | TextBlock | 槽位名固定文案 |
| `Text_Start` | TextBlock | 按钮上的「出 发」 |

---

## 交互

| 来源 | 事件 | 程序要做什么 |
|---|---|---|
| `ItemSlot_1` … `ItemSlot_30` | 左键单击 | 记下当前选中的仓库格，全体 `SetSelected(false)` 后给这一格 `SetSelected(true)`；抛 `OnSellTargetChanged(Handle)` 给控制器算价。点空格子 = 清空选中（抛无效 Handle） |
| `ItemSlot_*` | 拖拽结束 | 把 `From` / `To` 两个 `FDZ_ItemSlotHandle` 交给移动入口，合法性由数据层判定 |
| `BagSlot_1` … `BagSlot_6` | 左键单击 / 拖拽结束 | 同上，只是容器类型不同（背包格**不可出售**） |
| `EquipSlot_*` | 拖入 / 拖出 | 同 `WBP_EquipmentPanel` 的处理方式，聚合后统一发移动请求 |
| `Btn_Start` | `OnClicked` | 1) 校验能否出发（至少一件装备可选）；2) 把出发配置提交给出发流程；3) 提交成功后不许重复点击 |
| `Btn_Shop` | `OnClicked` | 抛 `OnShopRequested`，由 `UDZ_StashController` 转给 `UDZ_ShopController::Open()`。商店打开期间本面板的拖拽要禁用 |
| `Btn_Sell` | `OnClicked` | 把当前选中的 `FDZ_ItemSlotHandle` 抛给控制器，控制器调 `GameInstance::TrySellStashSlot(Handle, 1)`，**一次只卖 1 件** |
| 面板空白处 | 左键单击 | 取消选中（可选，不接也不影响） |
| `ESC` | 按键 | 隐藏面板（按键不在 UI 节点上，由 Controller 处理） |

**拖拽的容器类型**

| 区域 | 容器枚举 | 槽位下标 |
|---|---|---|
| 仓库格子 | `EItemContainer::Stash` | 0 ~ 29 |
| 随身背包格子 | `EItemContainer::LoadoutBag` | 0 ~ 5 |
| 四个装备槽 | `EItemContainer::LoadoutEquipment` | `EEquipSlot` 数值 |

> 为什么装备槽不复用 `EItemContainer::Equipment`：局内角色也有一套 `Equipment`，两者数据源不同。共用一个枚举值就得靠「当前在大厅还是局内」来分流，容易在跨阶段刷新时读错数组。

---

## 状态

| 状态 | 哪个节点变 | 变成什么 |
|---|---|---|
| 仓库格子有物品 / 为空 | `ItemSlot_*` | 见 [WBP_ItemSlot.md](WBP_ItemSlot.md) |
| 背包格子有物品 / 为空 | `BagSlot_*` | 同上 |
| 装备槽已装备 / 未装备 | `EquipSlot_*` | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| 仓库占用变化 | `Text_StashCount` | `已用 / 30` |
| 仓库总价值变化 | `Text_TotalValue` | `$总价值` |
| 背包占用变化 | `Text_BagCount` | `已放 / 6` |
| 是否可出发 | `Btn_Start` | 当前恒为可用。将来要加条件，在 `UDZ_StashPanel` 加一个 setter 调 `UButton::SetIsEnabled` |
| 未选中 / 选中空格 | `Text_SellInfo` / `Text_SellPrice` / `Btn_Sell` | 文案回「未选中物品」、价格 `Collapsed`、按钮禁用 |
| 选中可卖物品 | `Text_SellInfo` / `Text_SellPrice` / `Btn_Sell` | 「医疗包 ×3」+「售价 $40」+ 按钮可用；`Value` 为 0 时显示「售价 $0」也照样可点 |
| 出售扣减数量 | `Text_SellInfo` | `×3 → ×2`；卖到 0 时整行清空选中 |
| 面板显示 / 隐藏 | 面板整体 | 由外部加到视口 / 移除，或用 `SetVisibility` |

> 刷新后选中高亮要保留，不能因为重新铺格子而丢失。

---

## 含有的子控件

| 节点 | 资产 | 程序要做什么 | 交互文档 |
|---|---|---|---|
| `ItemSlot_1` … `ItemSlot_30` | `WBP_ItemSlot` | 逐个设置物品 / 数量 / 选中，接收点击与拖放 | 见 [WBP_ItemSlot.md](WBP_ItemSlot.md) |
| `BagSlot_1` … `BagSlot_6` | `WBP_ItemSlot` | 同上 | 见 [WBP_ItemSlot.md](WBP_ItemSlot.md) |
| `EquipSlot_*` | `WBP_EquipmentSlot` | 设置图标 / 名称，转发拖放 | 见 [WBP_EquipmentSlot.md](WBP_EquipmentSlot.md) |
| `Btn_Start` | `Button` | 只有一个 `OnClicked` | — |
| `Btn_Shop` | `Button` | 只有一个 `OnClicked` | — |
| `Btn_Sell` | `Button` | 只有一个 `OnClicked` | — |

---

## 与局内背包面板的差异

| | `WBP_StashPanel` | `WBP_InventoryPanel` |
|---|---|---|
| 用途 | 局外整理 + 出发 | 局内查看身上物品 |
| 底板 | 两张卡片，中间透出 3D 场景 | 一整块 1408×858 居中面板 |
| 仓库区 | 30 格，无重量限制 | — |
| 背包区 | 6 格随身背包（出发前选） | 角色身上的 5 格背包 |
| 属性区 | 无 | 有 `CharacterStats` |
| 主按钮 | 出发 | 无 |
