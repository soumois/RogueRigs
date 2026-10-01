---
id: task-006-05-dig-tests
description: "C++ automation tests for grid damage, drops, pickup collection and the drone dig, run headless from the command line, since MCP cannot drive PIE input."
parent: task-006-dig
kind: task
status: done
---
# Dig automation tests

- goal: prove the dig rules in code, because MCP cannot hold a key or call a UFunction in PIE (task-006-04).
- agent: cpp
- depends_on: task-006-04-bp-dig
- inputs: [[sys-grid]], [[sys-drone]], [[sys-pickups]], [[sys-player]], [[drone]], [[ore-blocks]], [[heal-ore]], [[build-toolchain]], [[cpp-standard]].
- expected_output: `Source/RogueRigs/Tests/` (compiled only `WITH_DEV_AUTOMATION_TESTS`), test names under `RogueRigs.Dig.*`, each in a fresh game world (e.g. `FAutomationTestWorld` or a transient `UWorld` of type Game, ticked manually at 1/60 s): (1) rock (43, 38) of `arena_m1` is destroyed after 1.5 s ± 1 frame of `ApplyTerrainDamage(dt)`, the floor HISM gains one instance and the rock HISM loses one, index maps stay consistent (every remaining instance maps back to its cell); (2) heal ore (49, 45): 2 pickups by 3.0 s, destroyed at 4.5 s, 5 pickups in total (load `/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal` as the class); (3) `DestroyCellsInRadius` removes every block in the radius; (4) drone dig: a miner and drone spawned next to rock (43, 38), `RequestDig` → state Dig, block destroyed after about 1.5 s of ticks (+ fly-in tolerance), state back to Orbit, shake stopped; moving the miner 2 cells away cancels; (5) pickup within `PickupRadius` of the miner is collected (`OnPickupCollected` fires). Use `BP_ArenaGrid`, `BP_Miner`, `BP_Drone` classes where Blueprint defaults are needed. Run them headless with the editor closed: `UnrealEditor-Cmd.exe <uproject> -ExecCmds="Automation RunTests RogueRigs.Dig;Quit" -unattended -nullrhi -nosplash -log`, and record the command in [[build-toolchain]]. Fix any C++ bug the tests reveal (report each fix).
- done_when: build exits 0; all `RogueRigs.Dig.*` tests pass in the headless run (log excerpt with results); docs updated for any fix; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: `Source/RogueRigs/Tests/RRDigTests.cpp` + `RRTestEventRecorder.h`; 6 tests pass headless (orchestrator re-ran them): rock destroyed at 1.500 s, heal ore drops at 1.5 and 3.0 s then 3 at 4.5 s (5 total), radius destruction, drone dig 1.5 s and cancel, pickup collected. No gameplay bug found. New doc sys-automation-tests; headless command in [[build-toolchain]]. About 139k tokens (over ceiling: harness frame-counter issue).

## Relations
- [[task-006-dig]] — parent global task.
