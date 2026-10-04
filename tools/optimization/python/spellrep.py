import re,json,csv,glob,os
S=os.environ.get('WORK', '.') # folder with lv2/, spall/ (and lib_analysis.csv)
REPO=os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..')
src=open(os.path.join(REPO, 'godot-code/scenes/decode_level.gd')).read().split('\n')
def parse(a,b):
    d={}
    for l in src[a:b]:
        if l.strip().startswith('#'): continue
        m=re.search(r'Vector3i\((-?\d+),\s*(-?\d+),\s*(-?\d+)\)\s*:\s*"(res://[^"]+)"',l)
        if m: d[(int(m[1]),int(m[2]))]=m[4]
    return d
_a=next(i for i,l in enumerate(src) if l.startswith('var library = {')); _b=next(i for i,l in enumerate(src) if l.startswith('var library2 = {')); _c=next(i for i in range(_b, len(src)) if src[i].startswith('}'))
lib=parse(_a,_b); lib2=parse(_b,_c)
st={}
for r in csv.DictReader(open(S+'/lib_analysis.csv'),delimiter=';'): st[r['path']]=r
names="Fireball Possession Castle SpeedUp Morph Heal Shield Lightning Rebound Meteor Teleport Invisible StealMana BeyondSight Duel Tremor Crater Earthquake Volcano SummonArmy GravityWell Whirlwind FoolsMana MagicMine Alliance CaveIn".split()
rows=[]
for sp in range(26):
  for lv in range(3):
    f=f'{S}/spall/{sp}_{lv}.json'
    d=json.load(open(f)); items=[]
    for k,v in d['max'].items():
        dlt=v-d['first'].get(k,0)
        if dlt<=0: continue
        c,m,dr=map(int,k.split(','))
        if dr==1 and c in (2,3,5,9,10,15): p=lib.get((c,m))
        else: p=lib2.get((c,m))
        p=p or ('(default text)' if dr==1 or True else '')
        s=st.get(p,{})
        items.append((dlt,k,p,int(s.get('tris') or 0),int(s.get('lights') or 0),int(s.get('particles') or 0),float(s.get('tex_mpix') or 0)))
    items.sort(reverse=True)
    rows.append((sp,lv,items))
out=open(S+'/spell_report.txt','w')
for sp,lv,items in rows:
    tot=sum(i[0] for i in items); tris=sum(i[0]*i[3] for i in items); li=sum(i[0]*i[4] for i in items); pa=sum(i[0]*i[5] for i in items)
    out.write(f"{sp:2d} {names[sp]} {'I'*(lv+1)}: +{tot} ent, ~{tris/1000:.0f}k tris, {li} lights, {pa} particle sys\n")
    for i in items[:8]: out.write(f"     +{i[0]:4d} {i[1]:10s} {os.path.basename(i[2])}  tris={i[3]} L={i[4]} P={i[5]}\n")
out.close()
print(open(S+'/spell_report.txt').read())
