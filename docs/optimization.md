# Optimalizace entit – stav a jak navázat

Poznámky k optimalizaci vykreslování entit (modely, kouzla, pool) a snímku celé hry. Nástroje, kterými
se měřilo a generovala data, jsou v `tools/optimization/`.

## Co je hotové

| Commit | Co | Kde |
|---|---|---|
| Fix freeing of pooled entities with a start script | `show_hide_entites`: ústa/portál (14,461/462) se dřív nikdy neuvolnila (uid má v `z` typ knihovny 1/2, ne 0), smyčka přeskakovala uzly, bucket se mazal podle hodnoty. | `scenes/decode_level.gd` |
| Prepare the entity pool for the level before it starts | Pool se naplní při načítání levelu podle tabulky + kouzel levelu; knihovna se nenačítá celá při startu programu. | `decode_level.gd`, `entity_prefill_data.gd`, `MBEXmain.cpp` (`GetLevelSpells`) |
| Merge entity lights when there are many of them | Nad 64 světel entit se blízká světla stejného druhu slučují. | `scenes/entity_light_budget.gd` |
| Draw the particles of many entities through one shared emitter | Ohně (fair 10_8, fire 10_77) – jeden `GPUParticles3D` na druh, `emit_particle()`. | `scenes/entity_particles.gd` |
| Limit entity textures to 1024 px | `process/size_limit=1024` u 52 textur (zdroje beze změny). | `*.import` |
| Decimate the heaviest entity models | Blender (collapse) / meshoptimizer pro meshe uvnitř `.tscn`. | `entites/sources/...` |
| Join the meshes of the balloons | 401 meshů → 1 mesh se 4 materiály, geometrie totožná. | `assets/ballon/*.glb` |
| Draw mesh-only entities with MultiMeshes | Scény jen z meshů (bez skriptu, animace, světla, částic, skinu, průhlednosti) se kreslí MultiMeshem po dlaždicích 32×32. | `scenes/entity_multimesh.gd` |
| Do not let the engine sleep in every frame | `maxGameFps = 0` – `LockFps(uint8_t)` z 1000 dělal 232, engine spal ~4,5 ms každý snímek. | `modified/remc2_mod.cpp` |
| Do not print the FPS in every frame | `print("FPS:…")` v `mb_engine._process` stál 3,2 ms/snímek. | `scenes/mb_engine.gd` |
| Draw the splash particles with fewer polygons | Koule šplouchnutí 64×32 → 16×8 (sdílený emitor 20 000 instancí: 345M → 30M primitiv). | `entites/object_10_186_splash.tscn` |
| Draw the entities from C++ | Smyčka entit, pool, MultiMeshe a show/hide v `EntityRenderer`; GDScript jen dodává scény. | `src/MBEX/entity_renderer.*`, `decode_level.gd` |
| Do not stall on showing entities with shared particles | Převzatý `GPUParticles3D` → `Node3D` (jinak `particles_is_inactive` při každém zobrazení – synchronizace renderu, až 135 ms). | `scenes/entity_particles.gd` |
| Print the sound debug output only with --debug-sounds | `print()` u každého zvuku: 15 s za replay levelu 25, až 218 ms/snímek. | `scenes/sounds.gd` |
| Stop the whole hidden pool nodes, not only their roots | `process_mode = DISABLED` pro skryté uzly – jejich AnimationPlayery/skeletony běžely; u uzlů vytvořených za hry to zakládalo GPU buffery desítkám skrytých skinovaných modelů (trhnutí až 690 ms, level 25). | `entity_renderer.cpp`, `decode_level.gd` |

## Jak to funguje

### Pool entit (`EntityRenderer` + `decode_level.gd`)
- Druh entity `uid = Vector3i(class, modelIndex, libType)`, libType 1 = `library`, 2 = `library2`.
- Každý snímek `renderEntites` předá data enginu (`GetEntites`, 31 hodnot × 1000) do C++ `EntityRenderer`:
  ten spočítá transformace, bere první volné uzly druhu, zapisuje do uzlů jen změny, plní MultiMeshe
  a nakonec (`finish_frame`) zobrazí použité uzly a skryje ostatní (`process_mode DISABLED` celého uzlu).
- GDScript odpovídá jen na to, co C++ nezná: scénu nového druhu (`_resolve_entity_kind`, i když se ještě
  načítá – pak se ptá znovu) a uzel, když pool došel (`_make_entity_node`).
