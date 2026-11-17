# WBP_RaidTime 交互文档

屏幕右上角的局内计时器，显示本局已进行时间（`MM:SS`）。数值由 **MVVM** 从 ViewModel 推过来，控件本身不做计算、不读 GameState。

| 项 | 值 |
|---|---|
| 资产 | `/Game/UI/HUD/WBP_RaidTime` |
| 源文件 | `UIWidgets/UI/HUD/WBP_RaidTime.json` |
| 父类 | `UDZ_RaidTime`（`UDZ_UserWidget` 子类，空壳，只为 HUD 根 `BindWidget` 提供类型） |
| 宿主 | `WBP_HUD` 的子控件（`UDZ_HUDLayout::RaidTime`） |
| 设计尺寸 | `Desired` |

---

## 1. 结构

```
Size_RaidTime (SizeBox, SelfHitTestInvisible, 根节点, 自适应尺寸)
└─ Border_RaidTimeOutline (Border, 1px 描边, 深色)
   └─ Border_RaidTimeFill (Border, 18/6 内边距, 深色底)
      └─ Text_RaidTime (TextBlock, 20px Bold, 居中, MinDesiredWidth=64)
```

- **位置**：不在本控件里，由 `WBP_HUD` 的槽位决定（锚点右上 `(1,0)`、轴心 `(1,0)`、偏移 `left=-28 / top=18`）。文本变长时向左扩展（轴心 `x=1`），不会越过屏幕右边界。
- **输入**：根节点 `SelfHitTestInvisible`，不拦截鼠标，不参与输入模式切换。
- **配色**：沿用 `WBP_Tips` 的描边/底色/文字色，保证 HUD 视觉一致。

---

## 2. 需要程序接的节点

MVVM 路线下 **没有 `BindWidget` 成员**，控件全部通过 View Bindings 声明式更新。

| 节点 | 类型 | 用途 |
|---|---|---|
| `Text_RaidTime` | `UTextBlock` | 显示 `MM:SS`；`isVariable: true`，供 View Bindings 按名字选中 |

其余节点纯装饰，程序不需要引用。

> ⚠️ **名字即契约**：View Bindings 的 widget 源是按控件名（`WidgetTree->FindWidget`）解析的。改 JSON 里 `Text_RaidTime` 这个名字，绑定会失效，只在 WBP 编译时报警。

---

## 3. 绑定契约

由界面侧在 WBP 的 **View Bindings** 面板里配置（学习用途，本人手工配）：

| 项 | 值 |
|---|---|
| ViewModel 类 | `UDZ_RaidTimeViewModel`（继承 `UMVVMViewModelBase`） |
| ViewModel 创建方式 | `Manual`（实例由 controller 注入，不走 Create Instance） |
| 绑定 | `Text_RaidTime.Text` ← VM 的 `FText` 属性（建议名 `TimeText`） |
| 绑定模式 | One Way（VM → 控件） |
| 转换函数 | 不需要：VM 直接给格式化好的 `FText` |

为什么不放 `float`：`float → FText` 需要在绑定里挂一个自定义转换函数，多一层没有收益；格式化逻辑留在 VM 里，HUD 与结算面板以后可以共用同一份。

---

## 4. 数据流

```
ADZ_GameState::GetElapsedTime()          （= GetServerWorldTimeSeconds() - StartTime，客户端可用）
  → UDZ_RaidTimeController::Tick          （已由 ADZ_PlayerController::Tick → UIManager->TickControllers 驱动）
  → VM->SetElapsedSeconds(Seconds)        （VM 内部格式化为 MM:SS）
  → FieldNotify 广播（UE_MVVM_SET_PROPERTY_VALUE，值没变就不广播）
  → View Binding 把 FText 写进 Text_RaidTime.Text
```

- **不做本地计算**：秒数来自 GameState 的服务器时间差，客户端与服务端同源，两边显示一致。
- **不需要 Widget Tick**：controller 已有 Tick 通道，无需 `FTimerHandle`、不需要在 WBP 里写 Graph 定时器。
- **不需要 RPC**：`StartTime` 已经是 `Replicated` 字段，客户端本地求差即可。

---

## 5. 状态

| 状态 | 触发 | 表现 |
|---|---|---|
| 正常计时 | 进局后每秒 +1 | `Text_RaidTime` 显示 `MM:SS`（两位数补零） |
| 起始值 | 局内 map 加载完成、`ADZ_GameState::BeginPlay` 写入 `StartTime` | `StartTime` 在服务器世界起点为 `0`，属于合法值；显示从 `00:00` 起计时 |
| 超长局 | 超过 99 分钟 | 自然变成三位数分钟，不做截断、不做小时进位 |
| 未进局 / 无 GameState | controller 取不到 GameState | 保持 `00:00`，不崩、不刷错误 |

---

## 6. 实现状态

**已完成**

| 项 | 位置 |
|---|---|
| 控件树 JSON | `UIWidgets/UI/HUD/WBP_RaidTime.json` |
| `UDZ_RaidTimeViewModel`（`TimeText` FieldNotify + `SetElapsedSeconds`） | `Source/.../Public/UI/ViewModel/DZ_RaidTimeViewModel.h` + `.cpp` |
| `UDZ_RaidTime` 空壳控件类 | `Source/.../Public/UI/Widgets/HUD/DZ_RaidTime.h` + `.cpp` |
| `UDZ_HUDLayout::RaidTime`（`BindWidget`） | `Source/.../Public/UI/Widgets/HUD/DZ_HUDLayout.h` |
| `UDZ_RaidTimeController`（由 HUD 控制器创建并注入 widget；建 VM、注入、Tick 推秒数） | `Source/.../Public/UI/Controllers/DZ_RaidTimeController.h` + `.cpp` |

**待做**

| 项 | 负责 | 说明 |
|---|---|---|
| View Bindings 配置 | 你（学习 MVVM） | 见 §3；`Text_RaidTime.Text` ← `TimeText`，One Way，VM 创建方式 Manual |
| `DA_UIConfig::HUDClass` 指向 `WBP_HUD` | ✅ 已配 | 未填时 HUD 根不会创建 |
| PIE 验收 | 程序 | 见 §7 |

> 父类已由 JSON 的 `parentClass` 在建资产时指定为 `UDZ_RaidTime`，不需要再手工 Reparent。

---

## 7. 验收

| 场景 | 期望 |
|---|---|
| 进局 | 右上角出现计时器，从 `00:00` 开始每秒 +1 |
| 双人 PIE | 两个客户端秒数一致（同一份 server time） |
| 刚连入的客户端 | 不出现服务器运行时长之类的假值 |
| 打开背包 / 搜刮面板 | 计时器仍在、不被遮挡、不重复创建 |
| 离开局内（`UIManager::Shutdown`） | 控件被移除，VM 随 controller 释放，无残留 |
| 鼠标点击 HUD 区域 | 计时器不拦截输入 |

---

## 8. 已知限制

- **改名断绑**：`Text_RaidTime` 改名会静默断开 View Binding。
- **JSON 重建不保留 MVVM 绑定以外的资产内修改**：`animations` 字段缺省时不动已有动画；但如果以后给这个 WBP 加动画，要在 JSON 的 `animations` 里表达，否则编辑器里手加的动画会被下一次导入删除。
- **暂停语义**：秒数取自 world 的服务器时间，世界暂停时计时停止；这是当前期望行为，若要「真实经过时间」需另换时间源。
- **MVVM 为 Beta 插件**：升级引擎时留意 setter / 广播 API 的废弃变化。
