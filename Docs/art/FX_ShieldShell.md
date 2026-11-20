# 护盾壳（M5）美术交付

能量护罩风格的护盾外壳素材。给 `GA_Shield` 用，也准备给 `GA_Sprint` 复用同一套材质。
本文只讲**素材与接线**；护盾数值、GA、GE 属于程序侧。

---

## 1. 风格定调

| 项 | 决定 |
|---|---|
| 风格 | 能量护罩：Fresnel 轮廓 + 六边形能量网格 + 自下而上的扫描带 |
| 主色 | 青绿 `#35E0C0`（与能量条同色系），材质参数可改 |
| 混合模式 | Translucent + Unlit（自发光，不吃 Lumen，避免半透明糊成一片灰） |
| 核心约束 | **不遮挡角色**。中心几乎全透，颜色只出现在轮廓、网格线、扫描带上 |

### 为什么这样就不会糊住人

| 部位 | 不透明度预算 |
|---|---|
| 正对身体、正对镜头的一片 | 0.05（几乎看不见） |
| 六边形网格线 | ≈0.18 × 总 Alpha |
| 外轮廓（Fresnel 边缘） | ≈0.55 × 总 Alpha |
| 扫描带峰值 | ≈0.30 × 总 Alpha |
| 受击闪白（Flash=1） | +0.55，瞬时 |

另外球壳**只渲染背面**（材质里按 `TwoSidedSign` 区分：背面满强度、正面乘 0.05），
所以人物永远在壳的前面，任何镜头角度都不会被压糊。

---

## 2. 资产清单

| 资产 | 路径 | 类型 | 说明 |
|---|---|---|---|
| `T_Shield_HexGrid` | `/Game/Assets/FX/Shield/` | Texture2D | 1024²，白线黑底六边形网格。TC_Masks / 无 sRGB / World 组 |
| `T_Shield_Noise` | `/Game/Assets/FX/Shield/` | Texture2D | 1024²，分形值噪声，破盾溶解与亮边用。同上压缩设置 |
| `M_ShieldShell` | `/Game/Assets/FX/Shield/` | Material | 母材质，68 个节点，Translucent + Unlit + TwoSided |
| `MI_ShieldShell` | `/Game/Assets/FX/Shield/` | MaterialInstanceConstant | 参数实例，默认值即为"正常开盾"状态 |
| `SM_ShieldSphere` | `/Game/Assets/FX/Shield/` | StaticMesh | 由 `Engine/BasicShapes/Sphere` 复制，已删碰撞、默认材质指向 MI。本地半径 50cm |

源文件与生成脚本：`Content/Assets/_Source/FX/Shield/`

```
GenerateShieldTextures.py     # 两个贴图的生成脚本，改完直接重跑
T_Shield_HexGrid.png
T_Shield_Noise.png
```

---

## 3. 材质参数

只有四个参数，全部在 `MI_ShieldShell` 上可直接调：

| 参数 | 类型 | 默认 | 作用 |
|---|---|---|---|
| `Color` | Vector | (0.06, 0.78, 0.62) | 主色，青绿。改色只动这一个 |
| `Alpha` | Scalar | 1.0 | 总强度。0 = 完全隐形，1 = 正常 |
| `Flash` | Scalar | 0.0 | 受击闪白。0→1 瞬间拉高再回落 |
| `Dissolve` | Scalar | 0.0 | 破盾溶解。0→1 烧掉外壳并露出亮边 |

`HexGrid` / `NoiseMask` 也是参数（贴图），一般不用改。

`T_Shield_Noise` 的亮度下限是 0.15（生成脚本里的 `NOISE_FLOOR`）。这是刻意的：
材质用 `saturate((Noise - Dissolve) × 12)` 做不透明遮罩，噪声若压到 0，未溶解时外壳就会出现透明破洞。
要改噪声形态就改 `GenerateShieldTextures.py` 再重跑，但别把下限降到 0.09 以下。

内部常数想调时看这里（都在 `M_ShieldShell` 里，改完记得 Recompile）：
扫描带间距 `s1.ConstB = 0.008`（越小间距越大）、扫描速度 `s2.ConstB = 0.18`、
呼吸速度 `br1.ConstB = 0.22`、网格线强度 `o2.ConstB = 0.18`、轮廓强度 `o1.ConstB = 0.55`、
正面不透明度 `c_face_min.R = 0.05`。

---

