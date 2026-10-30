---
id: task-388-01-clear-chest-effects
description: "Cpp: find what of the opened chest survives the level change and remove it at the ClearingLevel step"
parent: task-388-chest-glow-cleared-between-levels
kind: task
status: todo
---
# Bonus chest glow removed between levels: cpp

- goal: Reproduce in -game (level 1, alert sped up, chest dropped, rr.Cheat.CompleteHack on the chest, then rr.Run.EndLevel, rr.Run.Stats.Next, rr.Run.Continue) and log which chest effects or lights still exist at level 2 start; then make the ClearingLevel step of the preload remove every effect and light the bonus chest left in the world (Niagara components spawned by OnLidSlam and OnLidOpened, any light), keeping the opening look unchanged during its own level. Check the map-travel fallback too (a new world should already be clean: confirm by log).
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), bp-bonus-chest-open (vault/reference/tech/blueprints/fx/bp-bonus-chest-open.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), global request and gdd_check: vault/reference/tasks/task-388-chest-glow-cleared-between-levels.md
- expected_output: C++ change in Source/RogueRigs (run transition preload and or bonus chest), log line `chest effects cleared for the next level: N` (or equivalent), commit `task-388-01-clear-chest-effects: ...`; a change summary with the cause found, the files changed, the log lines seen, for the doc subtask (no doc node written).
- done_when: Build passes with the editor closed; a -game log shows the chest opened on level 1 and, at the level 2 preparation, the removal line with N > 0 and no chest effect or light alive at level 2 start; the opening effects still logged as spawned on level 1; the uncommitted RRPlayerController and Config changes left untouched; committed by path.
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
- [[task-388-chest-glow-cleared-between-levels]] — parent global task.
