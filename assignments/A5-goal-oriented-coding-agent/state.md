# Code Architect — state

Read by the agent at the start of every run, updated at the end (or at a failure). The owner may edit any line; the next run only knows what is written here. Full reasoning is in [blackboard.md](blackboard.md).

## Built
- Run 1 (2026-10-14): **ore counter** (`ore-economy`, `extraction`), global task `task-183-ore-counter`, done. Carried ore counted, secured on extraction (game instance), lost on a missed shuttle or death; HUD counter with icon right of the minimap; secured ore on the success screen; cheat `rr.Cheat.GrantOre <N>`. Final agent commit `17083710`. Unverified: on-screen look, missed-shuttle loss at runtime.

## Failed
None. (Run 1: one Architect commit silently failed during a parallel owner commit; redone. Blueprint DSL writes retried inside the bp agent.)

## Left (ranked in run 1; rescan before reusing)
| Priority | Feature (spec) | Score | Blocker |
|---|---|---|---|
| 1 | Barrel particle burst on destruction (barrel) | 5 | none; mostly a Niagara asset (bp) |
| 2 | Deterministic C++ arena generator (arena-grid) | 5 | none; very large |
| 3 | Drone as the dialogue channel, barks shown (drone, crew-characters) | 4 | barks are M3 |
| 4 | Stat sheet UI (player-stats) | 3 | passes no pillar test |
| 5 | Damage-number on/off (damage-numbers) | 3 | no options menu |
| 6 | Destroyed-block run log (destruction-rules) | 3 | sys-logging not designed |
| 7 | Shooter hold tolerance ±1 cell (enemy-shooter) | 3 | tuning, owner's feel |
| — | Threat counter and scaling; tier pool; special ore; effect cards; magazine upgrade; ore-loss share | — | `open` values or values not given |
| — | 4-level run, level carry-over | — | blocked by the C++ generator or extra maps |
| — | EMP; audio | — | status effects; sound assets |

Correction from run 1: the locator distance in metres is already built (Blueprint `UpdateLocatorView`).

## Decisions
- Run 1: ore counter approved by the owner ("Ok"), defaults accepted: secured ore in the game instance, death loses carried ore, one dedicated widget. Placement changed by the owner: inline counter at the top, right of the minimap, larger than a typical gold counter.
- Run 1: the owner asked for an in-game render before the icon; render 2 and icon 2 chosen; their style-guide fails (A1 gloss, magenta lean, no glow) kept on purpose.
- Rule from the owner (run 1): in the blackboard, owner quotes stay verbatim and are followed by an English translation.
- Reported, not acted on: `sys-difficulty` names a missing `URRGE_EnemyDifficulty`; heavy cap 3 (Blueprint) vs 5 (code comment, sys-spawner); critical number orange-gold vs white-gold in color-code; missing `#include "Weapons/RRWeapon.h"` in `Upgrades/RRUpgradeCard.h`; `wbp-alert-objectives` purpose line still says z 20.
