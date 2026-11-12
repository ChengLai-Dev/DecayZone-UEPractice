r"""导入 SM_ExtractionPlatform_01 到 DecayZone 工程。

编辑器必须已关闭。调用：
  & "D:\Code\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
    "D:\Code\UE Projects\DecayZone\DecayZone.uproject" `
    -run=pythonscript -script="<this>" -unattended -nopause -nosplash

只操作 /Game/Assets/Props/ExtractionPlatform，不触碰任何已有资产。
"""
import datetime
import os

import unreal

SRC = r"D:\Code\UE Projects\DecayZone\Content\Assets\_Source\ExtractionPlatform\SM_ExtractionPlatform_01.glb"
NAME = "SM_ExtractionPlatform_01"
DEST = "/Game/Assets/Props/ExtractionPlatform"
LOG = r"D:\Code\UE Projects\DecayZone\Content\Assets\_Source\ExtractionPlatform\ue_import.log"
EXPECT_CM = (600.0, 600.0, 30.0)

RENAME = {
    "Image_0": "T_ExtractionPlatform_01_BC",
    "Image_1": "T_ExtractionPlatform_01_N",
    "Image_2": "T_ExtractionPlatform_01_ORM",
    "Material_0": "M_ExtractionPlatform_01",
    "Material_1": "M_ExtractionPlatform_01_Rim",
}

lines = []


def log(msg):
    line = "%s %s" % (datetime.datetime.now().strftime("%H:%M:%S"), msg)
    print(line, flush=True)
    lines.append(line)
    with open(LOG, "a", encoding="utf-8") as fh:
        fh.write(line + "\n")


def main():
    log("=" * 70)
    log("导入 %s" % SRC)
    if not os.path.isfile(SRC):
        log("!!! 源文件不存在")
        return 2

    if unreal.EditorAssetLibrary.does_directory_exist(DEST):
        log("清空旧目录 %s（本批资产尚无引用）" % DEST)
        unreal.EditorAssetLibrary.delete_directory(DEST)

    task = unreal.AssetImportTask()
    task.filename = SRC
    task.destination_path = DEST
    task.destination_name = NAME
    task.automated = True
    task.replace_existing = True
    task.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    paths = [str(p) for p in task.imported_object_paths]
    log("产物 %d 个:" % len(paths))
    for p in paths:
        log("  " + p)

    for path in paths:
        folder, _, asset_name = path.rpartition("/")
        if folder == DEST:
            continue
        target = "%s/%s" % (DEST, asset_name)
        if unreal.EditorAssetLibrary.does_asset_exist(target):
            continue
        if unreal.EditorAssetLibrary.rename_asset(path, target):
            log("  平铺 %s -> %s" % (asset_name, target))

    for old, new in RENAME.items():
        src = "%s/%s" % (DEST, old)
        dst = "%s/%s" % (DEST, new)
        if unreal.EditorAssetLibrary.does_asset_exist(src) and not unreal.EditorAssetLibrary.does_asset_exist(dst):
            unreal.EditorAssetLibrary.rename_asset(src, dst)
            log("  改名 %s -> %s" % (old, new))

    # 贴图采样设置
    settings = {
        "T_ExtractionPlatform_01_BC": (unreal.TextureCompressionSettings.TC_DEFAULT, True),
        "T_ExtractionPlatform_01_N": (unreal.TextureCompressionSettings.TC_NORMALMAP, False),
        "T_ExtractionPlatform_01_ORM": (unreal.TextureCompressionSettings.TC_MASKS, False),
    }
    for tex_name, (comp, srgb) in settings.items():
        path = "%s/%s" % (DEST, tex_name)
        if not unreal.EditorAssetLibrary.does_asset_exist(path):
            log("  !!! 缺贴图 %s" % tex_name)
            continue
        tex = unreal.EditorAssetLibrary.load_asset(path)
        tex.set_editor_property("compression_settings", comp)
        tex.set_editor_property("srgb", srgb)
        tex.set_editor_property("mip_gen_settings", unreal.TextureMipGenSettings.TMGS_FROM_TEXTURE_GROUP)
        unreal.EditorAssetLibrary.save_asset(path)
        log("  贴图 %s compression=%s sRGB=%s" % (tex_name, comp, srgb))

    mesh_path = "%s/%s" % (DEST, NAME)
    if not unreal.EditorAssetLibrary.does_asset_exist(mesh_path):
        log("!!! 找不到网格 %s" % mesh_path)
        return 2

    mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
    box = mesh.get_bounding_box()
    ext = box.max - box.min
    log("包围盒 cm X=%.1f Y=%.1f Z=%.1f (预期 %.0f/%.0f/%.0f)" % (
        ext.x, ext.y, ext.z, EXPECT_CM[0], EXPECT_CM[1], EXPECT_CM[2]))
    if abs(ext.x - EXPECT_CM[0]) > 2 or abs(ext.z - EXPECT_CM[2]) > 2:
        log("!!! 尺寸偏差超阈值")
    log("原点/包围盒 min Z=%.2f (期望 0)" % box.min.z)

    for i, slot in enumerate(mesh.get_editor_property("static_materials")):
        mi = slot.get_editor_property("material_interface")
        log("  材质槽 %d -> %s" % (i, mi.get_path_name() if mi else "!!! None"))

    ns = mesh.get_editor_property("nanite_settings")
    if not ns.enabled:
        ns.enabled = True
        mesh.set_editor_property("nanite_settings", ns)
        log("Nanite 已开启")
    log("Nanite=%s 面数(LOD0)=%d" % (ns.enabled, mesh.get_num_triangles(0)))

    unreal.EditorAssetLibrary.save_directory(DEST, only_if_is_dirty=False, recursive=True)
    log("完成")
    return 0


try:
    CODE = main()
except Exception:
    import traceback
    log("异常:\n" + traceback.format_exc())
    CODE = 3

raise SystemExit(CODE)
