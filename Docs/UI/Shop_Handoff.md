# 商店与出售 交接总览

| 项 | 值 |
|---|---|
| 范围 | 局外经济：商店买（无限供应）+ 仓库卖（一次 1 件） |
| 状态 | 界面 ✅ 完成；C++ ⬜ 未开始 |
| 涉及界面 | `WBP_ShopPanel`（重做）、`WBP_ShopEntry`（重做）、`WBP_StashPanel`（加商店入口 + 出售栏） |
| 相关文档 | [WBP_ShopPanel.md](WBP_ShopPanel.md)、[WBP_ShopEntry.md](WBP_ShopEntry.md)、[WBP_StashPanel.md](WBP_StashPanel.md)、[Stash_Handoff.md](Stash_Handoff.md) |
| 前置依赖 | `DT_ShopItems` 表、`UDZ_GameInstance` 货币/买/卖接口、`UDZ_ShopController` |

---

## 1. 目标与不做

**目标**

- 仓库标题行有「商 店」按钮，点开 1040×720 居中模态商店面板。
- 商店**无限供应**：只有余额与仓库容量能挡住购买，没有库存概念。
- 仓库底栏有出售栏：点选一个仓库格 → 显示名称、数量、单件售价 → 点「出 售」卖 **1 件**。
- 买/卖都实时更新余额，仓库面板与商店面板互相自动刷新。

**不做**

- 批量出售（数量选择）、卖出时改价
- 商品分类页签、分页
- 库存与余额的跨会话持久化（等 M8 SaveGame）
- 服务端校验（局外本地单机）
- 随身背包 / 装备槽里的物品出售（只卖仓库 30 格）

---

## 2. 形态与入口

```
Lobby.umap
 └ PC = BP_LobbyPlayerController（ADZ_LobbyPlayerController）
    └ UDZ_LobbyUIManager
       ├ UDZ_StashController → WBP_StashPanel（常驻）
       └ UDZ_ShopController  → WBP_ShopPanel（按需，叠在仓库之上）

【买】仓库标题行「商 店」
  → UDZ_StashPanel::OnShopRequested
     → UDZ_StashController 转发
        → UDZ_ShopController::Open()
           → UIManager::OpenWidget(ShopPanel)

【卖】点仓库格子 → 底栏显示售价
  → UDZ_StashPanel::OnSellTargetChanged(Handle)      // 选中格或格内数量变化都会发
     → UDZ_StashController 算价并回填 SetSellInfo
        → 点「出 售」
           → UDZ_StashPanel::OnSellRequested(Handle)   // 命令带自己的参数
              → GameInstance::TrySellStashSlot(Handle, 1)
                 → 广播 OnStashChanged + OnMoneyChanged
```

**为什么商店必须走 `UIManager`**：原来通过 `UDZ_ShopSubsystem` 独立开关，`Close()` 里硬写 `FGameOnly` + 隐藏鼠标，在局外关一次仓库就点不动了。改成 `UDZ_UserWidget` 由 UIManager 管理后，输入模式按「还有没有面板开着」自动算。

> 局外 PC 没有 Enhanced Input 绑定，所以没有 ESC。商店只能靠 `Btn_Close` 关闭。

---

## 3. 资产清单

| 资产 | 路径 | 状态 |
|---|---|---|
| `WBP_ShopPanel` | `/Game/UI/Shop/WBP_ShopPanel` | ✅ 1040×720 重做完成 |
| `WBP_ShopEntry` | `/Game/UI/Shop/WBP_ShopEntry` | ✅ 重做完成（库存块已 Collapsed） |
| `WBP_StashPanel` | `/Game/UI/Stash/WBP_StashPanel` | ✅ 加了 `Btn_Shop` 与 `HB_SellBar`（92 节点） |
| `DT_ShopItems` | `/Game/Data/Shop/DT_ShopItems` | ⬜ 待建（第 4 节） |
| 图标 | `/Game/Assets/ItemIcons/Runtime/*` | ✅ 复用 6 张，近战武器缺图标 |

---

## 4. 数据契约

### 4.1 商品表 `FDZ_ShopItemData`（`FTableRowBase`）

| 字段 | 类型 | 说明 |
|---|---|---|
| `ItemId` | `int32` | 必须能在 `DT_ItemData` 里查到，查不到的行 Init 时打 Warning 并跳过 |
| `Price` | `int32` | 买入价，独立于 `FDZ_ItemData::Value` |
| `bEnabled` | `bool` | 是否上架，临时下架不用删行 |

**没有 `Stock`。** 商店无限供应。

### 4.2 余额

| 项 | 定义 |
|---|---|
| 存放 | `UDZ_GameInstance::Money`（`int32`），不持久化 |
| 初始值 | `EditDefaultsOnly` 调试字段，例如 5000 |
| 收入 | 只有出售（本版没有结算发钱） |
| 通知 | `OnMoneyChanged` |

