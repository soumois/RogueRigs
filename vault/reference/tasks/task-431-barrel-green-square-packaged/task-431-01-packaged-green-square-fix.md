---
id: task-431-01-packaged-green-square-fix
description: "Cpp: reproduce the green square in the packaged build, find the cause, fix it, verify in packaged frames"
parent: task-431-barrel-green-square-packaged
kind: task
status: todo
---
# Barrel explosion green square in the packaged build: cpp

- goal: Reproduce the green decal square in the staged packaged build, name its cause with evidence, fix it, and show packaged frames without it on every barrel explosion checked.
- agent: cpp
- depends_on: none
- inputs: task-201-01-decal-first-frame-green (vault/reference/tasks/task-201-decal-first-frame-green/task-201-01-decal-first-frame-green.md), task-200-01-decal-green-square-check (vault/reference/tasks/task-200-decal-green-square-check/task-200-01-decal-green-square-check.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), agent-cpp (vault/reference/agents/agent-cpp.md), `Source/RogueRigs/Telegraph/RRDecalWarmUp.h`
- expected_output: Packaged-build frames and the log lines that show the cause (not committed, `Saved/Scratch/` or `Saved/Screenshots/`); C++ and, if needed, config fix; a change summary for the doc subtask (cause, fix, cvars or config changed, command lines used); one or two commits by path.
- done_when: (1) Before any change: a staged packaged Development build (BuildCookRun of build-toolchain, then `Saved/StagedBuilds/Windows/RogueRigs.exe`) with at least 5 successive barrel explosions in level 1 (a chain included) has frames at fuse start and explosion; the green square is seen on one of them with its log lines, or, after 2 runs without it, the agent stops and reports (no blind fix). (2) The cause is named with evidence (why the warm-up of task-201 does not cover it: cooked shader or PSO path, later instance, level travel, or other). (3) After the fix the package rebuilds with exit 0 and the same scenario shows no green square at any fuse start or explosion; level 2 checked the same way with 2 explosions if a debug command reaches it, else listed unverified. (4) The task-201 `-game` command still shows no green square. (5) Build exit 0; existing explosion, barrel and telegraph tests pass (only broken expectations updated, no new test). (6) Game rules unchanged; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-431-barrel-green-square-packaged]] — parent global task.
