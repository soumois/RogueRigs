# Rogue Rigs - robot capture for visual checks (task-283-01): one -game run per robot (heavy, kamikaze, shooter, chaser),
# robot spawned 400 uu from the miner with rr.Debug.SpawnNear, time frozen with slomo, one game-camera shot with rr.Debug.Shots,
# PNG copied to Saved/RobotMeshes/<Label>/<robot>.png. Runs alongside an open editor; only the -game process it starts is closed.
# -NoFreeze (task-284-11): no slomo, -ShotCount shots every -ShotInterval s from -ShotDelay s after the spawn, copied to <robot>_<i>.png (robots caught walking).
# Usage: powershell -NoProfile -ExecutionPolicy Bypass -File Tools\capture\robot_shots.ps1 [-Label before|after] [-Robots heavy,kamikaze,shooter,chaser,chaserv2] [-NoFreeze -ShotCount 4 -ShotInterval 0.25 -ShotDelay 0.5]
param(
	[string]$Label = "before",
	[string[]]$Robots = @("heavy", "kamikaze", "shooter", "chaser"),
	[int]$TimeoutSeconds = 240,
	[switch]$NoFreeze,
	[int]$ShotCount = 4,
	[double]$ShotInterval = 0.25,
	[double]$ShotDelay = 0.5
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
	chaserv2 = "/Game/RogueRigs/Blueprints/Enemies/BP_ChaserV2.BP_ChaserV2_C"
}
$OutDir = Join-Path $ProjectDir "Saved\RobotMeshes\$Label"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

foreach ($Robot in $Robots) {
	$Class = $Classes[$Robot]
	if (-not $Class) { Write-Warning "unknown robot '$Robot'"; continue }
	$Prefix = "robotshot_$Robot"
	# Delays are real seconds from the first frame: spawn at 15 s, freeze 0.4 s later, shot at 18 s, quit at 22 s.
	$Exec = "rr.Stress.Peace 1,rr.Debug.Delayed 15 rr.Debug.SpawnNear $Class 400 0,rr.Debug.Delayed 15.4 slomo 0.0001,rr.Debug.Shots 18 1 1 $Prefix,rr.Debug.Delayed 22 Quit"
	if ($NoFreeze) {
		# Spawn at 15 s, no freeze, $ShotCount shots from 15 + $ShotDelay s every $ShotInterval s, quit 4 s after the last one.
		$First = [string]::Format([Globalization.CultureInfo]::InvariantCulture, "{0:0.###}", 15 + $ShotDelay)
		$Gap = [string]::Format([Globalization.CultureInfo]::InvariantCulture, "{0:0.###}", $ShotInterval)
		$QuitAt = [int][Math]::Ceiling(15 + $ShotDelay + $ShotInterval * $ShotCount + 4)
		$Exec = "rr.Stress.Peace 1,rr.Debug.Delayed 15 rr.Debug.SpawnNear $Class 400 0,rr.Debug.Shots $First $ShotCount $Gap $Prefix,rr.Debug.Delayed $QuitAt Quit"
	}
	Write-Host "[$Robot] UnrealEditor.exe `"$Project`" $Map -game -windowed -ResX=1600 -ResY=900 -nosplash -log -ExecCmds=`"$Exec`""
	$Start = Get-Date
	$Proc = Start-Process -FilePath $Engine -PassThru -ArgumentList @("`"$Project`"", $Map, "-game", "-windowed", "-ResX=1600", "-ResY=900", "-nosplash", "-log", "-ExecCmds=`"$Exec`"")
	if (-not $Proc.WaitForExit($TimeoutSeconds * 1000)) {
		Write-Warning "[$Robot] timeout, closing the -game process $($Proc.Id) only"
		Stop-Process -Id $Proc.Id -Force
	}
	$Count = if ($NoFreeze) { $ShotCount } else { 1 }
	for ($i = 0; $i -lt $Count; $i++) {
		$Name = "{0}_{1:00}.png" -f $Prefix, $i
		$Dest = if ($NoFreeze) { Join-Path $OutDir ("{0}_{1}.png" -f $Robot, $i) } else { Join-Path $OutDir "$Robot.png" }
		$Shot = Get-ChildItem -Path (Join-Path $ProjectDir "Saved\Screenshots") -Recurse -Filter $Name -ErrorAction SilentlyContinue |
			Where-Object { $_.LastWriteTime -ge $Start } | Select-Object -First 1
		if ($Shot) {
			Copy-Item $Shot.FullName $Dest -Force
			Write-Host "[$Robot] -> $Dest"
		} else {
			Write-Warning "[$Robot] no screenshot $Name found"
		}
	}
}
