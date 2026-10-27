# Ladění trhnutí, ukončení a pádů – návod pro analýzu na jiném počítači

Návod pro člověka i pro Claude: jak hru spustit s diagnostikou, nahrát hraní, přehrát ho kolikrát je
třeba a z čeho poznat, kde je problém. Měření výkonu a harness jsou podrobně v `docs/optimization.md`.

## Co je potřeba

- Godot **4.7.2** (console verze, např. `..\Godot_v4.7-stable_win64_console.exe` vedle repozitáře).
- Visual Studio 2022 s C++ a jeho CMake
  (`C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`),
  Python 3 s Pillow (koláže snímků).
- Přeložené rozšíření: `cmake --build gitextension/godot-cpp-template/build --config Release --target EXTENSION-NAME`
  (Release DLL), totéž s `--config Debug` (DLL, kterou používá editor). POST_BUILD kopíruje DLL do
  `godot-code/MBEXout/bin/windows` – nesmí přitom běžet Godot.
- Pro měření, která mají odpovídat hře, dočasně přepnout v `godot-code/addons/MBEX/mbex.gdextension` řádek
  `windows.x86_64.single.debug` na Release DLL (`EXTENSION-NAME.windows.template_debug.x86_64.dll`);
  na konci `git checkout -- godot-code/addons/MBEX/mbex.gdextension`.

### Pravidla

- Okno hry bere myš (kurzor je uzavřený v okně). **Před každým otevřením okna se zeptat uživatele.**
- Okno hry nikdy nezabíjet natvrdo (`Stop-Process -Force`): Windows ponechá kurzor uzavřený. Zavírat
  `CloseMainWindow()`; kdyby kurzor zůstal zamčený, uvolní ho `user32!ClipCursor(NULL)`.
- Výstupy (logy, CSV, snímky) jen do `tools/optimization/out/` (gitignore), skripty harnessu se kopírují
  do `godot-code/zz_tools/` (gitignore). Nic nezapisovat mimo složku projektu.
- Na čekání na videa a klikání v menu uživatel nemá čas: testovací sezení vždy s `--skip-videos --level N`.

## Parametry hry (za `--`)

| Parametr | Co dělá |
|---|---|
| `--debug-log` | Log do stderr a `user://debug.log` (`%APPDATA%\Godot\app_userdata\MagicBalls\debug.log`), každý řádek hned zapsaný; první řádek obsahuje skutečný čas (pro spárování s časy zvenku). Stavy vlákna enginu, start/konec levelu, kroky ukončení. |
| `--debug-hitches` | Každý snímek o 30 ms delší, než povoluje limit FPS (24 ve hře, 30 v menu), s rozpisem: části `MBrun`, části enginu, vykreslení, kompilace shaderů, nové uzly a co se v něm stalo (čekání na scénu, uzel entity vytvořený za hry, pomalé volání zvuku). Každých 10 s souhrn (počet snímků, nejdelší). `scenes/hitch_log.gd`. |
| `--debug-sounds` | Výpis každého zvuku / hudby / řeči. |
| `--skip-videos` | Sám mačká mezerník přes uvítací obrazovku, intro a animace (ne mapu mezi levely). |
| `--level N` | Vlastní level N (1 = level 1) místo nastavení; s `--skip-videos` rovnou do něj (první menu přeskočí, menu po levelu ne – aby šlo hru ukončit). |
| `--record <soubor>` | Nahraje vstupy hráče (remc2 InputRecorder, `.dem`), uloží se na konci levelu. |
| `--play <soubor>` | Přehraje nahrávku v normální hře (místo hráče). |

Spuštění testovacího sezení (PowerShell, z adresáře nad repozitářem):

```powershell
$o = "MagicBalls\tools\optimization\out"
Start-Process -FilePath .\Godot_v4.7-stable_win64_console.exe `
  -ArgumentList @("--path","MagicBalls\godot-code","--","--debug-log","--debug-hitches","--skip-videos","--level","1","--record","$((Resolve-Path $o).Path)\rec-x.dem") `
  -RedirectStandardOutput "$o\play-x.out" -RedirectStandardError "$o\play-x.err" -PassThru -NoNewWindow
```

