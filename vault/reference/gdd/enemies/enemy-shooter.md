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
- parts: arm cannon integrated from the elbow; rest open
- size: 1.0 × the miner (provisional, owner 2026-10-04)
- animations: none in grey-box, rigid parts like the other robots (owner 2026-10-04)
- behavior:
  - keeps distance from the miner;
  - fires only with line of sight; when blocked, repositions to find a firing position;
  - its shots stop on rock blocks ([[destruction-rules]]).
- telegraph: a red targeting laser before each shot, shown only with line of sight.
- colors: acid green; red eyes, yellow logo plate; red shots and laser ([[color-code]])
- stats (provisional, owner 2026-10-04): hp 40, speed 300 uu/s, damage 12 per shot, one shot every 2 s, targeting laser 0.6 s before each shot, projectile 1200 uu/s (dodgeable), range 1200 uu (6 cells), preferred distance 800 uu (4 cells) ± 1 cell, xp_value 2, knockback_resistance 0.2.
- fires only while it is on screen; its shots stop on rock without breaking it and never hit other robots (owner 2026-10-04).
- milestone: M2, pulled into the current work (owner 2026-10-04)
- references: `vault/assets/enemies-lineup.jpg`

Must be spotted in under a second: it changes how the player moves.

## Relations
- [[enemies-index]] — parent index.
- [[sys-enemy-ai]] — line of sight and repositioning as built.
