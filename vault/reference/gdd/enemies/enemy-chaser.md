---
id: enemy-chaser
description: "Chaser: small yellow biped that runs at the miner and attacks in melee. The basic horde unit."
parent: enemies-index
kind: spec
status: approved
---
# Chaser

- role: basic horde unit; runs at the miner and attacks in melee.
- locomotion: biped
- rig: rigid parts, one bone per part
- parts: open
- size: small; ratio to the miner open
- animations: open
- behavior: moves straight toward the miner; no line-of-sight requirement.
- telegraph: open
- colors: bright company yellow; red eyes, yellow logo plate ([[color-code]])
- stats: hp, speed, damage, attack_rate, range, xp_value, knockback_resistance → open
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`

## Relations
- [[enemies-index]] — parent index.
- [[enemy-death]] — death effect.