## 4. 接线（程序侧，约 5 个节点）

素材已就绪，但**没有挂到任何角色上**。需要在校色角色上做三件事：

### 4.1 加组件

在 `BP_DZ_PlayerCharacter` 里加一个 `Static Mesh` 组件：

| 属性 | 值 |
|---|---|
| Static Mesh | `SM_ShieldSphere` |
| 挂到 | Mesh 的 `pelvis`（或根组件，位置对齐胶囊中心） |
| Relative Location | Z = 角色胶囊半高（约 90） |
| Scale | `(1.15, 1.15, 1.9)` —— 横向包住胶囊，纵向盖住整个人 |
| Collision | No Collision |
| Cast Shadow | 关 |
| Visible | **默认关闭**（`Hidden in Game` / `Set Visibility(false)`） |
| Translucency Sort Priority | 8（材质里已设，组件上不必重复） |

缩放的含义：网格本地半径 50cm，×1.15 = 57.5cm 横向、×1.9 = 95cm 纵向半高。
角色胶囊半径约 34、半高 88，所以壳比身体外扩 20cm 左右——这个余量是"不压脸"的关键，别缩到贴着身体。

### 4.2 按属性驱动显隐

客户端读复制过来的 `Shield` 属性即可（`Shield` 已 `Replicated` + RepNotify），不要自己造数据：

```
Shield > 0  → SetVisibility(true)
Shield == 0 → 播破盾（Dissolve 0→1，0.4s）后 SetVisibility(false)
```

取的入口：PlayerState 上 ASC 的 `UDZ_AttributeSet::ShieldAttribute`，跟
`UDZ_StatusBarsController` 读 Health/Energy 是同一个来源。

### 4.3 出现与消失（L2/L3）

出现（施放瞬间）：

```
SetVisibility(true)
Scale  : 1.15 → 1.38 → 1.15（0.2s 内，带一点回弹）
Alpha  : 0 → 1（0.2s）
```

到时消失（8 秒结束）：`Flash` 拉到 1 持续 0.1s，然后 `Alpha` 1→0 用 0.3s，再关闭可见。

以上三条都用 Timeline 或手动插值即可，没有必走 Niagara 的部分。

---

## 5. 已知限制

1. **极点会有六边形挤压**。球壳用等距柱状 UV，头顶和脚下是极点，网格会收拢。目前靠 Fresnel 边缘和低不透明度遮掉了大部分，正常第三人称视角看不出来。要彻底解决得改三平面投影，成本高，不建议现在做。
2. **UV 接缝在角色背后**。网格不是可平铺贴图，是按等距柱状铺满一次，接缝落在球体背面（角色背后），埋头时可能看到一条错位线。
3. **两人同时开盾会互相穿插**。半透明排序的固有行为，不接受就得改成只有自己可见，会损失信息量。
4. **没有破盾 Niagara**。`Dissolve` 参数和溶解亮边已经做好，只差一个向外的冲击波粒子，本轮没做。
5. **没有受击命中的局部亮点**。需要把伤害命中点传到表现层（现在 `IncomingDamage` 不带位置），本轮砍掉，用整体 `Flash` 代替。
6. **材质没做半透明排序优化**。单人开盾无感，四人同屏时理论上会有排序抖动，届时再考虑 `Translucency Sort Policy`。

---

## 6. 验收步骤

1. 内容浏览器打开 `/Game/Assets/FX/Shield/MI_ShieldShell`，在材质预览球上确认：青绿六边形壳、能看到两条横向扫描带、整体透。
2. 按 4.1 挂好组件但不驱动，手动把 `Hidden in Game` 关掉，PIE 里看静态效果：**必须能看清角色脸部、武器、背包轮廓**。
3. 角色站到暗处、亮墙、霓虹三种背景各转一圈，确认都不糊。
4. 接上 4.2 后按 1 开盾：出现有 0.2s 展开，8 秒后闪白消失。
5. 开盾时挨打：确认壳整体闪一下（`Flash`），不遮住角色。
6. 双人 PIE：A 开盾，B 视角里也能看到同一套表现。

---

## 7. 下一步

- 破盾冲击波（Niagara burst，一个环 + 少量碎片），依赖 4.2 里"Shield 归零"事件
- 受击局部亮点，依赖代码把命中位置传出来
- `GA_Sprint` 出来后复用同一材质，只换 `Color` 到琥珀色、`Alpha` 略低，避免和护盾撞色
