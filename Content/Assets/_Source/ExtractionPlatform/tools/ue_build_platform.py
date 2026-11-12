r"""Import SM_ExtractionPlatform_01 into DecayZone and build its material.

Steps
  1. wipe the previous (procedural) leftovers in the target folder - they are untracked
     scratch from an earlier attempt at the same asset, nothing references them
  2. import the glb (mesh + embedded textures + glTF material)
  3. flatten the glTF subfolders into the target folder
  4. rename textures by their actual material role (sampler type), not by guess
  5. build M_ExtractionPlatform_01: BC/ORM/N wired, plus an emissive mask that picks
     the teal rim out of the base colour (saturate((G+B)/2 - R))
  6. assign the material, enable Nanite, save

Run (editor closed):
  UnrealEditor-Cmd.exe DecayZone.uproject -run=pythonscript -script=<this> -unattended -nopause -nosplash
"""
import datetime
import os
import traceback

import unreal

SRC = r"D:\Code\temp\extractionplatform\SM_ExtractionPlatform_01.glb"
DEST = "/Game/Assets/Props/ExtractionPlatform"
NAME = "SM_ExtractionPlatform_01"
LOG = r"D:\Code\temp\extractionplatform\ue_import_platform.log"

mel = unreal.MaterialEditingLibrary


def log(msg):
    line = "%s %s" % (datetime.datetime.now().strftime("%H:%M:%S"), msg)
    print(line, flush=True)
    with open(LOG, "a", encoding="utf-8") as f:
        f.write(line + "\n")


def list_assets():
    return unreal.EditorAssetLibrary.list_assets(DEST, recursive=True, include_folder=False)


def wipe():
    if unreal.EditorAssetLibrary.does_directory_exist(DEST):
        names = [str(p) for p in list_assets()]
        log("清理旧资产 %d 个: %s" % (len(names), names))
        unreal.EditorAssetLibrary.delete_directory(DEST)
    unreal.EditorAssetLibrary.make_directory(DEST)


def import_mesh():
    task = unreal.AssetImportTask()
    task.filename = SRC
    task.destination_path = DEST
    task.destination_name = NAME
    task.automated = True
    task.replace_existing = True
    task.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    paths = [str(p) for p in task.imported_object_paths]
    log("导入产物 %d 个" % len(paths))
    for p in paths:
        log("  %s" % p)
    return paths


def flatten():
    for path in list_assets():
        folder, _, asset_name = path.rpartition("/")
        if folder == DEST:
            continue
        target = "%s/%s" % (DEST, asset_name)
        if unreal.EditorAssetLibrary.does_asset_exist(target):
            log("跳过重命名（目标已存在）: %s" % target)
            continue
        if unreal.EditorAssetLibrary.rename_asset(path, target):
            log("平铺 %s -> %s" % (path, target))
        else:
            log("!!! 角色重命名失败 %s" % path)
    # drop the now empty glTF subfolders
    for folder in [str(p)[:-1] for p in unreal.EditorAssetLibrary.list_assets(DEST, recursive=True, include_folder=True)]:
        if str(folder).endswith("/") and folder != DEST:
            if not unreal.EditorAssetLibrary.list_assets(folder, recursive=False, include_folder=False):
                unreal.EditorAssetLibrary.delete_directory(folder)


def texture_role(expr):
    st = str(expr.get_editor_property("sampler_type"))
    if "NORMAL" in st:
        return "N"
    if "MASK" in st or "LINEAR" in st:
        return "ORM"
    return "BC"


def collect_material_texture_roles(mat_path):
    """Map texture name -> sampler role, reading the glTF material or its instance parent."""
    roles = {}
    mat = unreal.EditorAssetLibrary.load_asset(mat_path)
    if mat is None:
        return roles
    if isinstance(mat, unreal.MaterialInstanceConstant):
        mat = mat.get_editor_property("parent")
        if mat is None:
            return roles
    if not isinstance(mat, unreal.Material):
        log("材质类型不是 Material: %s" % type(mat))
        return roles
    for expr in mel.get_material_expressions(mat):
        if isinstance(expr, unreal.MaterialExpressionTextureSample):
            tex = expr.get_editor_property("texture")
            if tex:
                roles[str(tex.get_name())] = (texture_role(expr), str(expr.get_editor_property("sampler_type")))
    return roles


