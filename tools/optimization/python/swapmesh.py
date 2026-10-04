# usage: swapmesh.py SCENE.tscn ID=res://path.res ...  - replace ArrayMesh sub-resources by external .res files
import sys, re
p = sys.argv[1]; s = open(p, encoding='utf-8').read()
for arg in sys.argv[2:]:
    sid, res = arg.split('=', 1)
    start = s.index('[sub_resource type="ArrayMesh" id="%s"]' % sid)
    nxt = s.index('\n[', start + 1)
    s = s[:start] + s[nxt + 1:]
    eid = "mesh_" + sid
    s = s.replace('SubResource("%s")' % sid, 'ExtResource("%s")' % eid)
    header_end = s.index('\n') + 1
    # after the last ext_resource, else right after the header
    m = list(re.finditer(r'^\[ext_resource [^\n]*\]\n', s, re.M))
    pos = m[-1].end() if m else header_end + 1
    s = s[:pos] + '[ext_resource type="ArrayMesh" path="%s" id="%s"]\n' % (res, eid) + ('' if m else '\n') + s[pos:]
open(p, 'w', encoding='utf-8').write(s)
