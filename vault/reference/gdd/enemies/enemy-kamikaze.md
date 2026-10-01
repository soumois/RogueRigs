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
- size: open
- animations: open
- behavior:
  - explodes on proximity or on death, with the same radius and damage both ways;
  - the explosion hurts robots, the miner and terrain ([[explosions]]);
  - it does not target defenses: breaking terrain is emergent, not intended;
  - one explosion breaks one ore block, so luring it onto a vein is a valid tactic ([[ore-blocks]]);
  - the share of kamikazes per wave controls how long a chokepoint lasts.
- telegraph: open (the core always pulses)
- colors: dark gunmetal, pulsing orange core; red eyes, yellow logo plate ([[color-code]])
- stats: hp, speed, explosion radius, explosion damage (terrain and characters), xp_value, knockback_resistance → open
- milestone: M1
- references: `vault/assets/enemies-lineup.jpg`

## Relations
- [[enemies-index]] — parent index.
- [[destruction-rules]] — it is one of three block-destruction sources.
