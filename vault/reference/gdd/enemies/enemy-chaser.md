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
- stats (provisional, owner 2026-10-02): hp 30, speed 288 uu/s (owner 2026-10-06: 20 % slower, was 360; 20 % slower, owner 2026-10-02, was 450), damage 10, attack_rate 1/s, range 120 uu, xp_value 1, knockback_resistance 0; parts and animations open
- grey box (M1): yellow capsule with a red eye
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`

## Shield variant (provisional, owner 2026-10-05)
- A chaser with a 30-point shield that absorbs damage before health; translucent pale blue-white shell that breaks; xp 2; mixed into chaser packs (25 %) after 1:00. Inspired by the DRGS red swarmers.
- Same hp, speed and damage as the chaser; the shield never regenerates; a hit larger than the remaining shield carries the rest into health. From 1:00, chaser packs hold 2–4 shield chasers among 8–16 chasers (25 % on average); lone chasers, the heavy escort and hordes stay plain chasers (provisional, task-028-01).

## Relations
- [[enemies-index]] — parent index.
- [[enemy-death]] — death effect.
