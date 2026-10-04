# usage: python decimate.py IN OUT TARGET_TRIS
import bpy, sys
src, dst, target = sys.argv[1], sys.argv[2], int(sys.argv[3])
if src.endswith('.blend'):
    bpy.ops.wm.open_mainfile(filepath=src)
else:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=src)
def tris(o): return sum(len(p.vertices) - 2 for p in o.data.polygons)
meshes = [o for o in bpy.data.objects if o.type == 'MESH']
# shared mesh data counts once
datas = {}
for o in meshes: datas.setdefault(o.data.name, o)
total = sum(tris(o) for o in datas.values())
small = sum(tris(o) for o in datas.values() if tris(o) < 2000)
ratio = max(0.01, min(1.0, (target - small) / max(1, total - small)))
print("total", total, "ratio", ratio)
if bpy.context.object and bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')
for o in datas.values():
    if tris(o) < 2000 or ratio >= 1.0: continue
    if o.data.shape_keys: print("skip shape keys", o.name); continue
    bpy.context.view_layer.objects.active = o
    for x in bpy.context.view_layer.objects: x.select_set(False)
    o.select_set(True)
    m = o.modifiers.new("Decimate", 'DECIMATE')
    m.decimate_type = 'COLLAPSE'
    m.ratio = ratio
    m.use_collapse_triangulate = True
    # keep it first, before an armature modifier
    while o.modifiers.find("Decimate") > 0:
        bpy.ops.object.modifier_move_up(modifier="Decimate")
    bpy.ops.object.modifier_apply(modifier="Decimate")
print("after", sum(tris(o) for o in {o.data.name: o for o in meshes}.values()))
if '.blend' in dst:
    bpy.ops.wm.save_as_mainfile(filepath=dst, compress=True, relative_remap=True, copy=True)
else:
    bpy.ops.export_scene.gltf(filepath=dst, export_format='GLB', export_yup=True, export_apply=False,
        export_animations=True, export_skins=True, export_morph=True, export_extras=True, export_cameras=False, export_lights=False)
