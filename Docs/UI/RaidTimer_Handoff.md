# 局内计时器（MVVM）交接说明

本文件记录「HUD 右上角局内时间」这条链路的资产清单、数据流、界面与代码的接缝，以及当前状态。节点级细节见 `Docs/UI/WBP_RaidTime.md`；HUD 整体结构见 `Docs/UI/WBP_HUD.md` 与 `Docs/UI/HUD_Root_Handoff.md`。

> 这是项目第一次使用 UE 的 MVVM（`ModelViewViewModel` 插件）。刻意的取舍：把「格式化 + 刷新」从 Widget 挪到 ViewModel、绑定声明化，代价是这一行文本比 `SetText` 多一层。收益在复用（结算面板的用时可以共用同一份格式化）与后续扩展。

## 1. 资产与代码清单

| 资产 / 文件 | 路径 | 状态 |
|---|---|---|
| `WBP_RaidTime` | `/Game/UI/HUD/WBP_RaidTime` | ✅ 由 JSON 生成，父类 `UDZ_RaidTime` |
| JSON 源 | `UIWidgets/UI/HUD/WBP_RaidTime.json` | ✅ |
| `UDZ_RaidTimeViewModel` | `Source/.../Public/UI/ViewModel/DZ_RaidTimeViewModel.h` + `.cpp` | ✅ `FText TimeText`（FieldNotify）+ `SetElapsedSeconds` + `FormatElapsedSeconds` |
| `UDZ_RaidTime` | `Source/.../Public/UI/Widgets/HUD/DZ_RaidTime.h` + `.cpp` | ✅ 空壳控件类（供 `BindWidget` 与 controller 强类型引用） |
| `UDZ_HUDLayout::RaidTime` | `Source/.../Public/UI/Widgets/HUD/DZ_HUDLayout.h` + `.cpp` | ✅ `BindWidget` + getter |
| `UDZ_RaidTimeController` | `Source/.../Public/UI/Controllers/DZ_RaidTimeController.h` + `.cpp` | ✅ 由 `UDZ_HUDController` 创建，`SetRaidTimeWidget` 注入控件；建 VM、注入、Tick 推秒数 |
| `UDZ_UIConfig::HUDClass` | `Source/.../Public/Data/UI/DZ_UIConfig.h` | ✅ 字段已加，DA 已指向 `WBP_HUD` |

## 2. 数据分层

| 层 | 对象 | 位置 | 可变性 |
|---|---|---|---|
| Model | `ADZ_GameState::GetElapsedTime()` / `GetRunStartTime()` | `Game/DZ_GameState` | 只读（`GetServerWorldTimeSeconds() - StartTime`，`StartTime` 已 `Replicated`） |
| ViewModel | `UDZ_RaidTimeViewModel` | 由 `UDZ_RaidTimeController` `NewObject` 持有（`UPROPERTY`） | 运行时可变，字段变更走 FieldNotify |
| View | `WBP_RaidTime`（`WBP_HUD` 的子控件） | `/Game/UI/HUD/` | 只做展示，不读 GameState、不格式化 |

没有新增数据字段、没有新增网络复制。秒数在客户端本地求差，双人 PIE 下两端一致。

## 3. 界面 ↔ 代码接缝

### 3.1 绑定契约（界面侧配置）

| 项 | 约定 |
|---|---|
| ViewModel 类 | `UDZ_RaidTimeViewModel` |
| ViewModel 创建方式 | `Manual` |
| 绑定 | `Text_RaidTime.Text` ← VM 的 `TimeText`，One Way |
| 转换函数 | 无（VM 直接给 `FText`） |
| VM 实例来源 | `UDZ_RaidTimeController`：`NewObject` → `UMVVMBlueprintLibrary::SetViewModelByClass(Widget, VM)` |

**Manual 是硬要求**：C++ 侧没有「按类取已创建实例」的 API。若界面侧配成 Create Instance，controller 拿不到实例，控件会一直停在 `00:00`。

### 3.2 已实现的接口

| 位置 | 接口 | 行为 |
|---|---|---|
| `UDZ_RaidTimeViewModel` | `TimeText`（`UPROPERTY(FieldNotify, BlueprintReadOnly)`） | 只读字段，绑定源 |
| `UDZ_RaidTimeViewModel` | `SetElapsedSeconds(float)` | 内部 `FormatElapsedSeconds` → `UE_MVVM_SET_PROPERTY_VALUE`，值没变不广播 |
| `UDZ_RaidTimeViewModel` | `static FormatElapsedSeconds(float)` | 秒 → `MM:SS`，结算面板以后可复用 |
| `UDZ_RaidTimeController` | `Initialize` | `GetHUDLayout()->GetRaidTime()` → 建 VM → 注入 → 立即推一次 |
| `UDZ_RaidTimeController` | `Tick` | 每帧 `RefreshTime()`；受益于 VM 侧的变更判断，实际每秒才写一次控件 |
| `UDZ_RaidTimeController` | `RefreshTime` | 读 `ADZ_GameState`，直接推 `GetElapsedTime()`。注意 `StartTime` 在局内 map 起点本身就是 `0`，是合法值，不能用 `> 0` 判断「是否已复制」 |

## 4. 顺序依赖

1. `ModelViewViewModel` 插件 + `DecayZone.Build.cs` 的模块依赖（已完成）。
2. `UDZ_RaidTimeViewModel` / `UDZ_RaidTime` / `UDZ_HUDLayout` 编译通过后，`WBP_HUD` 与 `WBP_RaidTime` 才能导入（`BindWidget` 与 `parentClass` 都依赖这些类）。
3. `DA_UIConfig.HUDClass` 必须指向 `WBP_HUD`，否则 HUD 根不会被创建，`GetHUDLayout()` 返回 null，计时器静默不显示。
4. View Bindings 由界面侧在编辑器里配置（见 §3.1）。

## 5. 未完成项

| 项 | 状态 | 备注 |
|---|---|---|
| View Bindings 三条配置 | ☐ 待做 | 手工配置（学习 MVVM） |
| `DA_UIConfig.HUDClass` 指向 `WBP_HUD` | ✅ 已完成 | 编辑器里指到 `WBP_HUD` |
| PIE 单机验收 | ☐ 待做 | 见 §6 |
| PIE 双人验收 | ☐ 待做 | 两端秒数一致 |
| 结算面板复用 `FormatElapsedSeconds` | ☐ 后续 | `BuildSettlementInfo` 已有 `ElapsedSeconds` |

## 6. 验收

| 场景 | 期望 |
|---|---|
| 进局 | 右上角计时器 `00:00` 起每秒 +1 |
| 双人 PIE | 两端秒数一致 |
| 后加入的客户端 | 不出现假值（`StartTime` 未复制时显示 `00:00`） |
| 打开背包 / 搜刮面板 | 计时器不消失、不被遮挡 |
| 鼠标点 HUD | 计时器不拦截输入 |
| 退出局内 | HUD 整棵销毁，VM 随 controller 释放，无 ensure |

## 7. 已知风险

1. **改名断绑**：View Bindings 按控件名解析，改 `Text_RaidTime` 名字会静默断开绑定，只在 WBP 编译时报无效路径。
2. **BindWidget 名字契约**：`WBP_HUD` 里的节点名 `RaidTime` 必须与 `UDZ_HUDLayout::RaidTime` 一致。
3. **MVVM 是 Beta 插件**：升级引擎时检查 `UE_MVVM_SET_PROPERTY_VALUE` 等宏与废弃项。
4. **暂停语义**：world 暂停时秒数停跳（沿用 server time），与「真实经过时间」不同。
