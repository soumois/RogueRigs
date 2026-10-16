# Code Architect — state

Read by the agent at the start of every run, updated at the end (or at a failure). The owner may edit any line; the next run only knows what is written here. Full reasoning is in [blackboard.md](blackboard.md).

## Built
- Run 1 (2026-10-14): **ore counter** (`ore-economy`, `extraction`), global task `task-183-ore-counter`, done. Carried ore counted, secured on extraction (game instance), lost on a missed shuttle or death; HUD counter with icon right of the minimap; secured ore on the success screen; cheat `rr.Cheat.GrantOre <N>`. Final agent commit `17083710`. Unverified: on-screen look, missed-shuttle loss at runtime.
- Run 2 (2026-10-16): **barrel explosion feedback** (`barrel`, owner rule 2026-10-16), global task `task-197-barrel-explosion-feedback` plus owner follow-ups `task-198` (ring 60 uu, own 0.8 s timeline after the sphere), `task-199` (bigger burst, 95 particles), `task-200` (check), `task-201` (green default-decal first frame fixed by a decal material warm-up, `ARRDecalWarmUp`), `task-202` (black crater first frame fixed: floor HISM render state dirtied on `AddInstance`), all done. During the fuse the barrel shakes (render only) and blinks orange to white-hot faster and faster; its explosion plays `NS_BarrelExplosion` and an orange ground shockwave ring with the sphere, through `BP_ExplosionFX_Barrel` (Barrel row only; kamikaze unchanged, can take it with one `ExplosionStats` line). Final agent commit `be077028`. Seen in `-game` frames: blink, danger circle, sphere, ring, burst, smoke, no green and no black frame. Unverified: the shake (stills), cooked build and cold shader cache for the warm-up, proxy rebuild cost in long chains; faint darker cell patches in a fresh crater floor reported, not acted on.

## Failed
None. (Run 1: one Architect commit silently failed during a parallel owner commit; redone. Run 2: no MCP route to call a UFunction in PIE, so the PIE read-backs of subtasks 03 and 06 were replaced by the `-game` frames of subtask 07.)

## Left (ranked in run 2; rescan before reusing)
| Priority | Feature (spec) | Score | Blocker |
|---|---|---|---|
| 1 | Drone shows the barks (drone, crew-characters, lore L2) | 5 | none; several subtasks |
| 2 | Shield-chaser pack makeup 2–4 among 8–16 (enemy-chaser) | 5 | check it is not an owner tuning first |
| 3 | Deterministic C++ arena generator (arena-grid) | 5 | none; very large |
| 4 | Shooter tolerance ±1 cell (enemy-shooter, CSV 200 vs 67) | 4 | owner's feel |
| 5 | Bonus chest gold icon on the alert bar (objective-bonus-chest) | 4 | none |
| 6 | Kamikaze yellow logo plate (color-code) | 4 | passes no pillar |
| 7 | Void stars and planet (visual-style) | 3 | art assets |
| 8 | Stat sheet UI (player-stats) | 3 | passes no pillar |
| — | Threat; tier pool; special ore; ore-loss share; run-structure objective and ramp; magazine and turret-charge upgrades; effect cards | — | `open` values |
| — | 4-level run, level carry-over | — | no run controller, `open` values |
| — | Damage-number toggle; run log; EMP; audio; miner look; robot parts | — | options menu; sys-logging; stun; sound assets; Tripo asset; parts `open` |

## Decisions
- Run 1: ore counter approved by the owner ("Ok"), defaults accepted: secured ore in the game instance, death loses carried ore, one dedicated widget. Placement changed by the owner: inline counter at the top, right of the minimap, larger than a typical gold counter.
- Run 1: the owner asked for an in-game render before the icon; render 2 and icon 2 chosen; their style-guide fails (A1 gloss, magenta lean, no glow) kept on purpose.
- Rule from the owner (run 1): in the blackboard, owner quotes stay verbatim and are followed by an English translation.
- Run 2: barrel approved and widened by the owner (unstable during the fuse, explosion particles, explosion circle, reusable for the kamikaze); defaults accepted with "Ok": shake + blink, burst on top of the sphere, ground shockwave ring, kamikaze later.
- Rule from the owner (run 2): no one-click test cheat ("Pas besoin de l'instruction test en un clic"); instead a visual check in game. Applies to the architect step 3 proposal from now on.
- Run 2 follow-ups: owner "Ok" to a wider and longer ring, a bigger burst and a second run; then "Oui corriger les points" for the green decal frame and the black crater (two tasks, split rule).
- Reported, not acted on: `sys-difficulty` names a missing `URRGE_EnemyDifficulty`; heavy cap 3 (Blueprint) vs 5 (code comment, sys-spawner); critical number orange-gold vs white-gold in color-code; mining laser red vs white-gold; big XP chip lilac core vs magenta trail; `bp-arena-grid` CellSize 200 / `arena_m1` stale; `RREnemy` `DebrisPoolSize` 40 vs 80 in the Blueprint; missing `#include "Weapons/RRWeapon.h"` in `Upgrades/RRUpgradeCard.h`; `wbp-alert-objectives` purpose line still says z 20; faint darker cell patches in a fresh crater floor; pre-existing test failures `Ore.HealVein` (flaky), `Enemies.ShotHitsMovingRobotAndDeathShowsParts`, `Integration.JuiceHealRing`.
