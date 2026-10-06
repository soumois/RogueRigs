---
id: enemy-heavy
description: "Heavy: slow, tough steel-blue biped, twice the size, with hazard stripes and high knockback resistance. Not a boss."
parent: enemies-index
kind: spec
status: approved
---
# Heavy

- role: slow robot with high health; a tougher regular enemy, not a boss.
- locomotion: biped
- rig: rigid parts, one bone per part
- parts: open
- size: twice the chaser visually; collision capsule radius at most 80 uu so it fits 3-cell corridors at 67 uu (owner 2026-10-04; corridor width owner, after handover-2026-10-06)
- animations: none in grey-box; the telegraph uses a body tilt and a glow (owner 2026-10-04)
- behavior: ground slam (owner 2026-10-04): stops, raises its arms, eyes and stripes glow for the telegraph, then a shockwave around it hits the miner; the shockwave never breaks rock ([[destruction-rules]]). High knockback resistance, due to its mass. Passes through every 3-cell corridor ([[arena-grid]]).
- telegraph: 0.8 s arms raised with eyes and stripes glowing (owner 2026-10-04); starts when the miner is within 0.7 × the slam radius (280 uu, task-028-01)
- colors: steel blue with yellow and black hazard stripes; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-04): hp 300, speed 160 uu/s (owner 2026-10-06: 20 % slower, was 200), slam damage 25, slam radius 400 uu (owner 2026-10-05: the area must be large; was 220), the slam area shown on the ground during the telegraph, one slam every 2.5 s, xp_value 5, knockback_resistance 0.8; at most 5 alive.
- milestone: M3, pulled into the current work (owner 2026-10-04)
- references: `vault/assets/enemies-lineup.jpg`

Glowing weak point on its back: [[wishlist]].

## Relations
- [[enemies-index]] — parent index.
