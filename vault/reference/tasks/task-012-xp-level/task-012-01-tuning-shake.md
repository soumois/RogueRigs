---
id: task-012-01-tuning-shake
description: "Kamikaze 100 HP, 300 uu/s, 1.1 × size; chaser 360 uu/s; block shake moves only the rendering, never the collision; tests updated."
parent: task-012-xp-level
kind: task
status: done
---
# Tuning and render-only shake

- goal: the owner tweaks, and shaking blocks that never push the miner.
- agent: cpp
- depends_on: none
- inputs: [[enemy-kamikaze]], [[enemy-chaser]], [[drone]], [[sys-grid]], [[sys-enemies]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `EnemyStats.csv`: `Kamikaze` MaxHealth 100, MoveSpeed 300; `Chaser` MoveSpeed 360. `ARRKamikaze` capsule about 1.1 × the miner (radius and half height scaled from the miner's 34 × 88), `HoverHeight` and `ArmRange` reviewed so the bigger body still arms before touching the miner (document). Grid: a shaking block moves only its rendering; its collision stays at the cell (e.g. split rendering and collision into separate instanced components, or a render-only offset; choose, document, mind the instance maps and destruction). Tests: update tests whose expected values derive from the changed data (same checks); new `RogueRigs.Dig.ShakeKeepsCollision`: while a block shakes, a capsule sweep against its cell face hits at the same position every frame; the miner pushing into a shaking block does not move backward. Docs [[sys-grid]], [[sys-enemies]], [[sys-automation-tests]].
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: grid rendering and collision split: block HISMs render only, one hidden collision ISM per block type kept at rest, maps and swap-remove shared, destruction removes both; shake moves only the render instance (collision 0 uu). CSV Kamikaze 100 HP, 300 uu/s; Chaser 360 uu/s; kamikaze capsule 37.4 × 96.8 (ArmRange 150 still arms before contact). New test Dig.ShakeKeepsCollision (no backward step); data-derived test values updated, RifleKillsKamikazeExplodes start cell moved to (43, 44). Build exit 0, 53/53 (orchestrator re-ran). Kamikaze meshes need the BP pass (task-012-03). About 149k tokens (over ceiling).

## Relations
- [[task-012-xp-level]] — parent global task.
