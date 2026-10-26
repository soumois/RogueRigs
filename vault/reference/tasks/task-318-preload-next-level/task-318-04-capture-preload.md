---
id: task-318-04-capture-preload
description: "Capture in -game the fade path and the loading-screen path and send them to the owner."
parent: task-318-preload-next-level
kind: task
status: done
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
- model: opus
- tokens: input 60, cache write 82392, cache read 1626593, output 17114
- usage: turns 30, context start 11k peak 88k, resumes 1 (message x1), 4 min, units 0.35 M
- runs: 1
- milestone: M2
- result: Rendered -game captures in Saved/PreloadNextLevel (contact_sheet.png): cards screen on black while level 2 prepared (601.8 ms, ready before Continue); fade path darkens cards then level 2 with no loading screen; loading path shows WBP_Loading with orange bar at 0.25 and 0.75, then fade in on level 3; carried state kept (trivial level-1 state). Unseen: fade-in on fade path (hitch), bar at 0.5/1.0, LoadingImage, fallback, restart/death. Fade-ins logged 684-705 ms because of post-switch hitches. Commit cd3bce1f.

## Relations
- [[task-318-preload-next-level]] — parent global task.
