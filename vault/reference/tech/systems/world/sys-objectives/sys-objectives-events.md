---
id: sys-objectives-events
description: "Blueprint events and HUD locator functions fed by the run flow. Open to wire objective visuals."
parent: sys-objectives
kind: doc
---
# Objectives — events

- Blueprint events (BlueprintImplementableEvent) for the visuals:
  - `ARRConsole`: `OnMarkedRocksChanged(Remaining, Total)` (also at drop), `OnHackProgress(Progress, bHacking)`, `OnHackStateChanged(NewState)`.
  - `ARRBonusChest`: the console events `OnHackProgress`, `OnHackStateChanged` (`OnMarkedRocksChanged(0, 0)` once at drop), plus `OnLidOpened(Location)` (lid opening, task-309-01), `OnRewardGranted(XP, OreChunks, bUpgradeChoiceQueued)` (right after it) and `OnLidSlam(Location)` (start of the lid hold) ([[sys-objectives-console-chest]] chest opening); pure getters `WasRewardGranted`, `GetRewardXP`, `GetXPNeedFraction`, `GetOreChunkCount`, `GetBonusLuck` and the console ones (`GetHackProgress`, `GetZoneRadius`, `GetConsoleState`).
  - `ARRShuttle`: `OnLanded()`, `OnCountdown(Remaining)` (every countdown frame), `OnExtracted()` (win), `OnMissed()` (loss); takeoff (task-370-01): `OnTakeoffStarted()` and `OnTakeoffFinished()` (rotor spin and wash, [[sys-objectives-shuttle]]) and the C++ delegate `OnShuttleTakeoffFinished(ARRShuttle*)` fired just before the shuttle is destroyed.
  - `ARRArenaGrid`: `OnCellMarkedChanged(Cell, bMarked, CellCenter)`; and per-instance custom data float `MarkedCustomDataIndex` (0) = `MarkedCustomDataValue` (1) on the block HISMs, readable by a material through PerInstanceCustomData (pulsing white outline, [[color-code]]).
  - `URRHUDWidget`: `OnAlertChanged`, `OnObjectiveChanged`, `OnObjectiveProgressChanged`, `OnExtractionReady`, `OnShuttleCountdown`, `OnRunEnded`; call `RestartLevel` ([[sys-hud]]).
- HUD locator (task-032-01): `URRHUDWidget::GetObjectiveTarget` / `GetObjectiveLocator` point at the console until it is `Complete`, then at the landed shuttle ([[sys-hud]]).

## Relations
- [[sys-objectives]] — parent hub: summary and verdict.
- [[color-code]] — referenced above.
- [[sys-hud]] — referenced above.
- [[sys-objectives-shuttle]] — the takeoff phases and tunables behind these events.
- [[sys-objectives-console-chest]] — the chest opening timeline behind the lid events.
