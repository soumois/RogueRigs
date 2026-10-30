---
id: task-388-01-clear-chest-effects
description: "Cpp: find what of the opened chest survives the level change and remove it at the ClearingLevel step"
parent: task-388-chest-glow-cleared-between-levels
kind: task
status: done
---
# Bonus chest glow removed between levels: cpp

- goal: Reproduce in -game (level 1, rr.Cheat.AlertSpeed until the chest drops, rr.Cheat.CompleteHack on the chest, then rr.Run.EndLevel, rr.Run.Stats.Next, rr.Run.Continue) and log which chest effects or lights still exist at level 2 start; then make the ClearingLevel step of the preload remove every effect and light the bonus chest left in the world (Niagara components spawned by OnLidSlam and OnLidOpened, any light), keeping the opening look unchanged in its own level. Confirm by log that the map-travel fallback (new world) is clean.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), bp-bonus-chest-open (vault/reference/tech/blueprints/fx/bp-bonus-chest-open.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), global request and gdd_check: vault/reference/tasks/task-388-chest-glow-cleared-between-levels.md
- expected_output: C++ change in Source/RogueRigs (preload clear and or bonus chest), log line `chest effects cleared for the next level: N` (or equivalent), commit `task-388-01-clear-chest-effects: ...`; change summary (cause found, files, log lines) for the doc subtask (no doc node written).
- done_when: Build passes with the editor closed; a -game log shows the chest opened on level 1 and its opening effects spawned, then at the level 2 preparation the removal line with N > 0 and no chest effect or light alive at level 2 start; uncommitted Config changes untouched; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 66, cache write 133106, cache read 2794349, output 30741
- usage: turns 33, context start 12k peak 133k, resumes 0, 14 min, units 0.60 M
- runs: 1
- milestone: M2
- started: 2026-10-30T20:43:17Z
- waiting_for: none
- result: Cause: opening effects (NS_BonusChestOpen) spawned world-owned, never finish, survive chest destroy. RRBonusChest tracks and ClearOpeningEffects at ResetForNextLevel (log: chest effects cleared for the next level: 1); level start logs live count on both paths. -game run: removed 1, 0 alive at level 2 start. Fallback path not forced after opening; OnLidSlam spawned 0 effects (not investigated). Commit 7ecb8a00.

## Relations
- [[task-388-chest-glow-cleared-between-levels]] — parent global task.
