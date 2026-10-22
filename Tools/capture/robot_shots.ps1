# Rogue Rigs - robot capture for visual checks (task-283-01): one -game run per robot (heavy, kamikaze, shooter, chaser),
# robot spawned 400 uu from the miner with rr.Debug.SpawnNear, time frozen with slomo, one game-camera shot with rr.Debug.Shots,
# PNG copied to Saved/RobotMeshes/<Label>/<robot>.png. Runs alongside an open editor; only the -game process it starts is closed.
# Usage: powershell -NoProfile -ExecutionPolicy Bypass -File Tools\capture\robot_shots.ps1 [-Label before|after] [-Robots heavy,kamikaze,shooter,chaser]
param(
	[string]$Label = "before",
	[string[]]$Robots = @("heavy", "kamikaze", "shooter", "chaser"),
	[int]$TimeoutSeconds = 240
)

$ErrorActionPreference = "Stop"
# -File passes "a,b,c" as one string: split it.
$Robots = @($Robots | ForEach-Object { $_ -split "," } | Where-Object { $_ })
$Engine = "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
$Project = Join-Path $PSScriptRoot "..\..\RogueRigs.uproject" | Resolve-Path
$ProjectDir = Split-Path $Project
$Map = "/Game/RogueRigs/Maps/L_ArenaM1"
$Classes = @{
	heavy    = "/Game/RogueRigs/Blueprints/Enemies/BP_Heavy.BP_Heavy_C"
	kamikaze = "/Game/RogueRigs/Blueprints/Enemies/BP_Kamikaze.BP_Kamikaze_C"
	shooter  = "/Game/RogueRigs/Blueprints/Enemies/BP_Shooter.BP_Shooter_C"
	chaser   = "/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C"
}
$OutDir = Join-Path $ProjectDir "Saved\RobotMeshes\$Label"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

foreach ($Robot in $Robots) {
	$Class = $Classes[$Robot]
	if (-not $Class) { Write-Warning "unknown robot '$Robot'"; continue }
	$Prefix = "robotshot_$Robot"
	# Delays are real seconds from the first frame: spawn at 15 s, freeze 0.4 s later, shot at 18 s, quit at 22 s.
	$Exec = "rr.Stress.Peace 1,rr.Debug.Delayed 15 rr.Debug.SpawnNear $Class 400 0,rr.Debug.Delayed 15.4 slomo 0.0001,rr.Debug.Shots 18 1 1 $Prefix,rr.Debug.Delayed 22 Quit"
	Write-Host "[$Robot] UnrealEditor.exe `"$Project`" $Map -game -windowed -ResX=1600 -ResY=900 -nosplash -log -ExecCmds=`"$Exec`""
	$Start = Get-Date
	$Proc = Start-Process -FilePath $Engine -PassThru -ArgumentList @("`"$Project`"", $Map, "-game", "-windowed", "-ResX=1600", "-ResY=900", "-nosplash", "-log", "-ExecCmds=`"$Exec`"")
	if (-not $Proc.WaitForExit($TimeoutSeconds * 1000)) {
		Write-Warning "[$Robot] timeout, closing the -game process $($Proc.Id) only"
		Stop-Process -Id $Proc.Id -Force
	}
	$Shot = Get-ChildItem -Path (Join-Path $ProjectDir "Saved\Screenshots") -Recurse -Filter "${Prefix}_00.png" -ErrorAction SilentlyContinue |
		Where-Object { $_.LastWriteTime -ge $Start } | Select-Object -First 1
	if ($Shot) {
		Copy-Item $Shot.FullName (Join-Path $OutDir "$Robot.png") -Force
		Write-Host "[$Robot] -> $(Join-Path $OutDir "$Robot.png")"
	} else {
		Write-Warning "[$Robot] no screenshot found"
	}
}
