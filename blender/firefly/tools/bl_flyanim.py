# Blender: rebuilds the "Fly-loop" action of the firefly (the open .blend, saved in place).
# Only the hind wings beat fast (WING_HZ); the body, abdomen, elytra, legs and antennae move slowly
# (one or two cycles per loop). Pose after a photo of a flying firefly: elytra raised and spread
# obliquely up, hind wings about horizontal under them, the abdomen hanging down, the head forward.
#   blender -b firefly.blend -P bl_flyanim.py
import bpy, math
from mathutils import Quaternion, Matrix, Vector

FPS = 24
LOOP_S = 2.0 # length of the loop - every motion has a whole number of cycles in it
STEP = 0.25 # frames between keys (the wing beat is 2 frames long)
WING_HZ = 12.0

BODY_PITCH = 12.0 # degrees nose up (was ~27) - the head looks forward
ABD_BEND = (18.0, 15.0, 12.0) # Abd1-3, degrees down (was ~6, 5, 4)
# the elytra turn about their hinge at the body (the pivot): opened about the long axis (over ~90 they
# would turn upside down), the rear raised and swung out to the side
ELYTRA_OPEN = 50.0 # degrees about the opening axis (was ~51)
ELYTRA_LIFT = 18.0 # degrees, the rear raised
ELYTRA_SPREAD = 35.0 # degrees, the rear swung out
WING_STROKE = 28.0 # +- degrees up/down around WING_CENTER
WING_CENTER = -5.0 # degrees, a bit under horizontal (under the raised elytra)
WING_SWEEP = 10.0 # +- degrees forward/back
WING_TWIST = 18.0 # +- degrees about the wing's long axis

def deg(a):
    return math.radians(a)

def qa(axis, a_deg):
    return Quaternion(Vector(axis).normalized(), deg(a_deg))

def wave(t, hz, phase=0.0):
    return math.sin(2 * math.pi * (hz * t + phase))

act = bpy.data.actions['Fly-loop']
arm = bpy.data.objects['Armature']
users = {o.animation_data.action_slot.identifier: o for o in bpy.data.objects
         if o.animation_data and o.animation_data.action == act and o.animation_data.action_slot}
bags = {cb.slot.identifier: cb for layer in act.layers for strip in layer.strips for cb in strip.channelbags}

def curves(slot, path):
    return sorted([fc for fc in bags[slot].fcurves if fc.data_path == path], key=lambda fc: fc.array_index)

def mean_quat(slot, path):
    fcs = curves(slot, path)
    s = [sum(fc.evaluate(f / 10) for f in range(int(act.frame_range[1] * 10) + 1)) for fc in fcs]
    return Quaternion(s).normalized()

def mean_vec(slot, path):
    fcs = curves(slot, path)
    n = int(act.frame_range[1] * 10) + 1
    return Vector([sum(fc.evaluate(f / 10) for f in range(n)) / n for fc in fcs])

# the old opening axis of the elytra and the rest poses of legs / antennae (their means over the loop)
el_old = mean_quat('OBElytraPivot_L', 'rotation_quaternion')
el_axis = el_old.axis
bone_names = [b.name for b in arm.data.bones]
bone_mean = {}
for b in bone_names:
    path = f'pose.bones["{b}"].rotation_quaternion'
    if curves('OBArmature', path):
        bone_mean[b] = mean_quat('OBArmature', path)
rig_loc0 = mean_vec('OBRig', 'location')

times = [i * STEP / FPS for i in range(int(LOOP_S * FPS / STEP) + 1)]
samples = {} # (slot, path) -> list of value tuples

def put(slot, path, value):
    samples.setdefault((slot, path), []).append(tuple(value))

