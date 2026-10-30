---
id: task-397-01-extractor-out-of-sight
description: "Cpp: find which extractor shows at its start spot during the intermission and keep it out of sight until the next level"
parent: task-397-shuttle-stays-gone
kind: task
status: done
---
# Extractor stays gone after its takeoff: cpp

- goal: Reproduce in -game: extraction (real boarding or rr.Run.EndLevel), takeoff, stats screen, rr.Run.Stats.Next, wait several seconds on the cards screen, rr.Run.Continue; log every ARRShuttle actor (exit and arrival) with location, rotation, hidden state, takeoff phase and transition phase each second and at each phase change; find which one is back at a start spot and why (takeoff end or removal resetting the transform, preload ClearingLevel or PlacingMiner, arrival placed during the preparation, other). Fix it in C++ so that no extractor is visible from the end of the takeoff until the next level starts, without changing the takeoff values or the arrival behaviour at the level start.
- agent: cpp
- depends_on: task-391-04-recap-ore-balance
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), extraction (vault/reference/gdd/run/extraction.md), level-flow (vault/reference/gdd/run/level-flow.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), global request and gdd_check: vault/reference/tasks/task-397-shuttle-stays-gone.md
- expected_output: C++ fix in Source/RogueRigs, log lines of the extractor states, commit `task-397-01-extractor-out-of-sight: ...`; change summary (what the owner saw, cause, files, log lines) for the doc subtask.
- done_when: Build passes with the editor closed; the -game log of the sequence shows, after the fix, no visible extractor between the takeoff removal and the next level start (stats and cards screens), and the arrival extractor placed and taking off at the next level start; uncommitted Config changes untouched; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 82, cache write 111320, cache read 3117239, output 25116
- usage: turns 41, context start 12k peak 117k, resumes 0, 13 min, units 0.58 M
- runs: 1
- milestone: M2
- started: 2026-10-30T22:24:41Z
- waiting_for: none
- result: Owner saw the next level arrival extractor (placed by PlacingMiner during the cards screen at the level 1 start cell), not the exit shuttle (removed after 3.6 s). Fix: SetArrivalShuttleHidden, hidden after placement, shown under black at StartPreparedLevel; log verified before/after; build ok; commit 6de46eec

## Relations
- [[task-397-shuttle-stays-gone]] — parent global task.
