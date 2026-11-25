# 美术交付：近战武器（剑）第一版

> 日期：2026-11-24
> 范围：玩家右手持握的剑——模型、材质、挂点 socket、纯显示蓝图、物品图标
> 不包含：枪的模型与蓝图、攻击动画、任何玩法逻辑

## 1. 来源与授权

| 项 | 内容 |
|---|---|
| 素材包 | KayKit Character Pack: Adventurers 1.0（Kay Lousberg） |
| 授权 | CC0 1.0，可商用、无需署名（License 原文已随源文件保存） |
| 使用对象 | `sword_1handed`（约 300 三角面，单材质槽） |

原始文件保存在 `Content/Assets/_Source/Weapons/`：`sword_1handed.gltf`、`sword_1handed.bin`、`knight_texture.png`、`LICENSE.txt`。

## 2. 资产清单

| 资产 | 路径 | 说明 |
|---|---|---|
| 剑模型 | `/Game/Assets/Weapons/SM_Sword_01` | 静态网格，1 个材质槽 `knight_texture`，未开 Nanite |
| 材质 | `/Game/Assets/Weapons/knight_texture` | 素材包自带的调色板贴图材质，随导入自动生成 |
| 显示蓝图 | `/Game/Actors/BP_Weapon_Sword` | 纯资源蓝图：根节点 + `WeaponMesh`（StaticMeshComponent）。无 Graph 逻辑 |
| 手部插槽 | `/Game/Assets/Player/Adventurer` → 插槽 `hand_r` | 挂在骨骼 `Wrist_R` 上 |
| 物品图标 | `/Game/Assets/ItemIcons/Runtime/T_UI_Icon_Sword` | 256×256，贴图设置复制自 `T_UI_Icon_Potion` |

显示蓝图的组件设置：

| 属性 | 值 | 备注 |
|---|---|---|
| `WeaponMesh.StaticMesh` | `SM_Sword_01` | |
| `WeaponMesh` 碰撞 | `NoCollision`（`bodyInstance`） | 第一版不做任何碰撞参与 |
| `WeaponMesh` Mobility | Movable | |
| `WeaponMesh` CastShadow | true | |
| `bReplicates` | true | 供服务端 spawn 后复制到客户端 |
| `bReplicateMovement` | false | 挂到手部 socket 后由骨骼驱动，不走移动复制 |

## 3. 单位与朝向

- 模型尺寸：长 **88.8 cm**、宽 25.2 cm、厚 6.5 cm（UE 单位 cm）。源文件按 1 单位 = 1 m 导出，导入时在 Blender 侧先乘 **0.5**（KayKit 原始单手剑为 177.5 cm，接近角色身高，不适合写实比例角色）。
- 参考：角色网格身高约 **182 cm**。剑长约为角色的一半。
- 轴与枢轴：枢轴在**握把中心**，剑尖沿**局部 +Z**，剑刃平面法线沿 **±Y**，剑柄末端（配重球）沿 **−Z**（约 −18.3 cm）。
- 插槽 `hand_r`：当前为相对 `Wrist_R` 的**单位变换（未做朝向校准）**。

## 4. 图标

- 渲染方式：Blender 5.2 EEVEE，直接渲染 `sword_1handed.gltf`，绕视线轴 45°，透明背景，1024²。
- 后处理：按 alpha 包围盒裁切，等比放进方形槽位（占比 0.92），输出 512 主图与 256 运行时图。
- 主图 `Content/Assets/_Source/UI/Icons/T_UI_Icon_Sword.png`（512），运行时 `_runtime.png`（256），参数记录在同目录 `T_UI_Icon_Sword.post.json`。

## 5. 已知问题

1. **插槽朝向未在 PIE 内校准**：`hand_r` 用的是单位变换，剑刃极可能不是自然持握角度；需要在接好挂载后按实际画面补旋转偏移。
2. 剑已按 0.5 重新导出（177.5 → 88.8 cm）。KayKit 原始比例是「武器与角色等高」的夸张风格，直接用会偏大。如需再调，可在显示 BP 的 `WeaponMesh` 上改 `RelativeScale3D`。
3. 材质是素材包调色板贴图 + 基础 PBR，未做二次调色，与场景内其它物件的色调未统一。
4. 图标与既有 AK 图标风格不完全一致（既有图标为写实渲染，本图标是直接渲染低模本体），优点是和游戏内模型一致。
5. 没有左手 IK 插槽（双手握持姿势需要另行增加 `hand_l` 目标点）。
6. 没有枪的模型、枪口插槽与显示蓝图。

## 6. 需要在引擎里手动接的部分

- 装备近战槽时在服务端生成 `BP_Weapon_Sword` 并附加到角色网格的 `hand_r`；卸下、死亡、换装时销毁。
- 攻击蒙太奇与武器数据的关联（`FDZ_WeaponParams.AttackMontage` 目前无逻辑引用）。
- 插槽朝向校准（见第 5 节第 1 条）。

## 7. 已写入的数据表配置

`/Game/Data/ItemData/DT_ItemData` → `Weapon_Sword_01`：

| 字段 | 值 |
|---|---|
| `Icon` | `T_UI_Icon_Sword` |
| `WeaponParams.AttackMontage` | `/Game/Characters/Player/AM_Sword_Slash` |
| `WeaponParams.WeaponActorClass` | `/Game/Actors/BP_Weapon_Sword` |
| `Damage` / `Range` / `AttackAngle` / `AttackCooldown` / `bAutomatic` | 沿用原测试值 20 / 180 / 100 / 1.35 / false |

步枪行未改：`WeaponActorClass` 与 `AttackMontage` 等枪的资产做出来再填。
