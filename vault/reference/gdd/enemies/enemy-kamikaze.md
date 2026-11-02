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
- parts (provisional, owner 2026-10-22, decided, not built yet: tasks 281 to 288): 3 rigid parts: body (head, arms, core shell, vanes) and 2 lower legs; the core is a separate mesh so it keeps its pulse; death debris = these 3 pieces, 2 or 3 thrown
- model_source (provisional, owner 2026-10-22): the owner's Meshy mesh (10172 triangles kept), flattened panels, cleaned texture, base colour only ([[robots-meshy-raw-2026-10-22]]); 194 cm tall
- size: 1.1 × the miner, hovering (owner 2026-10-02, was 0.6)
- animations (provisional, owner 2026-10-22, decided, not built yet: tasks 281 to 288): it keeps hovering, no walk; only a very light leg sway loop (at most 8 degrees)
- behavior:
  - explodes on proximity or on death, with the same radius and damage both ways; before a proximity explosion its core flares with a strong light for the fuse (`EnemyStats.csv` row `Kamikaze`, column `WindUp`; owner 2026-10-05; 0.5 s before 2026-10-06); killed by the player (weapon, turret, drone) it does not explode at once: it goes through the same preparation as a proximity trigger (stops, core flares, the same fuse) and then explodes, and it releases its XP only at the explosion, never before (owner 2026-10-10); killed by an explosion (chain reaction, barrel) its death explosion stays instant;
  - the explosion hurts robots, the miner and terrain ([[explosions]]);
  - it does not target defenses: breaking terrain is emergent, not intended;
  - its explosion destroys every block in the radius ([[explosions]]), so luring it onto a vein is a valid tactic ([[ore-blocks]]);
  - the share of kamikazes per wave controls how long a chokepoint lasts.
- telegraph: the core always pulses; within `AttackRange` (`EnemyStats.csv` row `Kamikaze`) of the miner it arms, stops, the core pulses faster and grows for `WindUp`, then it explodes (owner 2026-10-06; was 150 uu and 0.5 s, owner 2026-10-02)
- colors: dark gunmetal, pulsing orange core; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-02): `EnemyStats.csv` row `Kamikaze`, columns `MaxHealth`, `MoveSpeed`, `XPValue` (owner 2026-10-06: every robot moves at the same speed; slow and tough; owner 2026-10-02, was 15 and 520); explosion: `ExplosionStats.csv` row `Kamikaze`, columns `Radius` (owner 2026-10-04: +25 %), `Damage` (robots) and `MinerDamage` (provisional, owner 2026-10-23; was the same for every character), terrain: every block in the radius; knockback_resistance open
- spawn share: set per pack by the wave director, at most 1 kamikaze per pack in one pack out of two on average, 1–2 per horde ([[enemies-index]], owner 2026-10-07; the M1 rule was 25 % of spawned robots)
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`

- Fuse shake (provisional, owner 2026-10-23): during the fuse (`WindUp`) the whole body shakes lightly like a rock wall being dug, the shake speeding up over the fuse; the core keeps growing and pulsing at the same time.
- Crowd spacing (provisional, owner 2026-10-23): its separation radius from other robots is 30 % larger than its body radius.
- Trigger (owner 2026-10-06 fourth test, provisional): it arms when the miner comes within `AttackRange` (was 150 uu), stops dead once armed, and explodes `WindUp` later (was 0.5 s), `EnemyStats.csv` row `Kamikaze`.

## Relations
- [[enemies-index]] — parent index.
- [[destruction-rules]] — it is one of three block-destruction sources.
- [[art-prompts-robots-meshy]] — Meshy prompts and owner turnaround sheet for this robot.
