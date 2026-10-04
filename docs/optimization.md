# Optimalizace entit – stav a jak navázat

Poznámky k optimalizaci vykreslování entit (modely, kouzla, pool). Nástroje, kterými se měřilo a
generovala data, jsou v `tools/optimization/`.

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

## Jak to funguje

### Pool entit (`decode_level.gd`)
- `entites_pool[uid]` – `uid = Vector3i(class, modelIndex, libType)`, libType 1 = `library`, 2 = `library2`.
- Každý snímek `renderEntites` bere první volné uzly (`act_index`), `show_hide_entites` zobrazí
  použité a skryje zbytek, vynuluje počítadla.
- **Předplnění** (`_begin_level_pool`, voláno z `gameInit`):
  1. `GetLevelSpells()["level"]` → `PrefillData.LEVELS[level]` (max. současných entit každého druhu).
  2. + kouzla z `PrefillData.LEVEL_SPELLS[level]` → `PrefillData.SPELLS[spell]` (max. přes kouzla, přičteno).
  3. Scény se načtou hned (`wait=true`) a všechny uzly se vytvoří hned (`_fill_pool(-1)`) – během načítání.
  4. Zbytek knihovny se načítá na pozadí po jedné (`_background_load_step`).
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
- Původní uzel `emitting=false` (světlo pod ním zůstává), sdílený emitor dostává částice přes
  `emit_particle()` s pozicí a rychlostí spočítanou z `ParticleProcessMaterial` (sám shader je
  ručně emitovaným částicím nepočítá).
- Meteor (orbit velocity) se nesdílí.

### MultiMesh (`entity_multimesh.gd`)
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

Testovací prostředí, které se osvědčilo: herní data převedená do `~/.local/share/godot/app_userdata/MagicBalls`,
Xvfb + `--rendering-driver vulkan` (lavapipe), Blender jako bpy wheel s malým wrapperem pro import `.blend`.