- Shoda s původní GDScript verzí ověřena výpisem stavu entit (`MB_STATE_FRAMES`) na snímcích replaye.
- **Předplnění** (`_begin_level_pool`, voláno z `gameInit`):
  1. `GetLevelSpells()["level"]` → `PrefillData.LEVELS[level]` (max. současných entit každého druhu).
  2. + kouzla z `PrefillData.LEVEL_SPELLS[level]` → `PrefillData.SPELLS[spell]` (max. přes kouzla, přičteno).
  3. Scény se načtou hned (`wait=true`) a všechny uzly se vytvoří hned (`_fill_pool(-1)`) – během načítání.
  4. Zbytek knihovny se načítá na pozadí po jedné (`_background_load_step`).
- Čarodějové: při startu levelu se načtou scény „sady“ všech 8 barev (hrad 3,96+b, vlajky budov 10,96+b,
  balony 3,88+b, mana koule 10,67+b, model čaroděje); jakmile engine ohlásí čaroděje v levelu
  (`GetLevelSpells()["wizards"]`, snímek 10 a pak každých 300), dostane každá přítomná barva sadu:
  hrad 2, vlajky 4, balony 4, model 1 a mana koule tolik, kolik jich v levelu je (zlaté + barevné, min. 32).
  Funguje pro libovolný počet hráčů v multiplayeru. Barva: 0 bílá, 1 červená, 2 fialová, 3 modrá,
  4 zelená, 5 růžová, 6 oranžová, 7 černá; u hráče z `GetTrueWizardNumber`, u počítačového čaroděje
  z jeho modelu (203 modrá, 211 červená, 219 černá, 227 oranžová, 235 růžová, 243 fialová, 251 zelená).
- Za hry: `_check_level_spells` (každých 300 snímků) přidá nově nalezená kouzla; druh, kterému dojdou
  uzly (`ran_out`), dostane rezervu 25 % (min. 2). Doplňuje se s rozpočtem 3 ms/snímek.
- Entity se startovacím skriptem (14,461 / 14,462) se nepoolují – po zmizení se uvolní.
- Scéna, která se ještě načítá, se nekreslí (objeví se, až je načtená) – nikdy se nečeká.

### Engine: `GetLevelSpells()`
Vrací `{"level": levelnumber_43w, "spells": [...]}` – kouzla, která má kterýkoli čaroděj
(entity class 3, model 0, `SpellsEnabled`) + kouzla položená v levelu (`terrain_2FECE.entity_0x30311`,
type 9, subtype = index kouzla). Na úplném začátku levelu (prvních pár kroků) ještě čarodějové kouzla
nemají – proto i tabulka `LEVEL_SPELLS`.

Pořadí kouzel (správné, `Spells.h` je špatně – je tam „???“): 0 Fireball, 1 Possession, 2 Castle,
3 Speed Up, 4 Morph, 5 Heal, 6 Shield, 7 Lightning, 8 Rebound, 9 Meteor, 10 Teleport, 11 Invisible,
12 Steal Mana, 13 Beyond Sight, 14 Duel, 15 Tremor, 16 Crater, 17 Earthquake, 18 Volcano,
19 Summon Army, 20 Gravity Well, 21 Whirlwind, 22 Fool's Mana, 23 Magic Mine, 24 Alliance, 25 Cave In.

### Světla (`entity_light_budget.gd`)
- Sbírají se `OmniLight3D` aktivních entit (bez stínů, ve scéně původně viditelná, rodič viditelný).
- ≤ `MAX_LIGHTS` (64): beze změny. Víc: mřížka (2 jednotky u kamery, za 16 jednotkami se zdvojuje),
  ve skupině stejné barvy/dosahu → jedno světlo v těžišti, energie = součet, dosah + ½ velikosti skupiny.
- Zkoušeno a zamítnuto: zvedání sloučeného světla (horší), MAX 32 (velký rozdíl).
- 300 ohňů (lavapipe): rozdíl obrazu ~4/255.

### Částice (`entity_particles.gd`)
- Sdílí se jen `GPUParticles3D`, které nezávisí na poloze emitoru: ne one-shot, explosiveness 0,
  world space, bez sub-emitterů/trailů/orbit/radial velocity, tvar point/sphere/box.
