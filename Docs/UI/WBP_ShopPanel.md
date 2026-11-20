# WBP_ShopPanel 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Shop/WBP_ShopPanel` |
| 父类 | `UDZ_ShopPanel`（**待从 `UUserWidget` 改为 `UDZ_UserWidget`**，见交接文档） |
| 源文件 | `UIWidgets/UI/Shop/WBP_ShopPanel.json` |
| C++ | `UI/Widgets/Shop/DZ_ShopPanel.h/.cpp` |
| 尺寸 | **1040 × 720**，屏幕居中；列表区高 592 |
| 形态 | 局外模态面板，叠在 `WBP_StashPanel` 之上 |
| 入口 | 由 `UDZ_ShopController` 调用 `UIManager::OpenWidget` 打开（不再是控制台命令） |

---

## 控件树

```
RootCanvas (CanvasPanel)
└─ Size_Root (SizeBox 1040×720, 居中锚点)
   └─ Border_Outline (#07090C)
      └─ BG_Panel (Border, Padding 1)
         └─ VB_Main (VerticalBox, Padding 20/16)
            ├─ Panel_Top (Border, #050607)
            │  └─ VB_Top (VerticalBox, Padding 16/12)
            │     └─ HB_Title (HorizontalBox)
            │        ├─ Text_Title   (TextBlock 22, 金 #D6AC4B,「商店」)
            │        ├─ Spacer_Title (Spacer, Fill)
            │        ├─ Text_Money   (TextBlock 18, #C6D4DA,「$5,000」)
            │        └─ Size_Close   (SizeBox 28×28)
            │           └─ Btn_Close (Button) └─ Text_Close (「✕」)
            ├─ Size_List (SizeBox, 高 592)
            │  └─ List_Items (ListView, EntryWidgetClass = WBP_ShopEntry_C)
            ├─ HB_Footer (HorizontalBox)
            │  ├─ Text_Hint       (「点击「购买」后物品直接进仓库，仓库满时无法购买」)
            │  ├─ Spacer_Footer   (Fill)
            │  └─ Text_Hint_Close (「右上角 ✕ 关闭」)
            └─ VB_Debug (VerticalBox, **Collapsed**)
               ├─ Text_Stats (TextBlock)
               └─ HB_Buttons (HorizontalBox)
                  ├─ Btn_Gen20 / Btn_Gen200 / Btn_Gen2000 / Btn_Clear
```

> `VB_Debug` 整块 `Collapsed`，界面上看不见。里面这 5 个节点目前**不能从 JSON 删**：父类 `UDZ_ShopPanel` 还声明着对应的 `BindWidget`，删掉会让 WidgetJsonBridge 的导入校验失败（`WidgetJsonSerializer.cpp` 提交前会检查父类链上所有 BindWidget 必须在树里）。等 C++ 删掉这 5 个属性后，把 `VB_Debug` 整个移除。

---

## 需要程序接的节点

| 节点 | 类型 | BindWidget 属性 | 状态 | 程序用它做什么 |
|---|---|---|---|---|
| `List_Items` | `UDZ_ListView` | `List_Items` | ✅ | 商品列表；`RefreshDisplayedEntries()` 只重刷可见条目，不重建 |
| `Text_Money` | TextBlock | `Text_Money` | ✅ | 显示余额 `$N` |
| `Btn_Close` | Button | `Btn_Close` | ✅ | 关闭面板 |
| `Text_Stats` | TextBlock | — | 已移出 C++ | 调试残留，随 `VB_Debug` 一起废弃 |
| `Btn_Gen20` / `Btn_Gen200` / `Btn_Gen2000` / `Btn_Clear` | Button | — | 已移出 C++ | 压测残留，随 `VB_Debug` 一起废弃 |

`Text_Title`、`Text_Hint`、`Text_Hint_Close`、`Text_Close`、`Spacer_*`、`Panel_Top`、`BG_Panel`、`Border_Outline`、`VB_Main`、`VB_Top`、`HB_Title`、`Size_List`、`HB_Footer`、`VB_Debug`、`HB_Buttons` 不需要绑定。