thorax_rest = arm.data.bones['Thorax'].matrix_local
for t in times:
    # body: slow pitch / roll / yaw and bob
    pitch = BODY_PITCH + 2.0 * wave(t, 0.5)
    roll = 2.5 * wave(t, 0.5, 0.25)
    yaw = 1.5 * wave(t, 0.5, 0.6)
    q_body = qa((0, 0, 1), yaw) @ qa((0, 1, 0), roll) @ qa((1, 0, 0), pitch)
    loc = rig_loc0 + Vector((0.006 * wave(t, 0.5, 0.1), 0.004 * wave(t, 1.0, 0.3), 0.012 * wave(t, 1.0)))
    put('OBRig', 'rotation_quaternion', q_body)
    put('OBRig', 'location', loc)
    # the thorax bone follows the rigid parts exactly: pose = rest^-1 * body * rest
    m = thorax_rest.inverted() @ (Matrix.Translation(loc) @ q_body.to_matrix().to_4x4()) @ thorax_rest
    put('OBArmature', 'pose.bones["Thorax"].rotation_quaternion', m.to_quaternion())
    put('OBArmature', 'pose.bones["Thorax"].location', m.to_translation())
    # abdomen hanging down, slowly swinging (each segment a bit later)
    for i, b in enumerate(('Abd1', 'Abd2', 'Abd3')):
        bend = ABD_BEND[i] + 2.5 * wave(t, 0.5, 0.15 + 0.08 * i)
        side = 1.5 * wave(t, 0.5, 0.4 + 0.08 * i)
        put('OBArmature', f'pose.bones["{b}"].rotation_quaternion', qa((0, 1, 0), side) @ qa((1, 0, 0), bend))
    # legs hang loosely: the old mean pose, slowly swaying, every leg in its own phase
    for k, b in enumerate(n for n in bone_names if n.startswith('Leg_')):
        q = bone_mean[b] @ qa((1, 0, 0), 4.0 * wave(t, 0.5, 0.11 * k)) @ qa((0, 0, 1), 2.0 * wave(t, 0.5, 0.3 + 0.07 * k))
        put('OBArmature', f'pose.bones["{b}"].rotation_quaternion', q)
    for k, b in enumerate(n for n in bone_names if n.startswith('Antenna_')):
        q = bone_mean[b] @ qa((1, 0, 0), 3.0 * wave(t, 1.0, 0.13 * k)) @ qa((0, 0, 1), 2.0 * wave(t, 0.5, 0.2 * k))
        put('OBArmature', f'pose.bones["{b}"].rotation_quaternion', q)
    # elytra: wide open and raised, a slow small flutter
    o = ELYTRA_OPEN + 3.0 * wave(t, 1.0)
    el_l = qa((0, 0, 1), -ELYTRA_SPREAD) @ qa((1, 0, 0), -ELYTRA_LIFT) @ Quaternion(el_axis, deg(o))
    put('OBElytraPivot_L', 'rotation_quaternion', el_l)
    put('OBElytraPivot_R', 'rotation_quaternion', Quaternion((el_l.w, el_l.x, -el_l.y, -el_l.z)))
    # hind wings: fast beat around horizontal (rotation 0 = straight out to the side)
    s = WING_CENTER + WING_STROKE * wave(t, WING_HZ)
    sw = WING_SWEEP * wave(t, WING_HZ, 0.25)
    tw = WING_TWIST * wave(t, WING_HZ, 0.25)
    w_l = qa((0, 0, 1), sw) @ qa((0, 1, 0), s) @ qa((1, 0, 0), tw)
    put('OBWingPivot_L', 'rotation_quaternion', w_l)
    put('OBWingPivot_R', 'rotation_quaternion', Quaternion((w_l.w, w_l.x, -w_l.y, -w_l.z)))

# keep the quaternions continuous (no sign flips between keys)
for key, vals in samples.items():
    if len(vals[0]) == 4:
        for i in range(1, len(vals)):
            if sum(a * b for a, b in zip(vals[i], vals[i - 1])) < 0:
                vals[i] = tuple(-v for v in vals[i])

frames = [t * FPS for t in times]
for (slot, path), vals in samples.items():
    fcs = curves(slot, path)
    if not fcs:
        fcs = [bags[slot].fcurves.new(path, index=i) for i in range(len(vals[0]))]
    for fc in fcs:
        fc.keyframe_points.clear()
        fc.keyframe_points.add(len(frames))
        for k, (f, v) in enumerate(zip(frames, vals)):
            p = fc.keyframe_points[k]
            p.co = (f, v[fc.array_index])
            p.interpolation = 'LINEAR'
        fc.update()
act.use_frame_range = True
act.frame_start, act.frame_end = 0, LOOP_S * FPS
bpy.context.scene.frame_end = int(LOOP_S * FPS)
bpy.ops.wm.save_mainfile()
print('Fly-loop rebuilt:', len(frames), 'keys per curve,', LOOP_S, 's')