- Původní uzel se nahradí obyčejným `Node3D` (stejné jméno a místo, světlo pod ním zůstává),
  sdílený emitor dostává částice přes
  `emit_particle()` s pozicí a rychlostí spočítanou z `ParticleProcessMaterial` (sám shader je
  ručně emitovaným částicím nepočítá).
- Meteor (orbit velocity) se nesdílí.

### MultiMesh (`entity_multimesh.gd` rozhoduje, `EntityRenderer` kreslí)
- `can_batch(scene)` – jen `Node3D` + `MeshInstance3D`, neprůhledné `BaseMaterial3D` bez billboardu.
- Transformace jako u uzlu: pozice (s wrapem kolem kamery), yaw, scale entity, uzel „Scale“.
- Týká se: článků stonožky (až 224), krků draka, kamenných hlav, šípů, sudů, košů, dolmenů.
- Kouř a stromy NE (průhlednost / skripty) – změnilo by se pořadí vykreslení.

## Naměřené výsledky

- 300 ohňů (Meteor-like), lavapipe: 130 ms → 54 ms (sdílené částice) → 36 ms (+ světla).
- Knihovna (138 scén): načtení 128 s → 29 s, trojúhelníky 10,5M → 3,8M, textury 1013 → 534 Mpix,
  uzly 8409 → 2009.
- Kouzla v levelu 0 (max. současných nových entit): Meteor III ~340 (323× 10_38 fair),
  Volcano 537–620 (fair + kouř 10_63 + fire 10_77), Lightning III 264× 9_151, Whirlwind 24 tornád.
- Pozor: časy snímků pod softwarovým Vulkanem (lavapipe) jsou orientační, skutečný přínos je třeba
  změřit na GPU.

### Snímek celé hry (10/2026, Windows, RTX A4000, replaye remc2)
- Level 5 (`record003/Level5-mine.dem`), medián snímku: 19,1 ms → 5,9 ms (engine sleep, FPS print,
  entity v C++); GPU 7,4 → 5,4 ms (částice šplouchnutí).
- Level 25 (`record041/mc2l24.dem`): medián 7,8 → 4,4 ms, p99 15,8 → 8,8 ms, nejhorší snímek
  687 ms → 48 ms (zvukové printy, synchronizace částic, zastavené skryté uzly).
- Render na hlavním vlákně (`--render-thread safe`) by stál 0,2–1,2 ms na mediánu, 2,5–4 ms na p95/p99;
  samostatné renderovací vlákno je výchozí vypnuté (experimentální v Godotu, v 1 ze 17 replayů ztratilo
  Vulkan zařízení), v nastavení VIDEO jde zapnout.
- Zkoušeno a zamítnuto: načíst celou knihovnu při startu levelu (+3,5 s, trhnutí nezmizí), zahřát
  pipeline vykreslením poolu, naučené předvyplnění (`user://entity_counts.cfg`, +0,8 s – po zastavení
  skrytých uzlů už nic nepřineslo).
- Na GPU (ablace `MB_HIDE_KINDS`, level 5): nejvíc stojí castle orb (575k trojúhelníků v jednom kusu)
  a hornet (123k × 8), dohromady −1,9 ms; upíří lukostřelci jsou díky LOD levní, ale mají 45
  renderovacích objektů na kus.

## Co by šlo dál

- Kouř 10_63 (až 297 v levelu 0, 195 při Volcanu) – průhledný, do MultiMeshe jen se změnou pořadí
  vykreslení; případně sdílené částice/billboardy.
- Lightning (9_151, 264 při III) – světla už se slučují, AnimationPlayer na každé instanci zůstává.
- Čarodějové – instance jsou drahé (CSGTorus3D + SubViewport s ukazatelem zdraví); CSG šlo by
  zapéct do meshe.
- Volcano/Teleport/Possession v census ukazují i stromy apod. – to jsou změny stavu existujících
  entit, ne nové.
- Level 65: až 515 vampire bowmanů současně (dnes po 120k trojúhelníků) – kandidát na LOD/visibility range.

## Nástroje (`tools/optimization/`)

Godot skripty se spouštějí z projektu: zkopírovat `tools/optimization/godot/*` do
`godot-code/zz_tools/` (nekomitovat), Godot 4.7.2.

