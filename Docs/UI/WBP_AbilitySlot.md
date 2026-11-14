# WBP_AbilitySlot 交互文档

单个技能槽。被 `WBP_AbilityBar` 以实例形式摆两份（`AbilitySlot_1` / `AbilitySlot_2`）。

## 1. 资产

| 项 | 值 |
|---|---|
| 资产路径 | `/Game/UI/HUD/WBP_AbilitySlot` |
| JSON 源 | `UIWidgets/UI/HUD/WBP_AbilitySlot.json` |
| 父类 | `UDZ_AbilitySlot`（已建，继承 `UDZ_UserWidget`） |
| 设计尺寸 | `Desired`，根 `SizeBox` 固定 64×64 |

## 2. 控件树

```
Root_Size (SizeBox 64×64)
└─ Border_Frame (Border, 2px, #070B0E 底)
   └─ Border_Active (Border, 2px, 青绿 #2ED0C0, 默认 Collapsed)   ← 持续型技能激活时显示
      └─ Fill_Slot (Border, 3px, #020203 近黑, 内缩 4)
         └─ Overlay_Slot (Overlay)
            ├─ Img_Icon          (Image, 默认 Collapsed)           ← 程序喂图标贴图
            ├─ Img_CooldownMask  (Image, 默认 Collapsed, Brush=MI_CooldownRing) ← 控件内部取动态实例
            ├─ Border_Disabled   (Border, 黑 55% 透明, 默认 Collapsed) ← 能量不足时变暗
            ├─ Text_Cooldown     (TextBlock, 居中, 15 Bold, 默认空) ← 剩余秒数
            └─ Text_Key          (TextBlock, 左上, 10 Bold, "1")    ← 键位角标
```

层级顺序即绘制顺序：图标在最下，冷却遮罩压图标，变暗层压遮罩，两个文本在最上。

## 3. 程序要绑定的节点

| 节点 | 类型 | 用途 |
|---|---|---|
| `Img_Icon` | `UImage` | 技能图标，`SetBrushFromTexture` |
| `Img_CooldownMask` | `UImage` | 冷却遮罩，`GetDynamicMaterial()` 后设 `Percent` |
| `Border_Disabled` | `UBorder` | 能量不足 / 不可用时的变暗层 |
| `Border_Active` | `UBorder` | 持续型技能生效时的高亮框 |
| `Text_Cooldown` | `UTextBlock` | 冷却剩余秒数（`0` 或空串表示不显示） |
| `Text_Key` | `UTextBlock` | 键位角标文案 |

`Border_Frame` / `Fill_Slot` / `Overlay_Slot` 是纯装饰节点，不需要绑定。

## 4. 四个状态

| 状态 | `Border_Active` | `Border_Disabled` | `Img_CooldownMask` | `Text_Cooldown` |
|---|---|---|---|---|
| 空槽 | Collapsed | Collapsed | Collapsed | 空 |
| 就绪 | Collapsed | Collapsed | Collapsed | 空 |
| 冷却中 | Collapsed | Collapsed | Visible，`Percent` = 剩余比例 | 剩余整秒数，如 `7` |
| 能量不足 | Collapsed | Visible | 按冷却实际状态 | 按冷却实际状态 |
| 生效中（持续型） | Visible | Collapsed | 可显示剩余持续时间 | 剩余整秒数 |

「生效中」只对持续型技能有意义（护盾持续、加速持续）。如果最终做成瞬发进冷却，这一行永远不出现，`Border_Active` 保持 Collapsed 即可。

## 5. 图标与遮罩的来源

- `Img_Icon`：JSON 里不带贴图，图标由控制器的 `SetIcon` 喂入，与 `WBP_ItemSlot` 一致。Designer 里看到整格灰色属预期。
- `Img_CooldownMask`：JSON 的 Brush 直接引用 `MI_CooldownRing`，控件在 `NativeOnInitialized` 里取 `GetDynamicMaterial()` 缓存。做法与 `WBP_InteractProgress` 相同，C++ 不持有材质引用。

| 资源 | 路径 |
|---|---|
| 护盾图标 | `/Game/Assets/SkillIcons/Runtime/T_UI_Icon_Skill_Shield` |
| 加速图标 | `/Game/Assets/SkillIcons/Runtime/T_UI_Icon_Skill_Sprint` |
| 冷却遮罩材质 | `/Game/UI/Materials/M_CooldownRing` |
| 冷却遮罩材质实例 | `/Game/UI/Materials/MI_CooldownRing` |

