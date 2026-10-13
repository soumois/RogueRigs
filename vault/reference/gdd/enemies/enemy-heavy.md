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
- telegraph: 0.6 s arms raised with eyes and stripes glowing (owner 2026-10-06 third test: attacks a bit faster, was 0.8 s; owner 2026-10-04); starts when the miner is within 0.7 × the slam radius (202 uu since the 289 uu radius; task-028-01)
- colors: steel blue with yellow and black hazard stripes; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-04): hp 300, speed 153 uu/s (owner 2026-10-07: 15 % slower than the others, was 180; owner 2026-10-06: every robot at the same speed, 180; before that 151, was 101; owner 2026-10-06 third test: 10 % slower again, was 112; owner 2026-10-06 second test: 30 % slower so the miner has time to dig, was 160; 20 % slower, owner 2026-10-06, was 200), slam damage 25, slam radius 289 uu (owner 2026-10-07: 15 % smaller again, was 340; owner 2026-10-07 fifth test: 15 % smaller, was 400; owner 2026-10-05: the area must be large; was 220), the slam area shown on the ground during the telegraph, one slam every 2 s (owner 2026-10-06 third test, was 2.5 s), xp_value 5, knockback_resistance 0.8; at most 3 alive (owner 2026-10-07; was 5).
- milestone: M3, pulled into the current work (owner 2026-10-04)
- references: `vault/assets/enemies-lineup.jpg`

Glowing weak point on its back: [[wishlist]].

- Slam ground signal (owner 2026-10-12): during the telegraph the slam area is drawn on the ground (orange danger circle at the slam radius, 289 uu) and a second circle grows inside it and reaches the edge when the slam lands; it disappears at once if the heavy dies ([[ground-telegraph]]).

## Relations
- [[enemies-index]] — parent index.
