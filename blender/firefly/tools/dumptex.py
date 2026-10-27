import json,struct,sys
p=sys.argv[1]; out=sys.argv[2]
b=open(p,'rb').read()
l=struct.unpack('<I',b[12:16])[0]
j=json.loads(b[20:20+l])
binoff=20+l+8
for i,im in enumerate(j['images']):
    bv=j['bufferViews'][im['bufferView']]
    data=b[binoff+bv.get('byteOffset',0):binoff+bv.get('byteOffset',0)+bv['byteLength']]
    ext='jpg' if im['mimeType']=='image/jpeg' else 'png'
    open(f"{out}/img{i:02d}.{ext}",'wb').write(data)
for i,t in enumerate(j['textures']): print('tex',i,'->img',t['source'])
