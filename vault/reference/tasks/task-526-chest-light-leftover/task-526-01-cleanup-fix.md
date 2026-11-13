---
id: task-526-01-cleanup-fix
description: "Cpp: chest effects removed with the level they belong to"
parent: task-526-chest-light-leftover
kind: task
status: done
---
# Lumière du coffre restée du niveau précédent: cpp

- goal: chest effects removed with the level they belong to
- agent: cpp
- depends_on: none
- inputs: objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md)
- expected_output: see done_when
- done_when: A1 to A3 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-526
- usage: included in the executor run of task-526
- runs: 1
- milestone: M2
- started: 2026-11-13T14:10:41Z
- waiting_for: none
- result: Cause (owner PIE log 2026-11-13 10:32): the first NS_BonusChestOpen of the session was still compiling at the spawn, Niagara deferred its activation, so the before/after active snapshot of OpenLid tracked 0 effects; ClearOpeningEffects then cleared 0 and the never-ending system stayed into level 2 (level 2 chest, compiled by then, was tracked and cleared). Fix d62834de: RRBonusChest snapshots every world FX component with its active state and tracks components new since the snapshot even when not active yet, plus pooled ones turned active; IsOpeningEffectAlive counts a Niagara activation still waiting; ClearOpeningEffects calls Deactivate before DeactivateImmediate (cancels a waiting activation). Build green; RogueRigs.Console 6/6 pass. -game run: OnLidOpened tracked 1, level change removed 3 (NS_BonusChestOpen, NS_HeavySlamImpact, InnerLight), level 2 start alive 0. The deferred-compile case itself did not reproduce in -game (system already compiled), so it is covered by the code path and the owner log, not by a reproduced run.

## Relations
- [[task-526-chest-light-leftover]] — parent global task.
