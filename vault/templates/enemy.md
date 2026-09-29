---
id: enemy-<name>
description: "<one sentence: what this robot does and when to open this node>"
parent: enemies-index
kind: spec
status: open
---
# <Robot name>

- role: <what it does in a fight>
- locomotion: biped | hover | wheeled | static
- rig: rigid parts, one bone per part
- parts: <list of named parts>
- size: <relative to the miner, e.g. 1.1x>
- animations: <idle, move, attack, hit_react, death…>
- behavior: <targeting, line of sight, special rules>
- telegraph: <what warns the player before it acts>
- colors: <body, joints, markers> (see color-code)
- stats: hp, speed, damage, attack_rate, range → values or `open`
- milestone: M1 | M2 | M3
- references: <asset file in vault/assets/>

## Relations
- [[enemies-index]] — parent index.
