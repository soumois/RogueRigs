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
  - never backs away from the miner (owner 2026-10-06): it closes in until in range with line of sight, then stops and fires, even when the miner comes into contact;
  - fires only with line of sight; when blocked, repositions to find a firing position;
  - its shots stop on rock blocks ([[destruction-rules]]).
- telegraph: a red targeting laser before each shot, shown only with line of sight.
- colors: acid green; red eyes, yellow logo plate; red shots and laser ([[color-code]])
- stats (provisional, owner 2026-10-04): hp 40, speed 180 uu/s (owner 2026-10-06: every robot moves at the same speed, 180; before that 151, was 151; owner 2026-10-06 third test: 10 % slower again, was 168; owner 2026-10-06 second test: 30 % slower so the miner has time to dig, was 240; 20 % slower, owner 2026-10-06, was 300), damage 12 per shot, one shot every 2 s, targeting laser 0.6 s before each shot, projectile 1200 uu/s (dodgeable), range 960 uu (about 14 cells of 67 uu; owner 2026-10-06: 20 % shorter, was 1200), preferred distance 800 uu (about 12 cells) ± 1 cell, xp_value 2, knockback_resistance 0.2.
- fires only while it is on screen; its shots stop on rock without breaking it and never hit other robots (owner 2026-10-04).
- milestone: M2, pulled into the current work (owner 2026-10-04)
- references: `vault/assets/enemies-lineup.jpg`

Must be spotted in under a second: it changes how the player moves.

- Without line of sight (owner 2026-10-07 fifth test): a shooter in range but blocked by rock walks around like the other robots until it reaches a firing position, still never moving away from the miner.

## Relations
- [[enemies-index]] — parent index.
- [[sys-enemy-ai]] — line of sight and repositioning as built.
- [[art-prompts-robots-meshy]] — Meshy sheet and view prompts for this robot (not run yet).
