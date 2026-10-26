---
id: task-317-05-capture-stats-cards
description: "Capture in -game the stats screen, the cards screen and the level 4 stats screen and send them to the owner."
parent: task-317-stats-then-cards
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-317-stats-then-cards]] — parent global task.
