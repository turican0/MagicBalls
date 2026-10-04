#!/bin/bash
# Spell census: every spell (0..25) at levels I-III, cast in game level 0.
# Needs the engine built with ../engine/spelltest_hook.patch and tools/optimization/godot/* copied to
# godot-code/zz_tools/. Env: GODOT (Godot binary), WORK (output folder, gets spall/).
cd "$(dirname "$0")/../../../godot-code"
mkdir -p "$WORK/spall"
for sp in $(seq 0 25); do for lv in 0 1 2; do echo "$sp,$lv"; done; done | xargs -P 4 -I{} bash -c 'f=$(echo {} | tr , _); [ -f "$WORK/spall/$f.json" ] && exit 0; MB_TEST_SPELL={} AN_AIM_Y=300 AN_LEVEL=0 AN_STEPS=700 AN_OUT="$WORK/spall/$f.json" timeout 900 "$GODOT" --headless --path . res://zz_tools/spell.tscn > "$WORK/spall/$f.log" 2>&1'
