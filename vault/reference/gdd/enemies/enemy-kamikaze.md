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
  - explodes on proximity or on death, with the same radius and damage both ways; before a proximity explosion its core flares with a strong light for 0.5 s (owner 2026-10-05); a death explosion stays instant;
  - the explosion hurts robots, the miner and terrain ([[explosions]]);
  - it does not target defenses: breaking terrain is emergent, not intended;
  - its explosion destroys every block in the radius ([[explosions]]), so luring it onto a vein is a valid tactic ([[ore-blocks]]);
  - the share of kamikazes per wave controls how long a chokepoint lasts.
- telegraph: the core always pulses; within 250 uu of the miner it arms, stops, the core pulses faster and grows for 0.65 s, then it explodes (owner 2026-10-06; was 150 uu and 0.5 s, owner 2026-10-02)
- colors: dark gunmetal, pulsing orange core; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-02): hp 100 (about 3.3 × a chaser), speed 180 uu/s (owner 2026-10-06: every robot moves at the same speed, 180; before that 151; owner 2026-10-06 third test: 10 % slower again, was 168; owner 2026-10-06 second test: 30 % slower so the miner has time to dig, was 240; 20 % slower, owner 2026-10-06, was 300; slow and tough; owner 2026-10-02, was 15 and 520), explosion radius 312 uu (owner 2026-10-04: +25 %, was 250), character damage 30, terrain: every block in the radius, xp_value 1; knockback_resistance open
- spawn share: set per pack by the wave director, at most 1 kamikaze per pack in one pack out of two on average, 1–2 per horde ([[enemies-index]], owner 2026-10-07; the M1 rule was 25 % of spawned robots)
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`

- Trigger (owner 2026-10-06 fourth test, provisional): it arms when the miner comes within 250 uu (was 150), stops dead once armed, and explodes 0.65 s later (was 0.5 s).

## Relations
- [[enemies-index]] — parent index.
- [[destruction-rules]] — it is one of three block-destruction sources.
