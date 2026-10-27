# Blender: imports the firefly glb in its rest pose and dumps per loop: uv, world position,
# world normal and material, per mesh object (for baking photo projections into the UV maps).
import bpy, sys, numpy as np
argv = sys.argv[sys.argv.index('--') + 1:]
src, out = argv[0], argv[1]
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=src)
for a in bpy.data.objects:
    if a.type == 'ARMATURE':
        a.data.pose_position = 'REST'
    if a.animation_data:
        a.animation_data.action = None
bpy.context.view_layer.update()
dg = bpy.context.evaluated_depsgraph_get()
data = {}
for o in bpy.data.objects:
    if o.type != 'MESH':
        continue
    e = o.evaluated_get(dg)
    m = e.to_mesh()
    m.calc_loop_triangles()
    mw = np.array(o.matrix_world)
    co = np.array([v.co for v in m.vertices])
    co = co @ mw[:3, :3].T + mw[:3, 3]
    nrm = np.array([v.normal for v in m.vertices]) @ np.linalg.inv(mw[:3, :3])
    nrm /= np.linalg.norm(nrm, axis=1, keepdims=True) + 1e-9
    uvl = m.uv_layers.active.data
    tris = []
    for t in m.loop_triangles:
        tris.append([(uvl[l].uv[0], uvl[l].uv[1], *co[m.loops[l].vertex_index], *nrm[m.loops[l].vertex_index]) for l in t.loops])
    mat = o.material_slots[0].material.name if o.material_slots else ''
    data[o.name + '|' + mat] = np.array(tris, np.float32)
    print(o.name, mat, len(tris), co.min(0), co.max(0))
np.savez(out, **data)
