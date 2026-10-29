---
id: sys-objectives-shuttle
description: "ARRShuttle: landing cell, countdown and extraction. Open to change the shuttle."
parent: sys-objectives
kind: doc
---
# Objectives — shuttle

  - `ARRShuttle` (Blueprintable, not abstract): spawned on a random reachable Floor cell (footprint clear) straight-line `ShuttleMinDistanceCells`–`ShuttleMaxDistanceCells` from the miner cell (fallbacks above; no reachable cell: retried each tick). Countdown `ShuttleCountdownSeconds` (cheat `ShuttleCountdownOverride` > 0 replaces it at landing, [[sys-cheats]]). Each tick (not paused): miner alive within the board radius (2D) → extracted (win); else countdown −dt, at 0 → missed (loss). Dead miner freezes it. Robots keep spawning meanwhile ([[sys-spawner]] is independent). Mesh NoCollision.
  - Visual components (task-369-02, bc6422d1): `Mesh` is the extractor body; protected `RotorFL`, `RotorFR`, `RotorRL`, `RotorRR` (`UStaticMeshComponent`, VisibleAnywhere, BlueprintReadOnly) are attached to `Mesh`, NoCollision, no overlap events, no navigation impact, empty mesh and zero location until the Blueprint fills them ([[bp-extraction-shuttle]]). Public BlueprintPure `GetRotors()` returns them in the order FL, FR, RL, RR. Board radius and the rule are unchanged (3 cells = 201 uu). Tests RogueRigs.Extraction and RunFlow 8/8, Integration.FineConsoleShuttlePlacement pass.
  - Takeoff (task-370-01, 2e1bde02): `StartTakeoff()` (BlueprintCallable, category Shuttle|Takeoff; no-op if already taking off; callable from C++, reserved for the arrival of task-371), `IsTakingOff()`, `GetTakeoffSeconds()`. It sets `SetTickableWhenPaused(true)` only then, so the shuttle ticks while the game is paused behind the end screen; the countdown and board rule stay stopped. Phases: spin-up `SpinUpSeconds`; vertical rise `RiseHeight` over `RiseSeconds` (smooth ease); nose-down tilt of `PitchDegrees` over `TiltSeconds`; then flight speed grows by `ForwardAcceleration` × dt × tilt alpha along the nose (+X). Removal once, after the tilt, none of the 8 bounds corners projects inside the player 0 viewport (or `TakeoffMaxSeconds` elapsed): `OnTakeoffFinished` event, delegate `OnShuttleTakeoffFinished(ARRShuttle*)`, then `Destroy`. Blueprint events `OnTakeoffStarted` and `OnTakeoffFinished` (BlueprintImplementableEvent) are where the Blueprint spins the rotors and plays the wash; not implemented yet in [[bp-extraction-shuttle]].
  - Takeoff tunables (EditDefaultsOnly on `ARRShuttle`): `SpinUpSeconds` 0.5, `RiseHeight` 400 uu, `RiseSeconds` 1.5, `PitchDegrees` 20, `TiltSeconds` 0.4 (first five from the owner request, [[extraction]] Q3 values); `ForwardAcceleration` 1500 uu/s², `TakeoffMaxSeconds` 12, and the removal rule: provisional choices of the cpp agent, not in the gdd, need owner confirmation.
  - Hidden miner (task-370-01): `EndRun(true)` calls `ARRMiner::HideForExtraction()` then `StartTakeoff()` right after `EnterEndPause` (screen timing unchanged): miner and drone hidden in game, collision off, drone `ReleaseDig`. `IsHiddenForExtraction()` reads it; `ShowAfterExtraction()` undoes it and is called by `PlaceMinerAtStart` ([[sys-run-transition]]).
  - Arrival mode (task-371-01, 66b2c5e2, be525566): `InitializeArrival(Grid, Cell)` places a landed extractor at the footprint centre and sets `bArrival` (`IsArrival()`, BlueprintPure, for the Blueprint to hide chevrons or glow later); `bInitialized` stays false, so no board check, no countdown, no `OnLanded`. It takes off through `StartTakeoff()`; placement and trigger live in [[sys-objectives-run-flow]]. Not verified: whether `BP_ExtractionShuttle` shows chevrons or glow by default in arrival mode (`open`, BP work later).
  - Tests: RogueRigs.Extraction.Takeoff added, OverlapWin extended.

## Relations
- [[sys-objectives]] — parent hub: summary and verdict.
- [[sys-cheats]] — referenced above.
- [[sys-spawner]] — referenced above.
- [[bp-extraction-shuttle]] — the Blueprint that sets the body and rotor meshes.
- [[sys-objectives-run-flow]] — EndRun starts the takeoff and hides the miner.
- [[sys-objectives-events]] — the takeoff events and delegate.
- [[sys-run-transition]] — PlaceMinerAtStart shows the miner again.
- [[extraction]] — the gdd rule.
