---
id: task-391-01-level-ore-in-recap
description: "Cpp: find what the run-won end screen ore value counts, fix the recap totals, pure getter of the run total"
parent: task-391-final-screens-count-level-ore
kind: task
status: todo
---
# Run-won end screen shows the run ore total: cpp

- goal: Reproduce in -game: rr.Cheat.GrantOre with distinct amounts per level (3 on level 1, 5 on level 2, one card bought with rr.Shop.Buy, 7 on level 3, 11 on level 4), extract each level, log at each stats screen and at the run-won end screen: recap RunSecuredOre, OreSecuredThisLevel, GetLevelSecuredOre, the bank, RunFlow GetSecuredOre, and the console objective rock count; say what the owner's 33 was. Fix so Total Ore = ore secured since the run start including the extraction just made, purchases not subtracted, on levels 1 to 4 (preload path and fallback). Add a BlueprintPure getter of that run total usable by WBP_AlertObjectives (or reuse URRRunSubsystem::GetRunSecuredOre if it is right).
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), intermission (vault/reference/gdd/run/intermission.md), ore-economy (vault/reference/gdd/run/ore-economy.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), global request and gdd_check: vault/reference/tasks/task-391-final-screens-count-level-ore.md
- expected_output: C++ fix, log lines with the values per level, commit `task-391-01-level-ore-in-recap: ...`; change summary (cause, what 33 was, files, getter name) for the bp and doc subtasks.
- done_when: Build passes with the editor closed; the -game log of the scenario shows Total Ore 3, 8, 15, 26 on the four stats screens (purchase not subtracted) and the getter returns 26 at the run-won screen; uncommitted Config changes untouched; committed by path.
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
- [[task-391-final-screens-count-level-ore]] — parent global task.
