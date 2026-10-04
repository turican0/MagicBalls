# usage: glbextimg.py FILE.glb BASENAME - point the embedded images of a Blender-exported GLB at
# the textures Godot extracted from the original (BASENAME_<index>.<ext>, Blender named them
# Image_<index>) and drop the embedded copies from the binary chunk.
import json, struct, sys, os, glob
path, base = sys.argv[1], sys.argv[2]
data = open(path, 'rb').read()
magic, ver, length = struct.unpack_from('<III', data, 0)
assert magic == 0x46546C67
jlen, jtype = struct.unpack_from('<II', data, 12)
gj = json.loads(data[20:20 + jlen])
blen, btype = struct.unpack_from('<II', data, 20 + jlen)
bin_ = data[28 + jlen:28 + jlen + blen]
drop = set()
folder = os.path.dirname(path)
for img in gj.get('images', []):
    name = img.get('name', '')
    assert name.startswith('Image_'), name
    idx = name[len('Image_'):]
    cands = [f for f in glob.glob(os.path.join(folder, glob.escape(base) + '_' + idx + '.*')) if not f.endswith('.import')]
    assert len(cands) == 1, (name, cands)
    drop.add(img['bufferView'])
    for k in ('bufferView', 'mimeType'):
        img.pop(k, None)
    img['uri'] = os.path.basename(cands[0])
# rebuild the binary chunk without the dropped buffer views
remap, views, out = {}, [], bytearray()
for i, v in enumerate(gj['bufferViews']):
    if i in drop:
        continue
    while len(out) % 4: out.append(0)
    chunk = bin_[v.get('byteOffset', 0):v.get('byteOffset', 0) + v['byteLength']]
    v = dict(v); v['byteOffset'] = len(out); out += chunk
    remap[i] = len(views); views.append(v)
gj['bufferViews'] = views
for a in gj.get('accessors', []):
    if 'bufferView' in a: a['bufferView'] = remap[a['bufferView']]
    sp = a.get('sparse')
    if sp:
        sp['indices']['bufferView'] = remap[sp['indices']['bufferView']]
        sp['values']['bufferView'] = remap[sp['values']['bufferView']]
while len(out) % 4: out.append(0)
gj['buffers'][0]['byteLength'] = len(out)
js = json.dumps(gj, separators=(',', ':')).encode()
while len(js) % 4: js += b' '
glb = struct.pack('<III', 0x46546C67, 2, 12 + 8 + len(js) + 8 + len(out)) + struct.pack('<II', len(js), 0x4E4F534A) + js + struct.pack('<II', len(out), 0x004E4942) + bytes(out)
open(path, 'wb').write(glb)
print(path, 'images ->', [i['uri'] for i in gj.get('images', [])], 'bin', blen, '->', len(out))
