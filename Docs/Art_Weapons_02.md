# 美术交付：步枪与枪械表现资源

> 日期：2026-11-26
> 范围：步枪模型、材质、枪口火光、枪声、弹孔、命中火花、射击蒙太奇、准心 UI
> 不包含：GameplayCue 资产、后坐力、ADS 相机、连发逻辑、弹药与换弹、任何 C++ 代码

## 1. 来源与授权

| 项 | 内容 |
|---|---|
| 步枪模型 | Quaternius《Rifle》，CC0 1.0（公共领域，可商用、无需署名） |
| 步枪来源页 | `https://poly.pizza/m/cCAgiMOQow` |
| 枪声 | 《The Free Firearm Sound Library》（Ben Jaszczak 等），CC0 1.0 |
| 枪声来源页 | `https://opengameart.org/content/the-free-firearm-sound-library` |
| 原始录音 | `Prepared SFX Library/AR-15/D_32P.wav` 第 5.646 秒单发 |
| 贴图 / 网格 / 材质 | 本项目自建（程序化贴图 + Blender 网格 + UE 材质） |

源文件：

- `Content/Assets/_Source/Weapons/SM_Weapon_Rifle_01.glb` / `.fbx` / `.source.json`
- `Content/Assets/_Source/FX/Weapon/`：`T_MuzzleFlash.png`、`T_ImpactSpark.png`、`T_BulletHole.png`、`SM_MuzzleFlash.glb/.fbx`、`SM_ImpactSpark.glb/.fbx`、`S_Weapon_Rifle_Fire_01.wav` 及同名 `.json`
- `Content/Assets/_Source/UI/Crosshair/T_Crosshair.png`

## 2. 资产清单

| 资产 | 路径 | 说明 |
|---|---|---|
| 步枪网格 | `/Game/Assets/Weapons/SM_Weapon_Rifle_01` | 2836 三角面，5 个材质槽，插槽 `Muzzle` |
| 枪身金属 | `/Game/Assets/Weapons/Materials/M_Weapon_Metal` | 深灰金属，Metallic 0.85 / Roughness 0.38 |
| 聚合物件 | `/Game/Assets/Weapons/Materials/M_Weapon_Polymer` | 近黑枪身件，Metallic 0.05 / Roughness 0.62 |
| 瞄具镜片 | `/Game/Assets/Weapons/Materials/M_Weapon_Glass` | 深蓝底 + 微弱青色自发光，不透明 |
| 显示蓝图 | `/Game/Actors/BP_Weapon_Rifle` | 父类 `StaticMeshActor`，Mesh = 步枪；Movable、NoCollision、bReplicates |
| 枪口火光网格 | `/Game/Assets/FX/Weapon/SM_MuzzleFlash` | 3 片轴向 + 1 片枪口正面，长 52 cm；槽 0 = 火光，槽 1 = 核心 |
| 命中火花网格 | `/Game/Assets/FX/Weapon/SM_ImpactSpark` | 三向交叉面片，32 cm 见方 |
| 枪口火光材质 | `/Game/Assets/FX/Weapon/Materials/M_MuzzleFlash` | Unlit、Additive、双面；Emissive = RGB × A |
| 命中火花材质 | `/Game/Assets/FX/Weapon/Materials/M_ImpactSpark` | 同上 |
| 弹孔贴花材质 | `/Game/Assets/FX/Weapon/Materials/M_BulletHole_Decal` | Deferred Decal、Translucent；BaseColor + Opacity(A) |
| 枪声 | `/Game/Assets/Audio/Weapon/S_Weapon_Rifle_Fire_01` | 48 kHz 单声道 PCM16，1.35 s，峰值 -1.0 dBFS |
| 声音衰减 | `/Game/Assets/Audio/Weapon/ATT_Weapon_Rifle` | 球形，内半径 250 cm，衰减距离 7000 cm，Linear |
| 射击蒙太奇 | `/Game/Characters/Player/AM_Rifle_Fire` | 0.35 s，槽 `UpperBody`，`Gun_Shoot` 0–0.7 s、2 倍速 |
| 准心贴图 | `/Game/UI/Textures/T_Crosshair` | 128²，白线 + 深色描边 |
| 准心界面 | `/Game/UI/HUD/WBP_Crosshair` | 已挂到 `WBP_HUD` 的 `Crosshair` 子节点 |

