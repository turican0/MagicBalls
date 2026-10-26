# Runs the profiling harness (godot/profile.gd, copied to godot-code/zz_tools) with a remc2 replay,
# once per run, and writes the CSVs to out/ (not committed). See docs/optimization.md.
#
# run_variants.ps1 -Prefix base -Frames 1500 -Gpu 0 -Runs "base=;", "noshadows=noshadows;", "half=;0.5"
#   -Runs           name=variants;scale;quality - variants of profile.gd (MB_VARIANT), scale = 3D render
#                   scale, quality = MB_QUALITY "shadows,reflections" (e.g. "low=;0.85;1,0")
#   -Level / -Play  --set_level (level - 1) and the replay under -Replays (default level 5)
#   -RenderThread   safe / separate (Godot's --render-thread)
#   -Godot          the Godot 4.7 executable (default: MB_GODOT, else next to the repository)
#   -Replays        remc2-regression-test/memimages/regressions of remc2 (default: MB_REPLAYS)
# Environment passed to the harness as it is: MB_STATE_FRAMES, MB_HIDE_KINDS, MB_DUMP_FRAME,
# MB_SHOT_EVERY, MB_WINDOW, MB_USER_ARGS (user arguments after --, e.g. --debug-log).
param([string]$Prefix, [int]$Frames, [string]$Gpu, [string[]]$Runs, [string]$Level = "4",
	[string]$Play = "record003/Level5-mine.dem", [string]$RenderThread = "",
	[string]$Godot = $(if ($env:MB_GODOT) { $env:MB_GODOT } else { "$PSScriptRoot\..\..\..\Godot_v4.7-stable_win64_console.exe" }),
	[string]$Replays = $env:MB_REPLAYS)
$out = "$PSScriptRoot\out"
New-Item -ItemType Directory -Force $out | Out-Null
$project = (Resolve-Path "$PSScriptRoot\..\..\godot-code").Path
$env:MB_TEST_LEVEL = $Level
$env:MB_PLAY_FILE = "$Replays/$Play"
$env:MB_TEST_FRAMES = "$Frames"
$env:MB_GPU = $Gpu
foreach ($r in $Runs) {
	$name, $spec = $r.Split("=", 2)
	$variant, $scale, $quality = $spec.Split(";", 3)
	$env:MB_VARIANT = $variant
	$env:MB_SCALE = $scale
	$env:MB_QUALITY = $quality
	$env:MB_OUT = "$out/$Prefix-$name.csv"
	$env:MB_STATE_OUT = "$out/$Prefix-$name-state"
	$env:MB_SHOT_OUT = "$out/$Prefix-$name-shot"
	$godotArgs = @("--path", $project)
	if ($RenderThread -ne "") { $godotArgs += @("--render-thread", $RenderThread) }
	$godotArgs += "res://zz_tools/profile.tscn"
	if ($env:MB_USER_ARGS) { $godotArgs += @("--") + $env:MB_USER_ARGS.Split(" ") }
	$p = Start-Process -FilePath $Godot -ArgumentList $godotArgs -RedirectStandardOutput "$out\$Prefix-$name.out" -RedirectStandardError "$out\$Prefix-$name.err" -PassThru -NoNewWindow
	# never killed hard: the game confines the cursor and Windows keeps that after a killed process
	if (-not $p.WaitForExit(400000)) { $p.CloseMainWindow() | Out-Null; $p.WaitForExit(30000) | Out-Null; "$name timeout" } else { "$name exit $($p.ExitCode)" }
}
