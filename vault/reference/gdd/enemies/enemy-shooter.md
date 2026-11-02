---
id: enemy-shooter
description: "Shooter: acid-green biped with an elbow arm cannon; keeps distance, fires only with line of sight, shows a red targeting laser."
parent: enemies-index
kind: spec
status: approved
---
# Shooter

- role: ranged pressure; forces the miner to move and use rock as cover.
- locomotion: biped
- rig: rigid parts, one bone per part
- parts (provisional, owner 2026-10-22, decided, not built yet: tasks 281 to 288): 10 rigid parts as the chaser, the cannon on the right forearm part (arm cannon integrated from the elbow)
- model_source (provisional, owner 2026-10-22): the owner's Meshy mesh (7263 triangles kept), flattened panels, cleaned texture, base colour only ([[robots-meshy-raw-2026-10-22]]); 176 cm tall (the current capsule)
- size: 1.0 × the miner (provisional, owner 2026-10-04)
- animations (provisional, owner 2026-10-22, decided, not built yet: tasks 281 to 288): walk loop with a light knee bend (at most 15 degrees), rest pose when stopped; aim clip = cannon arm raised over the first half of the laser (`WindUp`), held; fire clip = 0.2 s recoil at the shot. Was: none in grey-box (owner 2026-10-04)
- behavior:
  - keeps distance from the miner;
  - never backs away from the miner (owner 2026-10-06): it closes in until in range with line of sight, then stops and fires, even when the miner comes into contact;
  - fires only with line of sight; when blocked, repositions to find a firing position;
  - its shots stop on rock blocks ([[destruction-rules]]).
- telegraph: a red targeting laser before each shot, shown only with line of sight; during the laser (`WindUp`, `EnemyStats.csv` row `Shooter`) a red charge-up glow grows at the cannon tip (provisional, owner 2026-10-23).
- colors: acid green; red eyes, yellow logo plate; red shots and laser ([[color-code]])
- stats (provisional, owner 2026-10-04): `EnemyStats.csv` row `Shooter`, columns `MaxHealth`, `MoveSpeed` (owner 2026-10-06: every robot moves at the same speed), `Damage` (per shot), `AttackRate`, `WindUp` (targeting laser before each shot), `ProjectileSpeed` (dodgeable), `AttackRange` (owner 2026-10-06: 20 % shorter, was 1200 uu), `PreferredDistance` ± `PreferredDistanceTolerance`, `XPValue`, `KnockbackResistance`.
- fires only while it is on screen; its shots stop on rock without breaking it and never hit other robots (owner 2026-10-04).
- milestone: M2, pulled into the current work (owner 2026-10-04)
- references: `vault/assets/enemies-lineup.jpg`

Must be spotted in under a second: it changes how the player moves.

- Without line of sight (owner 2026-10-07 fifth test): a shooter in range but blocked by rock walks around like the other robots until it reaches a firing position, still never moving away from the miner.

## Relations
- [[enemies-index]] — parent index.
- [[sys-enemy-ai]] — line of sight and repositioning as built.
- [[art-prompts-robots-meshy]] — Meshy prompts and owner turnaround sheet for this robot.
