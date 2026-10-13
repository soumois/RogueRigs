---
id: task-138-01-xp-pickups-perf-test
description: "Add the rr.Stress.Pickups and rr.Stress.ClearPickups console commands that spawn XP nuts and chips for the cost measurement."
parent: task-138-xp-pickups-perf-test
kind: task
status: todo
---
# XP pickups: add the stress command

- goal: Add the rr.Stress.Pickups and rr.Stress.ClearPickups console commands that spawn XP nuts and chips for the cost measurement.
- agent: cpp
- depends_on: none
- inputs: sys-stress (vault/reference/tech/systems/tools/sys-stress.md), sys-stress-run-modes (vault/reference/tech/systems/tools/sys-stress/sys-stress-run-modes.md), sys-stress-diagnostics (vault/reference/tech/systems/tools/sys-stress/sys-stress-diagnostics.md), perf-stress-test-m1 (vault/reference/tech/performance/perf-stress-test-m1.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: rr.Stress.Pickups <nuts> <chips> (spawns BP_PickupXP and BP_PickupXPBig on walkable cells around the miner, outside the magnet and pickup radius so the counts hold, no collection) and rr.Stress.ClearPickups in URRStressSubsystem (Source/RogueRigs/Debug/), console variables ECVF_Cheat, absent from shipping like the rest; documented in sys-stress; the project builds with the editor closed
- done_when: The project compiles (RogueRigsEditor, editor closed); rr.Stress.Pickups 100 6 followed by rr.Stress.ClearPickups works in a -game run with the counts read back in the log; sys-stress updated; lint clean; committed
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-138-xp-pickups-perf-test]] — parent global task.
