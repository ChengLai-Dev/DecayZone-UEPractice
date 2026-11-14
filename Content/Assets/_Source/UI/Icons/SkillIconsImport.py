r"""导入技能图标贴图、生成冷却环材质 M_CooldownRing 与 MI_CooldownRing。

编辑器必须已关闭。调用：
  & "D:\Code\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
    "D:\Code\UE Projects\DecayZone\DecayZone.uproject" `
    -run=pythonscript -script="<this>" -unattended -nopause -nosplash

只操作 /Game/Assets/SkillIcons 与 /Game/UI/Materials，不触碰其他资产。
贴图设置从 /Game/Assets/ItemIcons/Runtime/T_UI_Icon_Potion 复制，保证同批图标一致。

M_CooldownRing：UI 域、半透明，标量参数 Percent（0=就绪，1=冷却满），
输出到 Opacity，覆盖「从顶部顺时针、与 Percent 等比例」的扇形暗层。
与项目已有的 M_LootRing 区别：那条是细环描边（Radius/Thickness/Progress），
本条是填充扇形遮罩，两者不能互相替代。
"""
import datetime
import os

import unreal

SRC_DIR = r"D:\Code\UE Projects\DecayZone\Content\Assets\_Source\UI\Icons"
ICON_ROOT = "/Game/Assets/SkillIcons"
ICON_DEST = "/Game/Assets/SkillIcons/Runtime"
MAT_DEST = "/Game/UI/Materials"
MAT_NAME = "M_CooldownRing"
MI_NAME = "MI_CooldownRing"
REFERENCE_ICON = "/Game/Assets/ItemIcons/Runtime/T_UI_Icon_Potion"
LOG = os.path.join(SRC_DIR, "ue_import_skill_icons.log")

ICONS = ["T_UI_Icon_Skill_Shield", "T_UI_Icon_Skill_Sprint"]
TEXTURE_PROPS = ["compression_settings", "srgb", "mip_gen_settings", "lod_group",
                 "never_stream", "filter", "address_x", "address_y"]

HLSL = "\n".join([
    "float2 p = UV - 0.5;",
    "float r = length(p) * 2.0;",
    "float a = atan2(p.x, -p.y) / 6.28318530718;",
    "float ang = frac(a);",
    "float inside = step(r, 1.0);",
    "float covered = step(ang, Percent);",
    "return inside * covered * 0.72;",
])

lines = []


def log(msg):
    line = "%s %s" % (datetime.datetime.now().strftime("%H:%M:%S"), msg)
    print(line, flush=True)
    lines.append(line)
    with open(LOG, "a", encoding="utf-8") as fh:
        fh.write(line + "\n")


def ensure_dir(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
        log("新建目录 %s" % path)


def import_icons():
    tasks = []
    for name in ICONS:
        png = os.path.join(SRC_DIR, name + ".png")
        if not os.path.isfile(png):
            log("!!! 缺源文件 %s" % png)
            return False
        task = unreal.AssetImportTask()
        task.filename = png
        task.destination_path = ICON_DEST
        task.destination_name = name
        task.automated = True
        task.replace_existing = True
        task.save = True
        tasks.append(task)

    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)
    for task in tasks:
        log("导入 %s -> %s" % (task.destination_name, [str(p) for p in task.imported_object_paths]))

    # 早期一版放在了 SkillIcons 根目录，清掉
    for name in ICONS:
        stale = "%s/%s" % (ICON_ROOT, name)
        if unreal.EditorAssetLibrary.does_asset_exist(stale):
            unreal.EditorAssetLibrary.delete_asset(stale)
            log("删除旧位置 %s" % stale)

    if not unreal.EditorAssetLibrary.does_asset_exist(REFERENCE_ICON):
        log("!!! 参考图标不存在，跳过贴图设置复制：%s" % REFERENCE_ICON)
        return False

    ref = unreal.EditorAssetLibrary.load_asset(REFERENCE_ICON)
    for name in ICONS:
        path = "%s/%s" % (ICON_DEST, name)
        if not unreal.EditorAssetLibrary.does_asset_exist(path):
            log("!!! 导入后找不到 %s" % path)
            return False
        tex = unreal.EditorAssetLibrary.load_asset(path)
        copied = []
        for prop in TEXTURE_PROPS:
            try:
                value = ref.get_editor_property(prop)
                tex.set_editor_property(prop, value)
                copied.append("%s=%s" % (prop, value))
            except Exception as exc:
                copied.append("%s 跳过(%s)" % (prop, exc))
        unreal.EditorAssetLibrary.save_asset(path)
        log("贴图设置 %s: %s" % (name, ", ".join(copied)))
    return True


