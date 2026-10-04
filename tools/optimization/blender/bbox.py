import bpy, sys
from mathutils import Vector
for path in sys.argv[1:]:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=path)
    lo = Vector((1e9,)*3); hi = Vector((-1e9,)*3)
    for o in bpy.data.objects:
        if o.type != 'MESH': continue
        for c in o.bound_box:
            w = o.matrix_world @ Vector(c)
            lo = Vector(map(min, lo, w)); hi = Vector(map(max, hi, w))
    print("BBOX %-60s %s %s" % (path.split('/')[-1], tuple(round(x,3) for x in lo), tuple(round(x,3) for x in hi)))
