import bpy, sys
from mathutils import Vector
for path in sys.argv[1:]:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=path)
    bpy.context.view_layer.update()
    dg = bpy.context.evaluated_depsgraph_get()
    lo = [1e9]*3; hi = [-1e9]*3; n = 0
    for o in bpy.data.objects:
        if o.type != 'MESH': continue
        m = o.matrix_world
        for v in o.data.vertices:
            w = m @ v.co; n += 1
            for i in range(3): lo[i] = min(lo[i], w[i]); hi[i] = max(hi[i], w[i])
    print("VBB", path.split('/')[-1], n, [round(x,2) for x in lo], [round(x,2) for x in hi])