| Soubor | K čemu |
|---|---|
| `godot/levels.gd/.tscn` | Census levelu bez 3D scény: `AN_LEVEL`, `AN_STEPS` (3000), `AN_OUT` → JSON s max. počty `class,model,drawflag` + kouzla levelu. |
| `godot/spell.gd/.tscn` | Census kouzla – potřebuje engine s `engine/spelltest_hook.patch` (`MB_TEST_SPELL="spell,level"`): dá všechna kouzla, neomezenou manu, obejde hrad. |
| `python/runall.sh` | Pustí spell census pro všech 26×3 kouzel (`GODOT`, `WORK`). |
| `python/genprefill.py` | Z `WORK/lv2/*.json` a `WORK/spall/*.json` vygeneruje `scenes/entity_prefill_data.gd`. |
| `python/spellrep.py` | Čitelný přehled, co které kouzlo vytvoří (+ statistiky scén z `lib_analysis.csv`). |
| `godot/an.gd/.tscn` | Analýza knihovny: čas načtení/instance, trojúhelníky, textury, světla, částice (`AN_LIST`, `AN_OUT`). |
| `godot/tex.gd` | Seznam textur scén a jejich velikost (`-s`, `AN_LIST`, `AN_OUT`). |
| `godot/mmcand.gd` | Které scény jdou do MultiMeshe beze změny vzhledu. |
| `godot/game.gd/.tscn` | Celá hra (Xvfb + Vulkan): `MB_TEST_LEVEL`, `MB_TEST_FRAMES`, `MB_TEST_CLICKS`; vypisuje pool, MultiMesh, časy snímků. |
| `godot/shot.gd/.tscn` | Render scén zblízka/zdálky (`SCENES`, `OUT`) – porovnání před/po. |
| `godot/fires.gd`, `parts.gd`, `mmtest.gd` | Testy světel (`BUDGET`), sdílených částic (`SHARED`), MultiMeshe (`MM`) proti originálu. |
| `godot/decim.gd` | Zjednodušení `ArrayMesh` uložených v `.tscn` (meshoptimizer, `SRC`, `RATIO`) → `.res`; pak `python/swapmesh.py` přepojí `.tscn`. |
| `blender/decimate.py` | Decimace `.glb`/`.blend` na cílový počet trojúhelníků (bpy 5.1). |
| `blender/joinmeshes.py` | Spojení všech meshů `.glb` do jednoho (balony). Transformace jsou v pořádku – „menší“ bbox v Blenderu je jen nafouknutý `bound_box` originálu, ověřeno `vbb.py`. |
| `python/glbextimg.py` | Po exportu z Blenderu přepojí vložené obrázky `Image_N` na Godotem vytažené `<glb>_N.*` (jinak Godot vytáhne nové kopie bez limitu 1024). |
| `godot/profile.gd/.tscn` | Profil celé hry s replayem remc2 (`MB_PLAY_FILE`, `--play_file`): CSV po snímcích – části `MBrun` (`Global.prof_mark`), části enginu (C++ `getFrameProfile`, `frame_profile.h`), process/render, GPU (`MB_GPU=1`), draw cally, kompilace pipeline, VRAM, počty druhů (`k_*`); `-entities.csv` po druzích entit (klony, show/hide, statický rozbor scény); výpis stavu entit (`MB_STATE_FRAMES`), ablace druhů (`MB_HIDE_KINDS`), varianty nastavení (`MB_VARIANT`, `MB_SCALE`). |
| `run_variants.ps1` | Spouští `profile.gd` s replayem pro více variant za sebou, výstupy do `out/` (ignorováno). Hru nikdy neukončuje natvrdo – omezený kurzor by zůstal. |
| `python/profrep.py` | Přehled CSV (avg/p50/p95/p99), víc CSV vedle sebe (p50 a rozdíl proti prvnímu). |
| `python/spellparts.py` | Úseky replaye s kouzlem podle `k_*` (např. `k_meteor 1`) a časy v nich proti zbytku. |

Replaye jsou v remc2: `remc2-regression-test/memimages/regressions/` (`record003/Level5-mine.dem` level 5,
`record006/Level-25.dem` a `record041/mc2l24.dem` level 25 – v `record006` je Meteor). Level se zadává
o 1 menší (`-Level 24`). Hra s parametrem `-- --debug-log` píše hned vyprázdňovaný log startu a stavů
enginu (stderr + `user://debug.log`).

Testovací prostředí, které se osvědčilo: herní data převedená do `~/.local/share/godot/app_userdata/MagicBalls`,
Xvfb + `--rendering-driver vulkan` (lavapipe), Blender jako bpy wheel s malým wrapperem pro import `.blend`.