def rename_textures():
    mapping = collect_material_texture_roles("%s/%s_material_0" % (DEST, NAME))
    log("材质里的贴图角色: %s" % mapping)

    # the glTF importer keeps the image order of the source file
    # (0 = base colour, 1 = metallic-roughness/occlusion, 2 = normal)
    by_index = {"_0": "BC", "_1": "ORM", "_2": "N"}

    renamed = {}
    for path in list_assets():
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not isinstance(asset, unreal.Texture2D):
            continue
        short = str(asset.get_name())
        role, st = mapping.get(short, (None, "unknown"))
        if role is None:
            for suffix, guess in by_index.items():
                if short.endswith(suffix):
                    role = guess
                    st = "by-image-index"
                    break
        if role is None:
            log("!!! 无法判定贴图用途，跳过: %s" % short)
            continue
        new_name = "T_ExtractionPlatform_01_%s" % role
        target = "%s/%s" % (DEST, new_name)
        if unreal.EditorAssetLibrary.does_asset_exist(target):
            log("!!! 重名 %s 已存在，跳过 %s" % (target, short))
            continue
        if unreal.EditorAssetLibrary.rename_asset(path, target):
            renamed[role] = target
            log("贴图 %s -> %s   (sampler %s)" % (short, new_name, st))
    return renamed


def set_texture_settings(textures):
    for role, path in textures.items():
        tex = unreal.EditorAssetLibrary.load_asset(path)
        if role == "BC":
            tex.set_editor_property("srgb", True)
            tex.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_DEFAULT)
        elif role == "ORM":
            tex.set_editor_property("srgb", False)
            tex.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_MASKS)
        elif role == "N":
            tex.set_editor_property("srgb", False)
            tex.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
        unreal.EditorAssetLibrary.save_asset(path)
        log("贴图设置 %s: srgb=%s compression=%s" % (role,
                                                 tex.get_editor_property("srgb"),
                                                 tex.get_editor_property("compression_settings")))


def make_texture_sample(mat, tex, prop_name, sampler, x, y):
    node = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D, x, y)
    node.set_editor_property("texture", tex)
    node.set_editor_property("sampler_type", sampler)
    node.set_editor_property("parameter_name", prop_name)
    return node


