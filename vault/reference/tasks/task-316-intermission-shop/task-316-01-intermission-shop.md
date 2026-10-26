---
id: task-316-01-intermission-shop
description: "The intermission screen sells 5 upgrade cards priced by rarity, with an escalating reroll and the Continue button."
parent: task-316-intermission-shop
kind: task
status: done
---
# Upgrade shop between levels

- goal: The intermission screen sells 5 upgrade cards priced by rarity, with an escalating reroll and the Continue button. Details (owner defaults accepted 2026-10-26, see the global gdd_check): the offer is built when the intermission opens, 5 stat cards drawn like a level-up offer with owner luck, no new-weapon card; prices per tier in data (grey 25, green 40, blue 60, purple 80, yellow 100, provisional, owner 2026-10-26); buying spends secured ore (carried is 0 at the intermission), applies the card to the miner and records it in the run snapshot so it is replayed in the next level, and the slot stays empty; the shop reroll reuses the task-315 cost rule with its own count reset at each intermission, redraws all 5 slots; the recap ore values stay as recorded; Blueprint API for the widget (offer, price per slot, balance, buy, reroll cost and affordability, a change event). No new test (owner rule): only fix existing tests the change breaks.
- agent: cpp
- depends_on: task-315-01-paid-reroll
- inputs: run-shop (vault/reference/gdd/run/run-shop.md), intermission (vault/reference/gdd/run/intermission.md), rarities (vault/reference/gdd/weapons/rarities.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ shop logic in Source/RogueRigs/ (Run, Upgrades), prices in config data, updated docs sys-run-transition, sys-upgrades
- done_when: build passes; on the intermission a 5-card stat-only offer drawn with owner luck; price by tier grey 25, green 40, blue 60, purple 80, yellow 100 read from data; buying spends secured ore, applies the card, adds it to the run snapshot so it carries into the next level, leaves its slot empty; several buys allowed; refused without enough ore; shop reroll redraws the 5 slots at 15 then +5 per reroll, reset at each intermission; Blueprint API (offer, prices, balance, buy, reroll cost, change event) documented; existing tests broken by the change updated and passing; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: opus
- tokens: input 74, cache write 147886, cache read 3962376, output 40140
- runs: 1
- milestone: M2
- result: Shop built in URRIntermissionShopSubsystem (offer of 5 stat cards with owner luck, prices in URRShopSettings grey 25 green 40 blue 60 purple 80 yellow 100, BuySlot spends ore and appends to the run snapshot, RequestReroll on the task-315 cost rule with own count, OnShopChanged event, OnShopRefreshed widget hook); build passes; tests unchanged, only known failures LevelUpUpgradeLoop RerollButton and JuiceKillDropsStreamingCubes pickup collision (unrelated); docs sys-run-shop new, sys-run-transition, sys-upgrades updated; commits eedadd04 3b5f4992

## Relations
- [[task-316-intermission-shop]] — parent global task.
