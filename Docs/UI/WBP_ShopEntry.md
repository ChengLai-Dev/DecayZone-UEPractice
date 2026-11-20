# WBP_ShopEntry 交互文档

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/Shop/WBP_ShopEntry` |
| 父类 | `UDZ_ShopEntryWidget` |
| 源文件 | `UIWidgets/UI/Shop/WBP_ShopEntry.json` |
| C++ | `UI/Widgets/Shop/DZ_ShopEntryWidget.h/.cpp` |
| 行高 | **72**（`Root_Size.HeightOverride`） |
| 用途 | 商店列表的一行商品：图标 + 名称/类型 + 单价 + 购买按钮 |

> 商店是**无限供应**的，条目上不显示库存。

---

## 控件树

```
Root_Size (SizeBox, 高 72)
└─ BG_Row (Border, #040607)
   └─ HB_Row (HorizontalBox, Padding 10/6/12/6, VAlign_Center)
      ├─ Size_Icon  (SizeBox 56×56)
      │  └─ Img_Icon (Image, 默认 Collapsed)
      ├─ VB_Name    (VerticalBox, Fill, 左内边距 10)
      │  ├─ Text_Name (TextBlock 14.3, #C6D4DA, 超长省略号)
      │  └─ Text_Type (TextBlock 11,   #4A5D68)
      ├─ Size_Price (SizeBox 宽 110) └─ Text_Price (TextBlock 14.3, #95741F, 右对齐)
      ├─ Size_Stock (SizeBox 宽 88)  └─ Text_Stock (TextBlock 12.1, 右对齐)  ← **Collapsed**
      └─ Size_Buy   (SizeBox 宽 76)
         └─ Btn_Buy (Button, 底色 #1B140B)
            └─ Text_Buy (TextBlock 13.2, #95741F,「购买」)
```

`Size_Stock` 整块设了 `Visibility = Collapsed`，不参与布局，名称区自动吃掉这 88 宽。

> **为什么还留着它**：父类 `UDZ_ShopEntryWidget` 目前仍声明 `Text_Stock` 的 `BindWidget`，直接从 JSON 删掉会让 WidgetJsonBridge 的导入校验失败。等 C++ 删了这个属性，把 `Size_Stock` + `Text_Stock` 一起移除即可（第三章「待清理」）。

---

## 需要程序接的节点

| 节点 | 类型 | BindWidget 属性 | 状态 | 程序用它做什么 |
|---|---|---|---|---|
| `Text_Name` | TextBlock | `Text_Name` | ✅ | 商品名 |
| `Text_Price` | TextBlock | `Text_Price` | ✅ | `$价格` |
| `Text_Type` | TextBlock | `Text_Type` | ✅ | 类型副标题（护甲 / 主武器 / 近战武器 / 背包 / 物资） |
| `Img_Icon` | Image | `Img_Icon` | ✅ | 商品图标；无图标时为 `Collapsed` |
| `Btn_Buy` | Button | `Btn_Buy` | ✅ | 购买按钮；`UDZ_ShopItemObject::bCanBuy` 为 false 时禁用 |

`Root_Size`、`BG_Row`、`HB_Row`、`Size_Icon`、`VB_Name`、`Size_Price`、`Size_Buy`、`Text_Buy` 不需要绑定。

> 旧的 `Text_Stock` 已从 C++ 删除，节点还留在 JSON 里（`Collapsed`），下次清理时移除。

---

## 交互

| 节点 | 事件 | 程序要做什么 |
|---|---|---|
| `Btn_Buy` | `OnClicked` | `GetListItem<UDZ_ShopItemObject>()` 向列表要当前条目，沿 Outer 链找到所属 `UDZ_ShopPanel` 并调用 `RequestBuy(Item)`。**不要**在这里直接扣钱或改仓库 |

按钮点击的语义：Entry 只负责「哪一条被买了」，买不买得起、仓库放不放得下，全部由面板 / GameInstance 判定。

---

## 绘制入口（`OnEntryRefresh`）

条目继承 `UDZ_ListEntryWidget`，自己不缓存 item。写控件只发生在一个函数里：

```cpp
virtual void OnEntryRefresh(UObject* Item) override;
```

两条调用路径最终都落到它：

```
引擎绑定 ─→ UDZ_ListEntryWidget::NativeOnListItemObjectSet ─→ OnEntryRefresh(Item)
列表刷新 ─→ UDZ_ListView::RefreshDisplayedEntries() ────────→ OnEntryRefresh(Item)
```

所以这个函数里必须把**所有**字段都写全，漏一项就会拿到上一条商品的残留状态：

| 项 | 重置规则 |
|---|---|
| `Text_Name` / `Text_Type` / `Text_Price` | 每条都重写，空值也要写空字符串 |
| `Img_Icon` | 有图标 → `SetBrushFromTexture` + `Visible`；没图标 → `Collapsed` |
| `Btn_Buy` | 每次刷新都重新 `SetIsEnabled`，不要只在构造时设一次 |

`Btn_Buy` 的点击回调在 `NativeOnInitialized` 里只绑一次。

---

## 状态

| 状态 | 哪些节点变 | 变成什么 |
|---|---|---|
| 正常可买 | `Btn_Buy` | 可用，文字「购买」 |
| 余额不足 | `Text_Price` / `Btn_Buy` | 价格转暗红 `#8C4A3C`，按钮禁用 |
| 仓库放不下 | `Btn_Buy` | 按钮禁用（可选：价格不动，只在点击时用 Tips 说明） |
| 无图标 | `Img_Icon` | `Collapsed`，名称区自动左移 |

> 本版**没有售罄状态**，商店无限供应。
> 条目的选中态（`NativeOnItemSelectionChanged`）不使用，购买是行内按钮。

---

## 待清理（等 C++）

C++ 删掉 `UDZ_ShopEntryWidget::Text_Stock` 之后，本 JSON 需要再删一次：

1. 删 `Size_Stock` 整个节点（含内部的 `Text_Stock`）
2. 名称区宽度不用调，`VB_Name` 是 Fill，会自动占满

---

## 已知事项

- 行高 72，面板 `Size_List` 高 592，能放下 **8 行**；改任一个都要同步另一个。
- 图标源只有 6 张（Ammo / Armor / Grenade / MedKit / Potion / Rifle）。近战武器没有图标，那一条会走 `Collapsed` 分支。