def build_cooldown_material():
    mat_path = "%s/%s" % (MAT_DEST, MAT_NAME)
    if unreal.EditorAssetLibrary.does_asset_exist(mat_path):
        log("材质已存在，删除重建：%s" % mat_path)
        unreal.EditorAssetLibrary.delete_asset(mat_path)

    factory_class = getattr(unreal, "MaterialFactoryNew", None)
    if factory_class is None:
        log("!!! 没有 MaterialFactoryNew")
        return False

    mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        MAT_NAME, MAT_DEST, unreal.Material, factory_class())
    if mat is None:
        log("!!! 材质创建失败")
        return False

    mat.set_editor_property("material_domain", unreal.MaterialDomain.MD_UI)
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    mat.set_editor_property("two_sided", False)

    lib = unreal.MaterialEditingLibrary

    percent = lib.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -780, 160)
    percent.set_editor_property("parameter_name", "Percent")
    percent.set_editor_property("default_value", 0.0)

    uv = lib.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -1000, -220)

    custom = lib.create_material_expression(mat, unreal.MaterialExpressionCustom, -480, -160)
    custom.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT1)
    custom.set_editor_property("description", "CooldownRing")
    custom.set_editor_property("code", HLSL)

    input_uv = unreal.CustomInput()
    input_uv.set_editor_property("input_name", "UV")
    input_percent = unreal.CustomInput()
    input_percent.set_editor_property("input_name", "Percent")
    custom.set_editor_property("inputs", [input_uv, input_percent])

    lib.connect_material_expressions(uv, "", custom, "UV")
    lib.connect_material_expressions(percent, "", custom, "Percent")
    lib.connect_material_property(custom, "", unreal.MaterialProperty.MP_OPACITY)

    lib.recompile_material(mat)
    unreal.EditorAssetLibrary.save_asset(mat_path)
    log("材质已生成 %s（表达式 %d 个）" % (mat_path, lib.get_num_material_expressions(mat)))

    mi_path = "%s/%s" % (MAT_DEST, MI_NAME)
    if unreal.EditorAssetLibrary.does_asset_exist(mi_path):
        unreal.EditorAssetLibrary.delete_asset(mi_path)
    mi_factory = unreal.MaterialInstanceConstantFactoryNew()
    mi = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        MI_NAME, MAT_DEST, unreal.MaterialInstanceConstant, mi_factory)
    if mi is None:
        log("!!! 材质实例创建失败")
        return False
    mi.set_editor_property("parent", mat)
    lib.set_material_instance_scalar_parameter_value(mi, "Percent", 0.0)
    unreal.EditorAssetLibrary.save_asset(mi_path)
    log("材质实例已生成 %s" % mi_path)
    return True


def main():
    log("=" * 70)
    ensure_dir(ICON_ROOT)
    ensure_dir(ICON_DEST)
    ensure_dir(MAT_DEST)

    if not import_icons():
        return 2
    if not build_cooldown_material():
        return 2

    unreal.EditorAssetLibrary.save_directory(ICON_ROOT, only_if_is_dirty=False, recursive=True)
    unreal.EditorAssetLibrary.save_directory(MAT_DEST, only_if_is_dirty=False, recursive=True)
    log("完成")
    return 0


try:
    CODE = main()
except Exception:
    import traceback
    log("异常:\n" + traceback.format_exc())
    CODE = 3

raise SystemExit(CODE)