`M_CooldownRing` 只有一个标量参数 `Percent`：`0` = 无遮罩，`1` = 整圆被遮住，`0.4` = 从 12 点方向顺时针遮住 40% 的扇形。材质输出的是一层半透明暗色（Opacity ≈ 0.72），不需要额外接颜色。

## 6. 数据流

槽位不持有技能身份，只接收一份显示数据：

```
ADZ_PlayerController
  ├─ 槽位映射 SlotSkillIds[i] = SkillId（int32）
  ├─ 技能行按 SkillId 查 DT_SkillData → AbilityTag / 图标
  │
  ├─ 冷却：技能 Spec 的 GetCooldownTimeRemainingAndDuration
  ├─ 可用性：技能 Spec 的 CheckCost（纯检查，无副作用）
  └─ 生效中：技能 Spec 的 IsActive()
       → UDZ_AbilityBar::SetEmpty / SetKeyText / SetIcon / SetCooldown / SetAvailable / SetActive
       → UDZ_AbilitySlot 对应 setter
```

> UE 5.8 已没有 `ASC->GetCooldownRemainingForTag`，冷却必须从技能自己的 AbilitySpec 取。

**槽位不自己算时间，也不自己判定能不能放。** 时间与可用性都来自 ASC 与复制过的属性值。技能换槽后冷却跟着技能走，不需要迁移任何计时状态。

## 7. 已实现接口

`Source/DecayZone/Public/UI/Widgets/HUD/DZ_AbilitySlot.h`

| 方法 | 行为 |
|---|---|
| `SetKeyText(const FText&)` | 写左上角键位角标 |
| `SetIcon(UTexture2D*)` | 空则隐藏图标；与上次相同则不重建 Brush |
| `SetCooldown(float Percent, float RemainingSeconds)` | `Percent > 0` 显示扇形遮罩并写材质 `Percent`；`RemainingSeconds > 0.5` 显示向上取整的整秒 |
| `SetAvailable(bool)` | 切换 `Border_Disabled` |
| `SetActive(bool)` | 切换 `Border_Active`（仅持续型技能会亮） |
| `SetEmpty(bool)` | 空槽：隐藏图标、收起所有遮罩与边框、清空秒数；非空状态下的后四个 setter 才会生效 |

实现要点：

1. 冷却材质的动态实例在 `NativeOnInitialized` 里取一次并缓存，不每帧创建。
2. 冷却结束（`Percent <= 0`）时遮罩与秒数一起清空，不残留。
3. `SetAvailable(false)`（变暗）与冷却遮罩互不覆盖，两者可同时存在。
4. 本类不查询 ASC、不读能量、不认识任何技能；数据全部由 `UDZ_AbilityBarController` 组装后通过原语 setter 推入。
5. 图标按指针缓存，重复推同一个图标不会重建 Brush。

## 8. 依赖玩法侧的部分

| 项 | 说明 |
|---|---|
| 冷却 | 技能 GA 必须挂 Cooldown GE，且 CD GE 授予冷却 Tag；否则 `GetCooldownTimeRemainingAndDuration` 恒为 0 |
| 可用性 | 技能 GA 必须有 Cost GE；`CheckCost` 读的是 Cost GE 对属性的实际要求 |
| 生效中 | 只有「持续型、Spec 保持激活」的技能会让 `Border_Active` 变亮；瞬发 + 时长 GE 不会 |
| 技能授予 | 技能必须出现在 ASC 的 ActivatableAbilities 里，且 Asset Tag 与 `DT_SkillData` 里填的一致 |

## 9. 验收

| 场景 | 期望 |
|---|---|
| 空闲 | 两个 64×64 槽并排，图标清晰，左上角有 `1` / `2` |
| 没有 GA | 图标显示但整体变暗（`CheckCost` 找不到技能视为不可用），冷却遮罩不出现 |
| 放技能 | 扇形暗层从 12 点方向顺时针展开，中央出现倒计时数字 |
| 冷却结束 | 暗层与数字消失，图标恢复全亮 |
| 能量不足 | 图标整体变暗，但冷却遮罩仍按实际情况显示 |
| 持续型技能生效 | 槽外圈出现青绿描边 |
| 没有任何技能配置 | 槽位仍可见，图标隐藏、无变暗、无遮罩，键位保留 |