### 4.3 卖价

```
卖价 = FDZ_ItemData::Value（负值按 0 处理）
```

- 卖价与商店表**无关**，不查 `DT_ShopItems`，没有任何倍率。
- `Value = 0` 的物品**可以卖**，卖 0 元——留给"想清掉这件东西"的玩家。
- 买卖两个价格来源互不约束，所以**配表时必须保证 `Price ≥ Value`**，否则可以「买入 → 立刻卖掉」套利。

### 4.4 购买校验（顺序即优先级）

| 顺序 | 校验 | 失败文案 |
|---|---|---|
| 1 | 是否上架 | 「该商品已下架」 |
| 2 | 余额是否够 | 「余额不足」 |
| 3 | 仓库能否放下 | 「仓库空间不足」 |
| 4 | 扣钱 → 加物品 → 广播 | — |

> 仓库容量校验必须排在扣钱之前，否则会出现扣了钱没拿到东西。

### 4.5 出售校验

| 顺序 | 校验 | 说明 |
|---|---|---|
| 1 | 槽位属于 `EItemContainer::Stash` 且非空 | 防止卖背包 / 装备槽 |
| 2 | 数量合法（1 ≤ Count ≤ 该格数量） | 本版一次卖 1 件 |
| 3 | 移除 1 件 → 加钱 → 广播 | 数量减到 0 时把 `FDZ_ItemStack` 置为默认值，不能整格 `RemoveAt` |

> 价格不参与校验：0 元的物品也能卖。

---

## 5. 程序要实现什么

### 5.1 数据层

| 位置 | 内容 |
|---|---|
| `Data/Shop/FDZ_ShopItemData.h` | 新 RowStruct（3 字段） |
| `Content/Data/Shop/DT_ShopItems` | 新表 |
| `UDZ_GameInstance` | `DT_ShopItems` + Init 里建 `TMap<int32, FDZ_ShopItemData>` |
| | `int32 Money` / `GetMoney()` / `AddMoney(int32)`（私有）/ `OnMoneyChanged` |
| | `const TArray<FDZ_ShopItemData>& GetShopRows() const` |
| | `int32 GetSellPrice(int32 ItemId) const` |
| | `bool CanBuyShopItem(int32 ItemId, FText&) const` / `bool TryBuyShopItem(int32 ItemId, int32 Count, FText&)` |
| | `bool CanSellStashSlot(const FDZ_ItemSlotHandle&, int32 Count, FText&) const` / `bool TrySellStashSlot(const FDZ_ItemSlotHandle&, int32 Count, FText&)` |

`TryBuyShopItem` 内部调用现有 private `TryAddItemToStash`，**不要**把它开成公开接口。

### 5.2 UI 层

| 类 | 改什么 |
|---|---|
| `UDZ_ListEntryWidget` | **新建**（抽象基类）：`NativeOnListItemObjectSet` 把 item 转交给 `OnEntryRefresh`；条目不缓存 item |
| `UDZ_ListView` | **新建**：`UDZ_ListView : UListView`，`RefreshDisplayedEntries()` 遍历可见条目，用 `GetListObjectFromEntry` 取 item 后调 `OnEntryRefresh` |
| `UDZ_ShopItemObject` | 增 `ItemId`、`Icon`、`TypeText`、`bCanBuy` |
| `UDZ_ShopEntryWidget` | 父类改 `UDZ_ListEntryWidget`；加 `Img_Icon`、`Text_Type`、`Btn_Buy`；**删** `Text_Stock`；实现 `OnEntryRefresh`；点购买用 `GetListItem()` 取 item |
| `UDZ_ShopPanel` | 父类改 `UDZ_UserWidget`；`List_Items` 类型改 `UDZ_ListView`；加 `Text_Money`、`Btn_Close`、`RefreshEntries`；**删** `Text_Stats` 与四个压测按钮、`NativeTick`、`GenerateItems`、`RefreshStats` |
| `UDZ_ShopController` | 新建：`Open/Close/Toggle/Refresh/BuildEntries`，订阅 `OnMoneyChanged`/`OnStashChanged`；**`Initialize` 里 `BuildEntries` 一次**，`Refresh` 只改 `bCanBuy` + `RefreshEntries` |
| `UDZ_UIConfig` | 加 `ShopWidgetClass` |
| `UDZ_LobbyUIManager` | 加 `ShopController` |
| `UDZ_StashPanel` | 加 `Btn_Shop` + `OnShopRequested`；加 `Text_SellInfo`/`Text_SellPrice`/`Btn_Sell` + 选中状态 + `OnSellTargetChanged`/`OnSellRequested`（都带 `FDZ_ItemSlotHandle`） |
| `UDZ_StashController` | 转发商店请求；接 `OnSellTargetChanged(Handle)` → 算价 → `SetSellInfo`；接 `OnSellRequested(Handle)` → `TrySellStashSlot` |
| `UDZ_ShopSubsystem` | **已删除**（`DZ_ShopSubsystem.h/.cpp`） |

