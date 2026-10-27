# Watches how the game ends: from the "exit game" line of its log until the process is gone, every
# 50 ms the threads it still has, by the module they started in (our library, Godot, the audio
# driver...). Finds a thread that keeps the process alive after the game has ended.
#   threadwatch.ps1 -Log <the game's stderr with --debug-log> -Out <file>
# The game process is found by name (Godot_v4.7*, not the _console wrapper, or MagicBalls).
param([string]$Log, [string]$Out)

function Find-Game {
	Get-Process | ? { ($_.ProcessName -like "Godot_v4.7*" -and $_.ProcessName -notlike "*console*") -or $_.ProcessName -eq "MagicBalls" } | Select -First 1
}

# wait for the end of the game in the log
while (-not (Test-Path $Log) -or -not (Select-String -Path $Log -Pattern "exit game" -Quiet)) {
	Start-Sleep -Milliseconds 100
	if (-not (Find-Game)) { "no game process" | Out-File $Out; exit }
}
$t0 = Get-Date
"exit game seen at $($t0.ToString('HH:mm:ss.fff'))" | Out-File $Out
$game = Find-Game
$modules = $game.Modules | % { [pscustomobject]@{ Name = $_.ModuleName; Base = $_.BaseAddress.ToInt64(); End = $_.BaseAddress.ToInt64() + $_.ModuleMemorySize } }
$last = ""
while ($true) {
	$p = Get-Process -Id $game.Id -ErrorAction SilentlyContinue
	if (-not $p) { break }
	$p.Refresh()
	$byModule = @{}
	foreach ($t in $p.Threads) {
		$a = $t.StartAddress.ToInt64()
		$m = ($modules | ? { $a -ge $_.Base -and $a -lt $_.End } | Select -First 1).Name
		if (-not $m) { $m = "?" }
		$key = "$m/$($t.ThreadState)$(if ($t.ThreadState -eq 'Wait') { ':' + $t.WaitReason })"
		$byModule[$key] = 1 + [int]$byModule[$key]
	}
	$line = ($byModule.GetEnumerator() | Sort Name | % { "$($_.Name) x$($_.Value)" }) -join ", "
	if ($line -ne $last) {
		"{0,7:F2} s  threads {1}: {2}" -f ((Get-Date) - $t0).TotalSeconds, $p.Threads.Count, $line | Out-File $Out -Append
		$last = $line
	}
	Start-Sleep -Milliseconds 50
}
"process gone at {0:F2} s ({1})" -f ((Get-Date) - $t0).TotalSeconds, (Get-Date).ToString('HH:mm:ss.fff') | Out-File $Out -Append
