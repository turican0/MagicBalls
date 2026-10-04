# usage: python joinmeshes.py IN.glb OUT.glb - join all mesh objects into one (materials stay separate)
import bpy, sys
src, dst = sys.argv[1], sys.argv[2]
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=src)
meshes = [o for o in bpy.data.objects if o.type == 'MESH']
print("meshes", len(meshes), "actions", len(bpy.data.actions), "armatures", len([o for o in bpy.data.objects if o.type == 'ARMATURE']))
mats_before = sorted({m.name for o in meshes for m in o.data.materials if m})
for o in bpy.context.view_layer.objects: o.select_set(False)
for o in meshes:
    if o.data.users > 1:
        o.data = o.data.copy() # linked duplicates must be single-user to be joined
    o.select_set(True)
bpy.context.view_layer.objects.active = meshes[0]
bpy.ops.object.join()
joined = bpy.context.view_layer.objects.active
mats_after = sorted({m.name for m in joined.data.materials if m})
print("joined tris", sum(len(p.vertices) - 2 for p in joined.data.polygons), "materials", len(mats_before), len(mats_after))
assert mats_before == mats_after
# the empties which only held the joined meshes are not needed any more
keep = set()
o = joined
while o:
    keep.add(o.name); o = o.parent
for o in list(bpy.data.objects):
    if o.type == 'EMPTY' and o.name not in keep:
        bpy.data.objects.remove(o, do_unlink=True)
print("objects left", len(bpy.data.objects))
bpy.ops.export_scene.gltf(filepath=dst, export_format='GLB', export_yup=True, export_apply=False, export_animations=True, export_cameras=False, export_lights=False)