Log se shrne: `python tools\optimization\python\hitchrep.py tools\optimization\out\play-x.err` – časová osa fází
(stavy enginu, start a konec levelu, kroky ukončení) a dlouhé snímky seskupené podle toho, kam šel čas.

## Přehrání nahrávky (harness, bez hráče)

```powershell
copy tools\optimization\godot\profile.* godot-code\zz_tools\
$env:MB_REPLAYS = "<repozitář>/tools/optimization/recordings"   # nebo out
$env:MB_SHOT_EVERY = "30"        # snímek obrazovky každých 30 snímků (porovnání vzhledu)
$env:MB_USER_ARGS = "--debug-log"
tools\optimization\run_variants.ps1 -Prefix x -Frames 5000 -Gpu 0 -Level 0 -Play level1-checkpoint.dem -Runs "ultra=;;3,1","high=;;2,1"
```

- `-Level` je index levelu (level 1 = 0), `-Runs "název=varianty;měřítko3D;stíny,odrazy"`.
- Výstup: CSV po snímcích (`frame_ms`, `process`, `render`, části enginu `e_*`, …), log, snímky.
  `python tools\optimization\python\profrep.py a.csv b.csv` porovná běhy.
- Harness běží bez limitu FPS; čas snímku kolísá mezi běhy o ±0,5 ms (zatížení PC) – porovnávat čas GPU
  (`-Gpu 1`) a počty, ne jednotlivé běhy.
- Nahrávky v repozitáři (`tools/optimization/recordings`):
  - `level1-checkpoint.dem` – level 1 s checkpointem (uživatel, 27. 10. 2026),
  - `level1-pillars.dem` – level 1 až ke krystalovým pilířům na konci (shadow acne).
- Replaye remc2 (level 5, 25 …): `remc2-regression-test/memimages/regressions/` v repozitáři remc2.

## 1. Cuknutí ve hře

Postup: sezení s `--debug-hitches --record`, uživatel projede místo, kde to cuká → `hitchrep.py` → přehrát
nahrávku harnessem a hledat ve CSV (sloupec `frame_ms` a části) i menší cuknutí pod prahem (práh je
`THRESHOLD_MS` v `scenes/hitch_log.gd`).

Známé příčiny (27. 10. 2026):
- **Fade palety ve hře** dával každému kroku vlastní snímek → hra během každého fade (checkpoint, zásah,
  záblesky) běžela poloviční rychlostí. Opraveno `a9cf8a05`. Poznat podle střídání stavů
  `IN_GAME_LOOP` / `FADEIN_FADEOUT_LOOP` v `--debug-log`.
- **Start hudby** (`SOUND_start_sequence`) 17–53 ms na hlavním vlákně při každé změně hudby – neopraveno.
- **Start levelu** 2,7 s + 2× 0,5 s (kompilace 124 shaderů, 3 680 uzlů poolu, čekání na scény) – při načítání.
- **Přechody** (konec levelu, mapa, menu) 180–220 ms v enginu; **videa** jedou tempem videa (80 ms/snímek).
- Na tomto PC (RTX A4000) se cuknutí ve hře nad prahem nezachytilo – na slabším PC zkusit totéž.

## 2. Pomalé ukončení (proces visí)

Na tomto PC: konec hry → uvolnění scény 0,8 s → odpojení rozšíření → proces zmizí asi 0,4 s potom,
celkem ~1,2 s; žádné vlákno nevisí. Na jiném PC:

1. Spustit hru s `--debug-log` (+ `--skip-videos --level 1`) a vedle ní hlídač vláken:
   `pwsh -File tools\optimization\threadwatch.ps1 -Log tools\optimization\out\play-x.err -Out tools\optimization\out\threads-x.txt`
   – od řádku „exit game“ každých 50 ms vlákna procesu (stav, důvod čekání) a čas, kdy proces zmizel.