## 3. 单位与朝向

- 步枪长度 **90 cm**，高度约 25 cm，宽约 6.4 cm；参考角色身高约 182 cm。
- 枪口朝 **局部 +X**，枪身上方为 **+Z**，握把枢轴在原点。
- `Muzzle` 插槽：局部坐标 **(66, 0, 9)**。
- 网格长度沿 X：-23.2（枪托）到 +66.8（枪口）。

## 4. 音效

| 项 | 值 |
|---|---|
| 格式 | 48 kHz / 单声道 / PCM16 |
| 时长 | 1.35 s |
| 峰值 / RMS | -1.0 dBFS / -31.8 dBFS |
| 起振 | 4 ms（10%→峰值） |
| 尾部 | 120 ms 淡出 |
| 授权 | CC0 1.0，可商用、无需署名 |

## 5. 蒙太奇

- 槽位 `UpperBody`，时长 0.35 s。
- 片段引用 `/Game/Assets/Player/Gun_Shoot`，从 0 播到 0.7 s，播放速率 2.0。
- 通知：`DZ_AnimSendEvent`，时间 0.04 s，Tag = `Event.Attack.Ranged`。
- 原剑蒙太奇的 `Event.Attack.Melee` 通知已移除。

## 6. 准心

- `WBP_Crosshair` 使用 `T_Crosshair`，屏幕正中 64×64，`HitTestInvisible`。
- 交互文档：`Docs/UI/WBP_Crosshair.md`。

## 7. 已验证

- 网格包围盒、材质槽、`Muzzle` 插槽位置（66, 0, 9）已用无头脚本读取确认。
- 蒙太奇槽位、动画引用、时长、通知 Tag 已读取确认。
- 枪声已过 `check_audio.py --transient`：非静音、瞬态存在、频段正常。
- `BP_Weapon_Rifle`：Mesh 已指定，Movable、NoCollision、bReplicates = true。
- `WBP_Crosshair` 已生成并被 `WBP_HUD` 引用，JSON 桥无报错。

## 8. 已知问题

1. **挂载朝向未在 PIE 校准**：`BP_Weapon_Rifle` 的 Mesh 相对变换是单位变换，挂到 `hand_r` 后枪的朝向与位置很可能需要补旋转/位移。剑也存在同样问题。
2. **火光尺寸未在画面里调过**：网格长 52 cm，实际使用时可能偏大，建议在 Cue 里按需缩放。
3. **枪声 RMS 偏低**：枪声是瞬态型，峰值已到 -1 dBFS；如果听感偏小，靠 SoundClass/音量调，不要提峰值。
4. **枪口模型带消音器，音效用的是正常步枪单发**：造型与声音不完全对应，属于观感取舍。
5. **镜片不透明**：`M_Weapon_Glass` 用自发光模拟镜片，没有做真正的半透明。
6. **没有 AimOffset**：项目里没有 8 方向瞄准动画；ADS 只能用 `Idle_Gun_Pointing` 姿态。

## 9. 需要在引擎里手动接的部分

- `DA_Weapon_Rifle_01`：填 `WeaponActorClass = BP_Weapon_Rifle`、`AttackAbilityClass = GA_RangedWeaponAttack_C`、`AttackMontage = AM_Rifle_Fire`；`Range` 建议 5000–8000，`AttackCooldown` 建议 0.1–0.15。
- 射击表现：新建 GameplayCue，在服务端执行，负责枪口火光（`SM_MuzzleFlash` + `M_MuzzleFlash`）、枪声（`S_Weapon_Rifle_Fire_01`）、命中火花与弹孔。
- 命中火花与弹孔需要 Cue 参数带命中点位置与法线。
- 相机抖动只在本机控制端播放，不走 Cue。
- 连发、ADS、后坐力、弹药与换弹均未实现。
- ABP 需要读取瞄准状态并切到 `Idle_Gun_Pointing`。

## 10. 数据表与既有资产

- `DT_ItemData` 的 `Weapon_Rifle_01` 行已存在并指向 `DA_Weapon_Rifle_01`，本次未改数据表。
- `DA_Weapon_Rifle_01` 目前 `WeaponActorClass` / `AttackAbilityClass` / `AttackMontage` 为空，`Range` 仍为 200。
