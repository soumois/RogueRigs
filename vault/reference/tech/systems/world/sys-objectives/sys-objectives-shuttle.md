---
id: sys-objectives-shuttle
description: "ARRShuttle: landing cell, countdown and extraction. Open to change the shuttle."
parent: sys-objectives
kind: doc
---
# Objectives — shuttle

  - `ARRShuttle` (Blueprintable, not abstract): spawned on a random reachable Floor cell (footprint clear) straight-line `ShuttleMinDistanceCells`–`ShuttleMaxDistanceCells` from the miner cell (fallbacks above; no reachable cell: retried each tick). Countdown `ShuttleCountdownSeconds` (cheat `ShuttleCountdownOverride` > 0 replaces it at landing, [[sys-cheats]]). Each tick (not paused): miner alive within the board radius (2D) → extracted (win); else countdown −dt, at 0 → missed (loss). Dead miner freezes it. Robots keep spawning meanwhile ([[sys-spawner]] is independent). Mesh NoCollision.
  - Visual components (task-369-02, bc6422d1): `Mesh` is the extractor body; protected `RotorFL`, `RotorFR`, `RotorRL`, `RotorRR` (`UStaticMeshComponent`, VisibleAnywhere, BlueprintReadOnly) are attached to `Mesh`, NoCollision, no overlap events, no navigation impact, empty mesh and zero location until the Blueprint fills them ([[bp-extraction-shuttle]]). Public BlueprintPure `GetRotors()` returns them in the order FL, FR, RL, RR. Board radius and the rule are unchanged (3 cells = 201 uu). Tests RogueRigs.Extraction and RunFlow 8/8, Integration.FineConsoleShuttlePlacement pass.

## Relations
- [[sys-objectives]] — parent hub: summary and verdict.
- [[sys-cheats]] — referenced above.
- [[sys-spawner]] — referenced above.
- [[bp-extraction-shuttle]] — the Blueprint that sets the body and rotor meshes.
