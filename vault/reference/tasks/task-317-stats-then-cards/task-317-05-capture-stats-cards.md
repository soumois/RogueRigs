---
id: task-317-05-capture-stats-cards
description: "Capture in -game the stats screen, the cards screen and the level 4 stats screen and send them to the owner."
parent: task-317-stats-then-cards
kind: task
status: done
---
# Capture in -game the stats screen, the cards screen and the level 4 stats screen and send them to the owner

- goal: Capture in -game the stats screen, the cards screen and the level 4 stats screen and send them to the owner. No new test (owner rule): only fix existing tests the change breaks.
- agent: cpp
- depends_on: task-317-04-bp-stats-screen
- inputs: sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG captures in Saved/StatsCards/ with a contact sheet; log excerpt; doc verified lines updated in wbp-intermission, wbp-shop-screen
- done_when: rendered -game captures at 1920 x 1080: level 1 stats screen with kills per robot type and damage per weapon non-empty, cards screen on black with 5 cards, level 4 stats screen then the run-won screen with no cards screen; compared with the task-316 capture (Saved/IntermissionShop); unseen items listed; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 60, cache write 82727, cache read 1498563, output 12963
- runs: 1
- milestone: M2
- result: -game captures in Saved/StatsCards (contact-sheet.png): L1 stats (Chaser: 8, Burst rifle: 125, Next level), cards on black with 5 priced cards, L4 stats CLAIM EMPTIED / Cash out, then run-won screen, no cards on L4; no defect; unseen: mouse clicks, multi-entry lists, buy/reroll on new screen, nonzero ore. Commit f14a7110.

## Relations
- [[task-317-stats-then-cards]] — parent global task.