---

## 事件

| 触发 | 处理函数 | 行为 |
|---|---|---|
| `Btn_Close` 点击 | `HandleCloseClicked` | 通知 `UDZ_ShopController::Close()`，由 UIManager 移除并重算输入模式 |
| 条目点击「购买」 | `RequestBuy(UDZ_ShopItemObject*)` → `OnBuyRequested(ItemId)` | 转给 `UDZ_ShopController` → `GameInstance::TryBuyShopItem` |
| 打开面板 | `NativeOnActivated`（或 Controller 显式调 Refresh） | 重建商品列表 + 刷新余额 |
| `OnMoneyChanged` | `HandleMoneyChanged` | 只刷 `Text_Money`，不重建列表 |
| `OnStashChanged` | `HandleStashChanged` | 仓库容量变化会影响「能否购买」，刷新条目可用状态 |

`NativeTick` 里原来的统计刷新随 `Text_Stats` 一起删掉，不要保留空 Tick。

---

## 面板内数据流

```
【控制器创建时，只有一次】
UDZ_ShopController::Initialize()
  → CreateWidget(WBP_ShopPanel)
  → BuildEntries()
      → GameInstance::GetShopItems() / GetMoney()
      → 创建 UDZ_ShopItemObject（每件一次）
      → List_Items->SetListItems(...)      ← 一辈子只调这一次

【之后每次买/卖、每次打开面板】
ShopController.Refresh()
  → 遍历 Entries，更新 bCanBuy（CanBuyShopItem）
  → Text_Money->SetText(...)
  → List_Items->RefreshDisplayedEntries()  ← 只重刷可见的 8 行
        └ 逐个 Entry->OnEntryRefresh(item)

Entry.Btn_Buy 点击
  → GetListItem<UDZ_ShopItemObject>()      ← 向列表要，条目不缓存
  → Panel::RequestBuy(Item) → OnBuyRequested(ItemId)
  → ShopController → GameInstance::TryBuyShopItem(ItemId, 1, OutFailReason)
      ├ 失败 → 弹 Tips(OutFailReason)，列表不动
      └ 成功 → 广播 OnStashChanged + OnMoneyChanged
                 ├ ShopController::Refresh()（两次，都是轻量刷新）
                 └ StashController 已有 OnStashChanged 监听 → 仓库面板自动刷新
```

> `SetListItems` / `RegenerateAllEntries` / `RequestRefresh` 在刷新路径里都不再使用：条目集合不变，只有 `bCanBuy` 变，所以直接推给可见条目。

---

## 布局约束（改布局前先看）

| 项 | 约束 |
|---|---|
| 面板尺寸 | 1040 × 720 定死，居中锚点 `min=max=(0.5,0.5)`，偏移 `-520 / -360 / 1040 / 720` |
| 列表高度 | `Size_List.HeightOverride = 592`，按行高 72 能放 8 行 |
| 行高联动 | 改 `WBP_ShopEntry` 的行高必须同步改 `Size_List`，反之亦然 |
| 叠加关系 | 面板在仓库面板（`WBP_StashPanel`）之上，仓库卡片不会被压暗（商店面板不开全屏底） |
| 拖拽 | 商店打开期间仓库的拖拽要禁用，避免拖影穿透到商店面板 |

---

## 当前阶段

| 项 | 状态 |
|---|---|
| 界面（尺寸 / 图标 / 购买按钮 / 余额 / 关闭） | ✅ JSON 完成 |
| 局外入口按钮 `Btn_Shop` | ✅ 已加进 `WBP_StashPanel` |
| C++：面板父类改 `UDZ_UserWidget` | ✅ 已完成 |
| C++：`Text_Money` / `Btn_Close` 两个 BindWidget | ✅ 已完成 |
| C++：`UDZ_ShopController` + `UIConfig::ShopWidgetClass` | ✅ 已完成 |
| C++：删除 5 个调试 BindWidget | ✅ 已完成，待清 JSON 里的 `VB_Debug` |
