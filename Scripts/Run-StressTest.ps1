# Rogue Rigs - stress test launcher (vault: stress-test-m1).
# Starts the game standalone (-game, 1920x1080 windowed), runs rr.Stress.Run (robot count steps, settle then measure each),
# waits for the process to exit and prints the per-step table (average and p99 frame, game, draw, GPU ms).
# Results: Saved/Stress/Stress_<time>.csv (not committed). Usage, from the repo root:
#   powershell -NoProfile -ExecutionPolicy Bypass -File Scripts/Run-StressTest.ps1 [-Counts 50+100+200+300+400] [-Settle 5] [-Measure 20]
# Build the RogueRigsEditor target first (vault: build-toolchain), editor closed.

param(
	[string]$Counts = "50+100+200+300+400",
	[double]$Settle = 5,
	[double]$Measure = 20,
	[string]$Map = "/Game/RogueRigs/Maps/L_ArenaM1",
	[int]$ResX = 1920,
	[int]$ResY = 1080,
	[string]$EngineDir = "C:\Program Files\Epic Games\UE_5.8\Engine",
	[int]$TimeoutSeconds = 900
)

$ErrorActionPreference = "Stop"
$ProjectDir = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $ProjectDir "RogueRigs.uproject"
$Editor = Join-Path $EngineDir "Binaries\Win64\UnrealEditor.exe"
$StressDir = Join-Path $ProjectDir "Saved\Stress"
$StartTime = Get-Date

# -ExecCmds splits on commas: counts use '+'. Frame cap and vsync off so the frame time is the real cost.
$Exec = "t.MaxFPS 0,r.VSync 0,rr.Stress.Run $Counts $Settle $Measure 1"
$GameArgs = @("`"$Project`"", $Map, "-game", "-ResX=$ResX", "-ResY=$ResY", "-windowed", "-nosplash", "-nop4", "-log",
	"-ExecCmds=`"$Exec`"")

Write-Host "Launching: $Editor $($GameArgs -join ' ')"
$Process = Start-Process -FilePath $Editor -ArgumentList $GameArgs -PassThru
if (-not $Process.WaitForExit($TimeoutSeconds * 1000)) {
	Write-Warning "Timeout after $TimeoutSeconds s: killing the game process."
	Stop-Process -Id $Process.Id -Force
	exit 2
}

$Result = $null
for ($Try = 0; $Try -lt 10 -and -not $Result; $Try++) {
	$Result = Get-ChildItem -Path $StressDir -Filter "Stress_*.csv" -ErrorAction SilentlyContinue |
		Where-Object { $_.LastWriteTime -ge $StartTime.AddSeconds(-5) } | Sort-Object LastWriteTime | Select-Object -Last 1
	if (-not $Result) { Start-Sleep -Seconds 1 }
}
if (-not $Result) {
	Write-Warning "No result file in $StressDir (see Saved/Logs/RogueRigs.log, lines 'Stress')."
	exit 1
}
Write-Host "Results: $($Result.FullName)"
Import-Csv $Result.FullName | Format-Table -AutoSize | Out-String | Write-Host
exit 0