2. V logu: `exit game, scenes still loading: N` (scény načítané na pozadí – Godot na ně při ukončení čeká),
   `REMC2EndGame: …`, `end: …` (kroky úklidu enginu), `MBEX freed`, `extension uninitialized`. Čas zmizení
   procesu z `threadwatch` minus čas posledního řádku = co dělal Godot po nás.
3. Visí-li proces dlouho: Visual Studio → Debug → Attach to Process → Break All → okno Threads / Parallel
   Stacks ukáže, na čem které vlákno čeká (moduly vláken `threadwatch` nerozliší).
4. Podezřelé: načítání scén na pozadí, vlákno hudby AWE32, WorkerThreadPool, ovladač zvuku, antivir.

## 3. Pád při ukončení na Linuxu

Hlášení (raincz): `free(): invalid size`, signál 6, výpis našeho handleru (`[mb …] CRASH signal 6` a
zásobník s offsety `MagicBalls.linux…so+0x…`) – naše knihovna volaná přímo z `main()` při úklidu.
Pravděpodobně poškozená hlavička bloku haldy (zápis za konec bufferu dříve), glibc to pozná až při `free`.

Na Linuxu (kde se hra sestavuje):
1. Jména funkcí k offsetům: `addr2line -C -f -e MagicBalls.linux.template_debug.x86_64.so 0x3183e8 0x31290e`.
2. AddressSanitizer – zastaví se přímo u chybného zápisu:
   `cmake -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" -DCMAKE_SHARED_LINKER_FLAGS=-fsanitize=address …`,
   spuštění `LD_PRELOAD=$(gcc -print-file-name=libasan.so) ./MagicBalls.x86_64 -- --debug-log`.
3. Bez přestavby: `MALLOC_CHECK_=3` nebo `valgrind --error-limit=no ./MagicBalls.x86_64`.

Na Windows (27. 10. ASan nic nenašel – level 1 i 5 včetně ukončení):
- `cmake -S gitextension/godot-cpp-template -B gitextension/godot-cpp-template/build-asan -G "Visual Studio 17 2022" -A x64 "-DCMAKE_CXX_FLAGS=/DWIN32 /D_WINDOWS /GR /EHsc /fsanitize=address /Zi" "-DCMAKE_C_FLAGS=/DWIN32 /D_WINDOWS /fsanitize=address /Zi" "-DCMAKE_SHARED_LINKER_FLAGS=/DEBUG /INCREMENTAL:NO"`,
  build `--config Release`.
- `clang_rt.asan_dynamic-x86_64.dll` z `VC\Tools\MSVC\<verze>\bin\Hostx64\x64` zkopírovat vedle DLL do
  `godot-code/MBEXout/bin/windows` (přes PATH se nenačte – chyba 126), `mbex.gdextension` přepnout na
  Release DLL, pustit harness s nahrávkou.
- Potom runtime smazat a **vynutit nové linkování běžné DLL** (smazat `bin/windows/EXTENSION-NAME…x86_64.dll`
  a postavit `build`) – oba buildy sdílejí výstup a běžný by jinak ASan DLL považoval za aktuální.
- Offsety z výpisu pádu na Windows převede `python tools\optimization\python\symbolize.py <log> <dll>`.

## Další nástroje

- `tools/optimization/python/apklist.py <url> <out.tsv>` – seznam souborů APK/zipu na webu bez stažení
  (HTTP range na centrální adresář); tak se našlo 1 140 prázdných souborů ve starém APK.
- `tools/optimization/godot/settingsshot.gd` – snímky úvodní obrazovky a nastavení v několika jazycích.
- Build na CI: tag `v*` = celé vydání, `a*` = jen Android; oba vytvoří veřejný GitHub Release.
  Stav běhu: `https://api.github.com/repos/turican0/MagicBalls/actions/runs`.
