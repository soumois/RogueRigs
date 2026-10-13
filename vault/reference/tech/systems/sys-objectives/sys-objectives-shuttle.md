---
id: sys-objectives-shuttle
description: "ARRShuttle: landing cell, countdown and extraction. Open to change the shuttle."
parent: sys-objectives
kind: doc
---
# Objectives — shuttle

  - `ARRShuttle` (Blueprintable, not abstract): spawned on a random reachable Floor cell (footprint clear) straight-line `ShuttleMinDistanceCells`–`ShuttleMaxDistanceCells` from the miner cell (fallbacks above; no reachable cell: retried each tick). Countdown `ShuttleCountdownSeconds` (cheat `ShuttleCountdownOverride` > 0 replaces it at landing, [[sys-cheats]]). Each tick (not paused): miner alive within the board radius (2D) → extracted (win); else countdown −dt, at 0 → missed (loss). Dead miner freezes it. Robots keep spawning meanwhile ([[sys-spawner]] is independent). Mesh NoCollision.

## Relations
- [[sys-objectives]] — parent hub: summary and verdict.
- [[sys-cheats]] — referenced above.
- [[sys-spawner]] — referenced above.
