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
- size: open
- animations: open
- behavior:
  - keeps distance from the miner;
  - fires only with line of sight; when blocked, repositions to find a firing position;
  - its shots stop on rock blocks ([[destruction-rules]]).
- telegraph: a red targeting laser before each shot, shown only with line of sight.
- colors: acid green; red eyes, yellow logo plate; red shots and laser ([[color-code]])
- stats: hp, speed, damage, attack_rate, range, preferred distance → open
- milestone: M2
- references: `vault/assets/enemies-lineup.jpg`

Must be spotted in under a second: it changes how the player moves.

## Relations
- [[enemies-index]] — parent index.
- [[sys-enemy-ai]] — line of sight and repositioning as built.
