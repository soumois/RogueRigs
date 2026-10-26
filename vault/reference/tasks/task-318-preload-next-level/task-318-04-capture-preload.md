---
id: task-318-04-capture-preload
description: "Capture in -game the fade path and the loading-screen path and send them to the owner."
parent: task-318-preload-next-level
kind: task
status: in-progress
---
# Capture in -game the fade path and the loading-screen path and send them to the owner

- goal: Capture in -game the fade path and the loading-screen path and send them to the owner. No new test (owner rule): only fix existing tests the change breaks.
- agent: cpp
- depends_on: task-318-03-bp-loading-progress
- inputs: sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG captures in Saved/PreloadNextLevel/ with a contact sheet; log excerpt with preparation duration and readiness; doc verified lines updated in sys-run-transition, wbp-loading
- done_when: rendered -game: cards screen on black while the log shows the next level preparing; Continue after readiness gives the fade with no loading screen; Continue at once gives the loading screen with a moving progress bar, then the fade in; level 2 plays with the carried state; unseen items listed; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-318-preload-next-level]] — parent global task.
