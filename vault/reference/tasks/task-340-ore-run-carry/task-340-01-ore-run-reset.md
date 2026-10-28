---
id: task-340-01-ore-run-reset
description: "Cpp: reproduce the reset seen by the owner, keep ore across levels, clear it at each new run."
parent: task-340-ore-run-carry
kind: task
status: done
---
# Ore kept for the whole run, reset at run end: reproduce the reset seen by the owner, keep ore across levels, clear it at each new run

- goal: Reproduce in a -game run what the owner saw (log carried, secured and spendable ore at the level-1 extraction, at the level-2 start and at the next shop) and fix any loss of ore between levels; clear the secured ore bank when a new run starts (ResetRun, run lost, run won, main menu New Run; BeginRun of a fresh run) so ore lasts one run only (Q2); keep the recap figures (OreSecuredThisLevel, RunSecuredOre) right; update only tests the change breaks.
- agent: cpp
- depends_on: none
- inputs: ore-economy (vault/reference/gdd/run/ore-economy.md), run-structure (vault/reference/gdd/run/run-structure.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), sys-main-menu (vault/reference/tech/systems/ui/sys-main-menu.md), agent-cpp (vault/reference/agents/agent-cpp.md), global task-340 `gdd_check` (decided answers)
- expected_output: C++ change in `Source/RogueRigs/Objectives/` and `Source/RogueRigs/Run/`, build green, a change summary (cause found, functions changed, log lines) for the doc subtask; one commit.
- done_when: build succeeds; -game log shows ore secured on level 1 still spendable on level 2 and at the shop; after a restart or New Run the secured ore is 0; existing ore tests (Integration.LevelUpUpgradeLoop and any other touched) pass; committed.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: sonnet
- tokens: input 54, cache write 227818, cache read 2519977, output 10469
- usage: turns 27, context start 60k peak 119k, resumes 0, 17 min, units 0.59 M
- runs: 1
- milestone: M2
- started: 2026-10-28T06:01:10Z
- waiting_for: none
- result: Done, commit ed63c5c3. Cause: bank never reset between levels (HUD counter showed carried only, task 02); bank leaked into next run. Added URROreBankSubsystem::ResetSecuredOre called by URRRunSubsystem::ResetRun. -game log: 7 ore secured L1, spendable L2 and shop, cleared on restart. Build green. LevelUpUpgradeLoop (RerollButton enabled), JuiceKillDropsStreamingCubes fail identically on baseline (stash check), Text.AlertEventsWired fails too, unrelated; done_when test clause not met as written, pre-existing failure.

## Relations
- [[task-340-ore-run-carry]] — parent global task.
