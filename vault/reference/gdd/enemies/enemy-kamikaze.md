---
id: enemy-kamikaze
description: "Kamikaze: hovering robot that charges the miner and explodes on proximity or death, hurting robots, miner and terrain."
parent: enemies-index
kind: spec
status: approved
---
# Kamikaze

- role: charges the miner and explodes. Its explosions open chokepoints, so no position holds forever.
- locomotion: hover
- rig: rigid parts, one bone per part
- parts: open
- size: 1.1 × the miner, hovering (owner 2026-10-02, was 0.6)
- animations: open
- behavior:
  - explodes on proximity or on death, with the same radius and damage both ways;
  - the explosion hurts robots, the miner and terrain ([[explosions]]);
  - it does not target defenses: breaking terrain is emergent, not intended;
  - its explosion destroys every block in the radius ([[explosions]]), so luring it onto a vein is a valid tactic ([[ore-blocks]]);
  - the share of kamikazes per wave controls how long a chokepoint lasts.
- telegraph: the core always pulses; within 150 uu of the miner it arms: the core pulses faster and grows for 0.5 s, then it explodes (provisional, owner 2026-10-02)
- colors: dark gunmetal, pulsing orange core; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-02): hp 100 (about 3.3 × a chaser), speed 300 uu/s (slow and tough; owner 2026-10-02, was 15 and 520), explosion radius 250 uu, character damage 30, terrain: every block in the radius, xp_value 1; knockback_resistance open
- spawn share (M1): 25 % of spawned robots
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`

## Relations
- [[enemies-index]] — parent index.
- [[destruction-rules]] — it is one of three block-destruction sources.
