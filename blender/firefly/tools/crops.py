from PIL import Image
D='C:/Users/vesely/Downloads/'
A=Image.open(D+'1f0e15b3-9df0-4556-a564-cf70b186c975.png').convert('RGB')
B=Image.open(D+'1c12e346-22cc-436c-8342-bde02f632529.png').convert('RGB')
C=Image.open(D+'28c31d0c-eb7a-420d-bd80-d1b0eb7c2356.png').convert('RGB')
print(A.size,B.size,C.size)
cr={
 'a_head':(A,(4,336,350,618)),'a_elytra':(A,(364,336,658,618)),'a_lantern':(A,(672,336,944,618)),
 'a_leg':(A,(958,336,1238,618)),'a_ant':(A,(1252,336,1534,618)),
 'b_head':(B,(1102,30,1297,236)),'b_elytra':(B,(1314,30,1509,236)),'b_leg':(B,(1102,281,1297,488)),'b_light':(B,(1314,281,1509,488)),
 'c_head':(C,(2,645,248,985)),'c_elytra':(C,(262,645,478,985)),'c_lantern':(C,(490,645,678,985)),'c_leg':(C,(692,645,850,985)),
 'c_sph_elytra':(C,(880,818,990,928)),'c_sph_pron':(C,(1018,818,1128,928)),'c_sph_head':(C,(1154,818,1264,928)),'c_sph_lum':(C,(1286,818,1396,928)),'c_sph_eye':(C,(1416,818,1520,928)),
}
for k,(im,box) in cr.items():
    im.crop(box).save(f'crops/{k}.png')
# contact sheet
W=1600; x=y=0; rowh=0
sheet=Image.new('RGB',(W,1100),(40,0,40))
for k,(im,box) in cr.items():
    c=im.crop(box); c.thumbnail((300,300))
    if x+c.width>W: x=0;y+=rowh+4;rowh=0
    sheet.paste(c,(x,y)); x+=c.width+4; rowh=max(rowh,c.height)
sheet.save('crops/_sheet.png')