def build_material(textures):
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    path = "%s/M_ExtractionPlatform_01" % DEST
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        unreal.EditorAssetLibrary.delete_asset(path)
    mat = tools.create_asset("M_ExtractionPlatform_01", DEST, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property("two_sided", True)

    bc_tex = unreal.EditorAssetLibrary.load_asset(textures["BC"])
    orm_tex = unreal.EditorAssetLibrary.load_asset(textures["ORM"])
    n_tex = unreal.EditorAssetLibrary.load_asset(textures["N"])

    bc = make_texture_sample(mat, bc_tex, "BaseColor", unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, -1200, -300)
    orm = make_texture_sample(mat, orm_tex, "ORM", unreal.MaterialSamplerType.SAMPLERTYPE_MASKS, -1200, 100)
    nrm = make_texture_sample(mat, n_tex, "Normal", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, -1200, 400)

    # --- emissive: keep only the saturated teal, then push it bright
    mask_r = mel.create_material_expression(mat, unreal.MaterialExpressionComponentMask, -900, 700)
    mask_r.set_editor_property("r", True)
    mask_g = mel.create_material_expression(mat, unreal.MaterialExpressionComponentMask, -900, 800)
    mask_g.set_editor_property("g", True)
    mask_b = mel.create_material_expression(mat, unreal.MaterialExpressionComponentMask, -900, 900)
    mask_b.set_editor_property("b", True)
    for m in (mask_r, mask_g, mask_b):
        mel.connect_material_expressions(bc, "RGB", m, "")

    add_gb = mel.create_material_expression(mat, unreal.MaterialExpressionAdd, -700, 850)
    mel.connect_material_expressions(mask_g, "", add_gb, "A")
    mel.connect_material_expressions(mask_b, "", add_gb, "B")

    half = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -550, 850)
    mel.connect_material_expressions(add_gb, "", half, "A")
    half.set_editor_property("const_b", 0.5)

    diff = mel.create_material_expression(mat, unreal.MaterialExpressionSubtract, -400, 800)
    mel.connect_material_expressions(half, "", diff, "A")
    mel.connect_material_expressions(mask_r, "", diff, "B")

    clamp = mel.create_material_expression(mat, unreal.MaterialExpressionClamp, -250, 800)
    mel.connect_material_expressions(diff, "", clamp, "")

    boost = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -100, 800)
    mel.connect_material_expressions(clamp, "", boost, "A")
    boost.set_editor_property("const_b", 4.0)

    tinted = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, 50, 600)
    mel.connect_material_expressions(bc, "RGB", tinted, "A")
    mel.connect_material_expressions(boost, "", tinted, "B")

    mel.connect_material_property(bc, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
    mel.connect_material_property(orm, "B", unreal.MaterialProperty.MP_METALLIC)
    mel.connect_material_property(orm, "G", unreal.MaterialProperty.MP_ROUGHNESS)
    mel.connect_material_property(orm, "R", unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)
    mel.connect_material_property(nrm, "RGB", unreal.MaterialProperty.MP_NORMAL)
    mel.connect_material_property(tinted, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

    mel.recompile_material(mat)
    unreal.EditorAssetLibrary.save_asset(path)
    log("材质已建: %s  表达式 %d 个" % (path, len(mel.get_material_expressions(mat))))
    return mat


def assign_material(mat):
    mesh = unreal.EditorAssetLibrary.load_asset("%s/%s" % (DEST, NAME))
    if not mesh:
        log("!!! 找不到网格")
        return False
    msm = unreal.MaterialEditingLibrary
    mesh.set_editor_property("static_materials",
                             [unreal.StaticMaterial(material_interface=mat, material_slot_name="Platform")])
    settings = mesh.get_editor_property("nanite_settings")
    settings.enabled = True
    mesh.set_editor_property("nanite_settings", settings)
    unreal.EditorAssetLibrary.save_asset("%s/%s" % (DEST, NAME))
    slots = mesh.get_editor_property("static_materials")
    for i, slot in enumerate(slots):
        mi = slot.get_editor_property("material_interface")
        log("材质槽 %d -> %s" % (i, mi.get_path_name() if mi else "None"))
    bb = mesh.get_bounding_box()
    log("包围盒 min=%s max=%s" % (bb.min, bb.max))
    log("Nanite=%s" % mesh.get_editor_property("nanite_settings").enabled)

    # drop the glTF-imported material instance, the hand-built material replaces it
    leftover = "%s/%s_material_0" % (DEST, NAME)
    if unreal.EditorAssetLibrary.does_asset_exist(leftover):
        if unreal.EditorAssetLibrary.delete_asset(leftover):
            log("删除 glTF 自带材质实例 %s" % leftover)
        else:
            log("!!! 删除 glTF 材质实例失败 %s" % leftover)
    return True


def main():
    log("=" * 60)
    try:
        wipe()
        import_mesh()
        flatten()
        textures = rename_textures()
        if len(textures) < 3:
            log("!!! 贴图识别不全: %s" % textures)
        set_texture_settings(textures)
        mat = build_material(textures)
        ok = assign_material(mat)
        log("最终资产: %s" % [str(p) for p in list_assets()])
        log("结果: %s" % ("成功" if ok else "失败"))
        return 0 if ok else 2
    except Exception:
        log("失败:\n%s" % traceback.format_exc())
        return 1


raise SystemExit(main())