### 5.3 执行顺序

| 步 | 状态 |
|---|---|
| 1. `FDZ_ShopItemData` + GameInstance 货币/买/卖 | ✅ 代码已写 |
| 2. `UDZ_ShopItemObject` / `UDZ_ShopEntryWidget` / `UDZ_ShopPanel` | ✅ |
| 3. `UDZ_ShopController` + UIConfig + LobbyUIManager | ✅ |
| 4. `UDZ_StashPanel` / `UDZ_StashController` | ✅ |
| 5. 编译 | ✅ `DecayZoneEditor Win64 Development` 通过 |
| 6. 在编辑器里填 `DT_ShopItems` 与 `UIConfig::ShopWidgetClass` | ⬜ 需要你在编辑器里做 |
| 7. 清理 JSON 里废弃的 `Size_Stock` 与 `VB_Debug` | ⬜ 编辑器换成新二进制后处理 |

> 没配 `DT_ShopItems` 时商店就是**空列表**，`InitShopItems()` 打一条 Error 日志。不做任何兜底——价格与上架范围只能来自表。

---

## 6. 界面侧已完成 / 待清理

| 项 | 状态 |
|---|---|
| 商店面板 1408×858 → 1040×720 居中模态 | ✅ |
| 标题行：商店 / 余额 / `✕` | ✅ |
| 条目：图标 + 名称/类型 + 单价 + 购买 | ✅ |
| 条目库存块 `Size_Stock` 收起（`Collapsed`） | ✅ |
| 仓库标题行加「商 店」 | ✅ |
| 仓库底栏加出售栏（信息 / 售价 / 按钮） | ✅ |
| `VB_Debug`（5 个调试节点）收起 | ✅ 待 C++ 删属性后整块移除 |
| `Size_Stock`（库存节点） | ✅ 待 C++ 删 `Text_Stock` 后移除 |

---

## 7. 验收

界面层（现在就能看）：

| 场景 | 期望 |
|---|---|
| 打开 `WBP_ShopEntry` | 72 高：图标位 / 名称+类型 / 价格 / 购买，**没有库存文本** |
| 打开 `WBP_ShopPanel` | 1040×720 居中，标题行余额与 ✕，8 条占位行，调试块不可见 |
| 打开 `WBP_StashPanel` | 标题行「商 店」；底部多一行「未选中物品 … 出 售」，出售栏不被裁切 |

C++ 接上后：

| 场景 | 期望 |
|---|---|
| 大厅点「商 店」 | 面板弹出，商品为真实数据，余额正确 |
| 买一件 | 余额减少、仓库多一件、商店与仓库都刷新 |
| 同一件连买 10 次 | 只要有仓库位就都成功（无售罄） |
| 余额 0 再买 | 按钮禁用；强行点击提示「余额不足」 |
| 仓库塞满再买 | 提示「仓库空间不足」，余额与仓库都不变 |
| 点空格子 | 出售栏回「未选中物品」，按钮禁用 |
| 选中医疗包 ×3 | 「医疗包 ×3」「售价 $40」，按钮可用 |
| 点出售 3 次 | 3→2→1→整行消失，余额每次 +40 |
| `Value` 为 0 的物品 | 显示「售价 $0」，按钮仍可点，卖出后物品消失、余额不变 |
| 定价低于价值 | 能买进再卖掉赚钱——这是配表问题，代码不拦，`Price` 必须 ≥ `Value` |
| 关闭商店 | 面板消失，仓库拖拽与鼠标正常 |

---

## 8. 已知限制

| 项 | 说明 |
|---|---|
| 余额不持久化 | 重开游戏回初始值，等 M8 SaveGame |
| 库存不持久化 | 不存在库存，所以无此问题 |
| 无服务端校验 | 局外本地运行，客户端能改档 |
| 一次只卖 1 件 | 弹药堆叠要连点，后续可加数量选择 |
| 近战武器缺图标 | 该行走无图标分支 |
| 无「为什么按钮是灰的」提示 | 目前靠点击后的 Tips |

---

## 9. 待确认

| # | 问题 | 当前默认 |
|---|---|---|
| 1 | 初始余额 | 5000（`EditDefaultsOnly`） |
| 2 | `DT_ShopItems` 填哪些行、什么价 | 待给规则，例如「每件物品一行，价格 = `Value × 2` 取整到 10」 |
