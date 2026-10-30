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
- A button places a turret on the aimed spot, using the turret launcher ([[turret]]).
- Cannot shoot through or over rock blocks ([[destruction-rules]]).
- Standing against a block makes the [[drone]] dig it.
- Explosions never one-shot the miner ([[explosions]]).
- Invulnerability frames after a hit: 0.25 s, the miner blinks (provisional, owner 2026-10-02).
- Stats: see [[player-stats]].
- Healing: only from heal pickups dropped by heal ore ([[heal-ore]]).
- Death ends the run ([[run-structure]]). M1 (single arena): controls cut, "Game Over" text, the level restarts after 3 s (owner 2026-10-02).
- Look: plate armor, white + teal + rust, patched salvage ([[color-code]]).
- Asset: the owner makes the 3D mesh himself from a new turnaround sheet generated in ChatGPT (owner 2026-10-29, task-364; prompt in art-prompts-miner); rig: Meshy auto-rig made by the owner, Mixamo-named humanoid skeleton (`mixamorig:*`, fingers included), delivered 2026-10-30 as `Meshy_AI_Character_output.fbx` (task-382, files in art-prompts-miner); animation: Mixamo clips on this skeleton, played in place by a new ABP_Miner, movement from gameplay (owner 2026-10-30, task-386); Unreal import and replacement of the Quinn mannequin in task-386; triangle budget 25,744 as delivered (provisional, owner 2026-10-30); weapon hand socket open; reference `vault/assets/rigs-lineup.jpg`.
- Milestone: M1.

## Relations
- [[player-index]] — parent index.
- [[drone]] — the miner's helper.
- [[aim-indicator]] — ground chevrons showing the aim direction.
