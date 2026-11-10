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
- parts (provisional, owner 2026-10-22, decided, not built yet: tasks 281 to 288): 10 rigid parts as the chaser: head and torso, pelvis, 2 upper arms, 2 forearms with fist, 2 thighs, 2 lower legs with foot; the same parts feed the death debris ([[enemy-death]])
- model_source (provisional, owner 2026-10-22): the owner's Meshy mesh (10361 triangles kept, no decimation), flattened panels, cleaned texture, base colour only ([[robots-meshy-raw-2026-10-22]]); 248 cm tall (the current capsule)
- size: twice the chaser visually; collision capsule radius at most 80 uu so it fits 3-cell corridors at 67 uu (owner 2026-10-04; corridor width owner, after handover-2026-10-06)
- animations (provisional, owner 2026-10-22, decided, not built yet: tasks 281 to 288): baked part animation as the chaser: walk loop with a light knee bend (at most 15 degrees); telegraph clip = fists raised over most of the telegraph (`WindUp`, `EnemyStats.csv` row `Heavy`; provisional, owner 2026-10-23: the raise is stretched to the longer telegraph; was 0.45 s of 0.6 s), then the strike over its last 0.15 s: knees bend and the body leans forward so both fists touch the ground on the slam frame; recover clip 0.5 s back to rest. The clip replaces the body tilt; on the Meshy mesh only the eyes glow during the telegraph (the stripes are texture). Was: none in grey-box, body tilt and glow (owner 2026-10-04)
- behavior: ground slam (owner 2026-10-04): stops, raises its arms, eyes and stripes glow for the telegraph, then a shockwave around it hits the miner; the shockwave never breaks rock ([[destruction-rules]]). High knockback resistance, due to its mass. Passes through every 3-cell corridor ([[arena-grid]]).
- telegraph: arms raised with eyes and stripes glowing for `WindUp` (`EnemyStats.csv` row `Heavy`; provisional, owner 2026-10-23: "passe sa préparation à 1 sec", was 0.6 s; owner 2026-10-06 third test: attacks a bit faster, was 0.8 s; owner 2026-10-04); starts when the miner is within 0.7 × the slam radius (`AttackRange`; owner 2026-10-23; task-028-01)
- colors: steel blue with yellow and black hazard stripes; red eyes, yellow logo plate ([[color-code]])
- stats (provisional, owner 2026-10-04): `EnemyStats.csv` row `Heavy`, columns `MaxHealth`, `MoveSpeed` (owner 2026-10-07: 15 % slower than the others), `Damage` (slam), `AttackRange` (slam radius; provisional, owner 2026-10-23: attack 25 % larger, was 289; owner 2026-10-07: 15 % smaller again, was 340; owner 2026-10-07 fifth test: 15 % smaller, was 400; owner 2026-10-05: the area must be large; was 220), the slam area shown on the ground during the telegraph, `AttackRate` (one slam per; owner 2026-10-06 third test, was 2.5 s), `XPValue`, `KnockbackResistance`; at most 3 alive (owner 2026-10-07; was 5).
- milestone: M3, pulled into the current work (owner 2026-10-04)
- references: `vault/assets/enemies-lineup.jpg`

Glowing weak point on its back: [[wishlist]].

- Slam ground signal (owner 2026-10-12): during the telegraph the slam area is drawn on the ground (orange danger circle at the slam radius, `AttackRange`) and a second circle grows inside it and reaches the edge when the slam lands; it disappears at once if the heavy dies ([[ground-telegraph]]).
- Crowd spacing (provisional, owner 2026-10-23): its separation radius from other robots is 30 % larger than its body radius.
- Slam impact (provisional, owner 2026-10-23): when the slam lands, a dust ring and rock chips burst from the ground, sized to the slam radius. Look (owner 2026-11-10, provisional, [[task-498-heavy-slam-impact-fx]]): an orange cartoon dust ring on the ground in the style of the explosions ([[task-434-kamikaze-burst-and-ring]], [[task-446-explosions-red]]) and rock chips in the rock and floor violets; on every slam, hit or not; not when the heavy dies during the telegraph.
- Slam shake (provisional, owner 2026-11-02, [[task-419-camera-shake-on-heavy-slam]]): the slam frame shakes the camera whether or not the miner is hit, not during the telegraph; 8 uu at the heavy, 0.3 s; full at the heavy, 50 % at the max distance, 0 beyond; max distance 2 × the slam radius (`AttackRange`). Mixed with the hit shake ([[visual-style]]).
- Death puff (provisional, owner 2026-10-23): a larger puff than the other robots, so the death shrink of this big robot is hidden ([[enemy-death]]).
- Packs (provisional, owner 2026-10-23): at most 1 heavy per pack, so heavies come from different sides; pack weights unchanged, so fewer heavies per run; in hordes the 2–3 heavies stay but are spread along the front, not side by side; same rule on the last level; at most 3 alive unchanged ([[enemies-index]]).

## Relations
- [[enemies-index]] — parent index.
- [[art-prompts-robots-meshy]] — Meshy prompts and owner turnaround sheet for this robot.
