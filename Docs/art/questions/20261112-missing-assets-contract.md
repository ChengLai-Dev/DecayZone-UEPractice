# 问题：技术契约 `docs/contracts/assets.md` 在本仓库里不存在

提问方：artist
日期：2026-11-12
背景：本批要做撤离点平台 `SM_ExtractionPlatform_01`。按流程先读技术契约确认尺寸/格式/透明通道/锚点/朝向基准/命名/图集切分，但仓库里没有 `docs/contracts/assets.md`（`Docs/` 下只有 `UI/`、`Handoff_2026-11-08.md`、`Progress_2026-11-12.md`、`Weapon_Data.md`）。
阻塞：不阻塞本批。已按①派活里给定的命名与目录、②`Content/Assets/Props/LootChest` 的既有命名惯例（`SM_` / `M_` / `T_*_BC|_N|_ORM`）做完并交付。后续批次如果要引入新类型资产（带多 UV 通道、多 LOD、材质实例与主材质分离等），没有契约就会各自为政。
期望答复：请 programmer 补一份技术契约，至少要定下这几项，artist 才能照做而不靠猜：
1. 静态网格的体积单位与锚点约定（贴地件是否统一「原点在底面中心」）。
2. 贴图命名与通道打包（本项目现状是 R=AO / G=Roughness / B=Metallic）。
3. 材质是「主材质 + 材质实例」还是允许直接用引擎 Interchange 生成的 MI（本批后者，父材质在 `/InterchangeAssets/gltf/Substrate/M_GLTF`）。
4. 图集最大尺寸与纹理组的默认设置。
5. 朝向基准是否要求「正面朝 +X」——本件旋转对称，无此问题，但后续有正反面的道具需要这条。
