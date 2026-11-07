# WBP_StatusBars 交互文档

常驻屏幕左下角的状态条，显示生命与能量。

## 1. 资产

| 项 | 值 |
|---|---|
| 资产路径 | `/Game/UI/HUD/WBP_StatusBars` |
| JSON 源 | `UIWidgets/UI/HUD/WBP_StatusBars.json` |
| 父类 | `UDZ_StatusBars`（待建；未建前先用 `UDZ_UserWidget`，但拿不到属性绑定） |
| 设计尺寸 | `Desired` |

## 2. 控件树

```
RootCanvas (CanvasPanel, SelfHitTestInvisible)
└─ Size_Status (SizeBox 340×64, 锚点左下 (0,1), 偏移 40,-40)
   └─ VB_Status (VerticalBox)
      ├─ HB_Health (HorizontalBox)
      │  ├─ Text_HealthLabel   "生命"
      │  ├─ Spacer_Health      Fill
      │  └─ Text_HealthValue   "100 / 100"
      ├─ Size_HealthBar (SizeBox h=8, top padding 3)
      │  └─ Bar_Health (ProgressBar, 暗红)
      ├─ HB_Energy (top padding 8)
      │  ├─ Text_EnergyLabel   "能量"
      │  ├─ Spacer_Energy      Fill
      │  └─ Text_EnergyValue   "100 / 100"
      └─ Size_EnergyBar (SizeBox h=6, top padding 3)
         └─ Bar_Energy (ProgressBar, 青)
```

## 3. 程序要绑定的节点

| 节点 | 类型 | 用途 |
|---|---|---|
| `Bar_Health` | `UProgressBar` | 血量百分比 |
| `Bar_Energy` | `UProgressBar` | 能量百分比 |
| `Text_HealthValue` | `UTextBlock` | `当前 / 上限` |
| `Text_EnergyValue` | `UTextBlock` | `当前 / 上限` |

`Text_HealthLabel` / `Text_EnergyLabel` 是静态文案，不需要代码写入。

## 4. 数据流

```
服务器改属性
  → AttributeSet 复制
  → 客户端属性变化委托
  → WBP_StatusBars 刷新
```

**不主动拉数据，不做本地计算。** 百分比 = `Current / Max`，两边都用复制值。

属性委托：

- `GetGameplayAttributeValueChangeDelegate(HealthAttribute)`
- `GetGameplayAttributeValueChangeDelegate(MaxHealthAttribute)`
- `GetGameplayAttributeValueChangeDelegate(EnergyAttribute)`
- `GetGameplayAttributeValueChangeDelegate(MaxEnergyAttribute)`

## 5. 显示方式

- **常驻**：`AddToViewport` 一次，不随背包/搜刮面板开关。
- **层级**：通过 `UDZ_UIConfig::ZOrderMap` 配置，建议在交互提示之下。
- **输入**：`SelfHitTestInvisible`，不拦截鼠标，不参与输入模式切换。

## 6. 程序侧未完成项

1. 新建 `UDZ_StatusBars`（父类建议 `UDZ_UserWidget`），声明上面 4 个 `BindWidget`。
2. `UDZ_UIConfig` 加 `TSubclassOf<UDZ_StatusBars> StatusBarsClass`。
3. `ADZ_PlayerController::BeginPlay` 里创建并 `AddToViewport`。
4. `OnRep_PlayerState` 里绑定 4 个属性委托。
5. 回调里刷新 `Percent` 与文本。
6. 角色死亡 / 无 Pawn 时是否隐藏，待定。

## 7. 验收

| 场景 | 期望 |
|---|---|
| 进游戏 | 左下角出现生命条与能量条 |
| 受伤 | 血条变短，数字同步 |
| 能量恢复 | 能量条每秒增长 |
| 打开背包 / 搜刮 | 状态条仍在，不被遮挡 |
| 属性满值 | 条为满格，停在 100% |
