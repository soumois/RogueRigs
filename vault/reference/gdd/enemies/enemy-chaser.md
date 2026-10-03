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
- size: small; 0.7 × the miner (provisional, owner 2026-10-02)
- animations: open
- behavior: moves straight toward the miner; no line-of-sight requirement.
- telegraph: 0.1 s wind-up (the robot stops and flashes) before each hit (owner 2026-10-02, was 0.25 s)
- colors: bright company yellow; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-02): hp 30, speed 360 uu/s (20 % slower, owner 2026-10-02, was 450), damage 10, attack_rate 1/s, range 120 uu, xp_value 1, knockback_resistance 0; parts and animations open
- grey box (M1): yellow capsule with a red eye
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`

## Relations
- [[enemies-index]] — parent index.
- [[enemy-death]] — death effect.
