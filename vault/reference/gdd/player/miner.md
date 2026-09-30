---
id: miner
description: "The player character: free movement, mouse or stick aim, auto-firing weapons, turret button, damage rules, look and asset source."
parent: player-index
kind: spec
status: approved
---
# Miner

- Moves freely; aims with the mouse or the right stick.
- Every equipped weapon fires automatically toward the aim, each at its own rate ([[upgrades]] change the rates).
- A button places a [[turret]] on the aimed spot.
- Cannot shoot through or over rock blocks ([[destruction-rules]]).
- Standing against a block makes the [[drone]] dig it.
- Explosions never one-shot the miner ([[explosions]]).
- Invulnerability frames after a hit: provisional; duration open.
- Stats: health, move speed → open.
- Death ends the run ([[run-structure]]).
- Look: plate armor, white + teal + rust, patched salvage ([[color-code]]).
- Asset: Tripo (auto-rig); reference `vault/assets/rigs-lineup.jpg`.
- Milestone: M1.

## Relations
- [[player-index]] — parent index.
- [[drone]] — the miner's helper.
