"""Inspect a GLB from 6 fixed views (front/side/top/three-quarter) to check shape, thickness, up-axis.

blender -b --factory-startup --python inspect_glb.py -- <glb> <outdir> <name>
"""
import math
import os
import sys

import bpy
from mathutils import Vector

argv = sys.argv
args = argv[argv.index("--") + 1:] if "--" in argv else []
glb, outdir, name = args[0], args[1], args[2]
os.makedirs(outdir, exist_ok=True)

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=glb)

# assets written with glTF Z-up end up standing on edge after import (importer assumes Y-up);
# optional 4th arg "zup" rotates them back so renders show them laid out as authored
if len(args) > 3 and args[3] == "zup":
    for obj in [o for o in bpy.context.scene.objects if o.type == "MESH"]:
        obj.rotation_mode = "XYZ"
        obj.rotation_euler[0] += math.radians(90)
    bpy.context.view_layer.update()

meshes = [o for o in bpy.context.scene.objects if o.type == "MESH"]
coords = []
for o in meshes:
    for corner in o.bound_box:
        coords.append(o.matrix_world @ Vector(corner))
min_v = Vector((min(c.x for c in coords), min(c.y for c in coords), min(c.z for c in coords)))
max_v = Vector((max(c.x for c in coords), max(c.y for c in coords), max(c.z for c in coords)))
center = (min_v + max_v) / 2
size = max(max_v - min_v)
print("BBOX", tuple(round(v, 3) for v in min_v), tuple(round(v, 3) for v in max_v), "size", round(size, 3))

scene = bpy.context.scene
engines = [i.identifier for i in bpy.types.RenderSettings.bl_rna.properties["engine"].enum_items]
scene.render.engine = "BLENDER_EEVEE_NEXT" if "BLENDER_EEVEE_NEXT" in engines else "BLENDER_EEVEE"
scene.render.resolution_x = 900
scene.render.resolution_y = 900
scene.render.resolution_percentage = 100
scene.render.film_transparent = False
scene.view_settings.view_transform = "Standard"
scene.view_settings.look = "None"

world = bpy.data.worlds.new("W")
scene.world = world
world.use_nodes = True
bg = world.node_tree.nodes["Background"]
bg.inputs[0].default_value = (1.0, 1.0, 1.0, 1.0)
bg.inputs[1].default_value = 1.0


def add_sun(rotation, energy, angle_deg):
    data = bpy.data.lights.new("S", type="SUN")
    data.energy = energy
    data.angle = math.radians(angle_deg)
    data.use_shadow = False
    obj = bpy.data.objects.new("S", data)
    obj.rotation_euler = rotation
    scene.collection.objects.link(obj)


add_sun((math.radians(55), 0, math.radians(35)), 1.8, 25)
add_sun((math.radians(60), 0, math.radians(-120)), 0.9, 30)
add_sun((math.radians(110), 0, math.radians(200)), 0.6, 30)

cam_data = bpy.data.cameras.new("cam")
cam_data.angle = math.radians(20.0)
cam = bpy.data.objects.new("cam", cam_data)
scene.collection.objects.link(cam)
scene.camera = cam
half = math.tan(math.radians(20.0) / 2.0)
dist = (size / 2.0) / half * 1.15

# (tag, azimuth deg, elevation deg)
views = (
    ("front", 0, 0),
    ("side", 90, 0),
    ("top", 0, 89),
    ("threequarter", 0, 32),
    ("threequarter_back", 180, 32),
    ("bottom", 0, -55),
)
for tag, azim, elev in views:
    a = math.radians(azim)
    e = math.radians(elev)
    loc = center + Vector((
        math.sin(a) * math.cos(e) * dist,
        -math.cos(a) * math.cos(e) * dist,
        math.sin(e) * dist,
    ))
    cam.location = loc
    cam.rotation_euler = (center - loc).to_track_quat("-Z", "Y").to_euler()
    scene.render.filepath = os.path.join(outdir, "%s_%s.png" % (name, tag))
    bpy.ops.render.render(write_still=True)
    print("RENDERED", scene.render.filepath)
