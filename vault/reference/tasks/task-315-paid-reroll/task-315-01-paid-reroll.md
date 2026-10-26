---
id: task-315-01-paid-reroll
description: "The level-up reroll costs Duskite ore: 15, plus 5 for each further reroll without a level-up, usable while the ore lasts."
parent: task-315-paid-reroll
kind: task
status: in-progress
---
# Paid level-up reroll

- goal: The level-up reroll costs Duskite ore: 15, plus 5 for each further reroll without a level-up, usable while the ore lasts. Details (owner defaults accepted 2026-10-26, see the global gdd_check): base cost 15 and step 5 are tunables (provisional, owner 2026-10-26), in one place the task-316 shop reroll can reuse (same rule, its own count); the once-per-offer limit goes; the count resets when an offer opens for a level-up (chained level-ups included), a bonus chest offer keeps it; a reroll spends carried ore first, then secured ore (URROreBankSubsystem), and the HUD ore counter follows (OnOreChanged); the offer event's bCanReroll means affordable now, also after a reroll; Blueprint-readable GetRerollCost and CanReroll; the rr.Cheat Reroll command follows the same rule. No new test (owner rule): only fix existing tests the change breaks.
- agent: cpp
- depends_on: none
- inputs: xp-level-up (vault/reference/gdd/run/xp-level-up.md), run-shop (vault/reference/gdd/run/run-shop.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/ (Progression, Upgrades, ore counter), updated system docs sys-level-up, sys-upgrades, sys-objectives-ore-counter
- done_when: build passes; reroll costs 15, 20, 25 in a row and resets to 15 when a new level-up screen opens (a chest screen keeps the count); several rerolls per screen while ore suffices; refused without enough ore; ore spent from carried ore first then from secured ore; a Blueprint-readable cost and affordability; existing tests broken by the change updated and passing; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-315-paid-reroll]] — parent global task.
