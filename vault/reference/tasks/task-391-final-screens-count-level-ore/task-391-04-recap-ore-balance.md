---
id: task-391-04-recap-ore-balance
description: "Cpp: the stats screen Total Ore reads the ore balance the player holds; cd19ba03 secured-since-reset code reverted unless still used"
parent: task-391-final-screens-count-level-ore
kind: task
status: done
---
# Every on-screen ore number is the balance: cpp

- goal: Owner rule 2026-10-30: every player-facing ore number is the ore the player holds. Make the recap total that WBP_Intermission shows as "Total Ore" (field `FRRIntermissionRecap::RunSecuredOre`, read by the widget by name: keep the field name, change its meaning and its comment) equal the bank secured balance at the stats screen, i.e. after the extraction just made, with every purchase and reroll subtracted (`URROreBankSubsystem::GetSecuredOre`), on levels 1 to 4, preload path and map-travel fallback. The run-won end screen already reads `GetSecuredOre(RunFlow)`: leave it. Then remove what commit cd19ba03 added for the old "collected since run start" figure (`SecuredSinceReset` in the bank, `URRRunSubsystem::GetRunSecuredOre`, the level and run baselines) when nothing else uses it after this change: no dead code. `RecordIntermissionOreSpent` (removed by cd19ba03) is not restored unless `OreSecuredThisLevel` needs it. `OreSecuredThisLevel` stays as data and in the logs (the ore secured by this level's extraction), never shown as the total. Keep the ore figures log lines of cd19ba03, adjusted to the new meaning.
- agent: cpp
- depends_on: task-391-01-level-ore-in-recap
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), ore-economy (vault/reference/gdd/run/ore-economy.md), intermission (vault/reference/gdd/run/intermission.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), task-391-01-level-ore-in-recap (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-01-level-ore-in-recap.md) result and commit cd19ba03 (`git show cd19ba03`), owner run log `Saved/Packaged/Windows/RogueRigs/Saved/Logs/RogueRigs.log` (run 20261030-131717: level 4 "30 ore secured, session total 33"; stats screens showed run 26, 77, 138, 168 while the shop opened with balance 26, 52, 63), global request and gdd_check: vault/reference/tasks/task-391-final-screens-count-level-ore.md
- expected_output: C++ change in Source/RogueRigs (run subsystem, transition recap, ore bank), commit `task-391-04-recap-ore-balance: ...`; change summary for the doc subtask: what the recap total now reads, what of cd19ba03 was removed or kept and why, log lines seen (no doc node written).
- done_when: Build passes with the editor closed; a -game run with rr.Cheat.GrantOre 3 on level 1, rr.Shop.Buy of one card (price P) on the level 1 shop, 5 on level 2, 7 on level 3, 11 on level 4 logs at each stats screen a recap total equal to the bank balance: 3, 8 - P, 15 - P, 26 - P, equal to the shop balance that follows (levels 1 to 3) and to the run-won end screen value (level 4); a level-up reroll paid in ore lowers the next figure by its price; no remaining reference to removed symbols (grep); uncommitted Config changes untouched; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 72, cache write 114118, cache read 2557629, output 27601
- usage: turns 36, context start 12k peak 114k, resumes 0, 9 min, units 0.54 M
- runs: 1
- milestone: M2
- started: 2026-10-30T22:14:00Z
- waiting_for: none
- result: Recap RunSecuredOre = bank balance (L1-3 snapshot, L4 run flow); cd19ba03 baselines/SecuredSinceReset/GetRunSecuredOre/GetLevelSecuredOre removed; GetExtractionSecuredOre added; -game run figures equal shop balance and run-won line (level 1 granted 103 not 3: cheapest card costs 25); map-travel path not run; commit 3b5d5b88

## Relations
- [[task-391-final-screens-count-level-ore]] — parent global task.
