# Blender: the firefly glb with the photo textures (textures/*), saved as firefly.blend;
# optional preview renders (top, side, below, 3/4).
import bpy, sys, os, math
argv = sys.argv[sys.argv.index('--') + 1:]
src, texdir, out_blend, render_dir = argv[0], argv[1], argv[2], (argv[3] if len(argv) > 3 else '')
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=src)
# the helper sphere of the source file is not part of the model
for o in list(bpy.data.objects):
    if o.name.startswith('Icosphere'):
        bpy.data.objects.remove(o)

SETS = {'M_Elytra': 'elytra', 'M_Pronotum': 'pronotum', 'M_Body': 'body', 'M_Lantern': 'lantern',
        'M_Eye': 'eye', 'M_Legs': 'legs', 'M_Antenna': 'antenna'}

def img(name, noncolor):
    for e in ('jpg', 'png'):
        p = os.path.join(texdir, f'{name}.{e}')
        if os.path.exists(p):
            im = bpy.data.images.load(p, check_existing=True)
            if noncolor:
                im.colorspace_settings.name = 'Non-Color'
            return im
    return None

def tex(nt, image, x, y):
    n = nt.nodes.new('ShaderNodeTexImage'); n.image = image; n.location = (x, y)
    return n

for mat in bpy.data.materials:
    base = SETS.get(mat.name)
    if not base or not mat.use_nodes:
        continue
    nt = mat.node_tree
    bsdf = next(n for n in nt.nodes if n.type == 'BSDF_PRINCIPLED')
    out = next(n for n in nt.nodes if n.type == 'OUTPUT_MATERIAL')
    for n in list(nt.nodes):
        if n not in (bsdf, out):
            nt.nodes.remove(n)
    c = tex(nt, img(base + '_color', False), -700, 300)
    nt.links.new(c.outputs['Color'], bsdf.inputs['Base Color'])
    # glTF ORM: R occlusion, G roughness, B metallic
    o = tex(nt, img(base + '_orm', True), -700, 0)
    sep = nt.nodes.new('ShaderNodeSeparateColor'); sep.location = (-400, 0)
    nt.links.new(o.outputs['Color'], sep.inputs['Color'])
    nt.links.new(sep.outputs['Green'], bsdf.inputs['Roughness'])
    nt.links.new(sep.outputs['Blue'], bsdf.inputs['Metallic'])
    nrm = tex(nt, img(base + '_normal', True), -700, -300)
    nm = nt.nodes.new('ShaderNodeNormalMap'); nm.location = (-400, -300)
    nt.links.new(nrm.outputs['Color'], nm.inputs['Color'])
    nt.links.new(nm.outputs['Normal'], bsdf.inputs['Normal'])
    if base == 'lantern':
        e = tex(nt, img('lantern_emission', False), -700, -600)
        nt.links.new(e.outputs['Color'], bsdf.inputs['Emission Color'])
        bsdf.inputs['Emission Strength'].default_value = 2.0
    else:
        bsdf.inputs['Emission Strength'].default_value = 0.0
    if base in ('elytra', 'pronotum'):
        bsdf.inputs['Coat Weight'].default_value = 0.25 # the thin waxy sheen of the chitin
        bsdf.inputs['Coat Roughness'].default_value = 0.35
    if base == 'pronotum':
        bsdf.inputs['Subsurface Weight'].default_value = 0.15

for im in bpy.data.images:
    if im.filepath:
        im.filepath = bpy.path.relpath(im.filepath, start=os.path.dirname(out_blend))
bpy.ops.wm.save_as_mainfile(filepath=out_blend, relative_remap=True)
print('saved', out_blend)

if render_dir:
    sc = bpy.context.scene
    sc.render.engine = 'BLENDER_EEVEE'
    sc.render.resolution_x = 900; sc.render.resolution_y = 900
    w = bpy.data.worlds.new('W'); sc.world = w; w.use_nodes = True
    w.node_tree.nodes['Background'].inputs['Color'].default_value = (0.02, 0.02, 0.025, 1)
    for i, (rot, energy) in enumerate([((math.radians(40), 0, math.radians(30)), 3.0), ((math.radians(-130), 0, math.radians(200)), 2.5)]):
        l = bpy.data.lights.new(f'L{i}', 'SUN'); l.energy = energy
        lo = bpy.data.objects.new(f'L{i}', l); lo.rotation_euler = rot; sc.collection.objects.link(lo)
    cam = bpy.data.cameras.new('C'); cam.type = 'ORTHO'; cam.ortho_scale = 1.25
    co = bpy.data.objects.new('C', cam); sc.collection.objects.link(co); sc.camera = co
    views = {'top': ((0, 0, 3), (0, 0, 0)), 'below': ((0, 0, -3), (math.pi, 0, 0)),
             'side': ((3, 0, 0.05), (math.pi / 2, 0, math.pi / 2)),
             'three4': ((1.8, -1.6, 1.4), (math.radians(60), 0, math.radians(48)))}
    for name, (loc, rot) in views.items():
        co.location = loc; co.rotation_euler = rot
        if name == 'three4':
            cam.ortho_scale = 1.1
        sc.render.filepath = os.path.join(render_dir, f'r_{name}.png')
        bpy.ops.render.render(write_still=True)
