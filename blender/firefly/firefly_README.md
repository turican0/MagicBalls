# firefly_flight.glb (v4)

Zdrojový model: "Firefly" od Xiao Yang (https://sketchfab.com/xyang), licence CC BY 4.0
(https://sketchfab.com/3d-models/firefly-f723433582e1405abf7c3950f781f898).
Úpravy: tvar krovek, nové UV, procedurální PBR textury, kostra/skinning, doplněná blanitá zadní křídla, animace.
Při použití je nutné uvést autora původního modelu (CC BY 4.0); atribuce je i v metadatech souboru.

## Co je nového ve v2
- Krovky: slabší, kožovité a chlupaté (matná povrchová úprava, jemné tečkování, tři podélné linie),
  každá se zužuje ke špičce a špičky se nedotýkají ve švu (jako u čeledi Lampyridae).
- Krovky se hýbou: jsou vyklopené do stran pod úhlem (dihedral ~40 °) a při každém úderu křídel se
  pohybují dolů/nahoru spolu s nimi v menší amplitudě (~±11 °) a s mírným zpožděním.
- Textury: mnohem jemnější mikrodetail (chloupky, dvě velikosti důlků, mramorování, růžový štít s tmavým
  podélným pruhem, matné roughness), nohy a tykadla mají vlastní normal mapy a chloupky.

## Co je nového ve v4 (textury podle fotek)
- Povrch krovek je odvozen z naměřených statistik na referenční fotce (spektrum měřítka, rozdělení jasu,
  hustota světlých bodů): matné modrošedé „uhlí“ s měkkým chmýřím, řídké světlé body, světlejší k vnějšímu okraji.
  Pixely z fotek nejsou zkopírované, textura je syntetizovaná se stejnými vlastnostmi.
- Štít: průsvitná krémová skořápka s červenorůžovým jádrem a světlými skvrnami; hlava téměř černá a lesklá.
- Zadeček: tmavé hřbetní články s krémovými lemy, krémové boky a břicho (viz fotka za letu); hruď oranžová.
- Křídla: kouřově šedá s tmavou žilnatinou; tykadla černobíle proužkovaná; nohy tmavší.

## Co bylo ve v3
- Struktura povrchu podle makrofotek: krovky mají hustou, nepravidelnou zrnitost s řídkými jasnými odlesky,
  tenký vyvýšený světlý šev a úzký lem; štít a zadeček mají jemnější zrno. Základní barvy jsou beze změny.
- Vyšší rozlišení: krovky 4096x1024, štít 2048x2048, tělo 1536x1536, svítící orgán 2048x1024.
- Tykadla mají výraznější proužkování (tmavé články, světlé spoje).
- Normal mapy jsou JPEG (úspora místa); soubor má ~20 MB, v Godotu se při importu VRAM-komprimuje.

## Obsah
- Osa těla: hlava míří do -Z, nahoru +Y, tělo je dlouhé 1.0 jednotky (přeškáluj kořen podle potřeby).
- Animace: `Fly-loop` (1 s, smyčka, vznášení na místě), `Takeoff` (0.6 s, z klidové pózy do letu).
- Výchozí póza = klidová (krovky zavřené, křídla skrytá).
- Uzly: Rig (tuhé části: Head, Eye_*, Elytra_*, Wing_*), Armature (28 kostí: Thorax, Abd1-3, nohy, tykadla).
- Materiály: M_Elytra, M_Pronotum, M_Body, M_Lantern (emisní, KHR_materials_emissive_strength = 4), M_Eye,
  M_Legs, M_Antenna, M_Palp, M_Wing (alpha blend). Barevné a ORM textury jsou JPEG, normal mapy PNG.
- Křídla mávají 12 Hz (reálně ~40-60 Hz, což by na 60 fps vypadalo jako rozmazání). Rychlost změň přes speed_scale.
- V Godotu zapni v WorldEnvironment Glow, ať svítící orgán září.
