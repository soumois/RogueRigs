---
id: task-208-01-delay-travel-code
description: "cpp: Continue waits until the old-world loading widget has been rendered at least once"
parent: task-208-loading-drawn-before-travel
kind: task
status: done
---
# delay-travel-code

- goal: Continue waits until the old-world loading widget has been rendered at least once (Slate post-tick or a 2-frame delay) before OpenLevel.
- agent: cpp
- depends_on: none
- inputs: intermission (vault/reference/gdd/run/intermission.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run (vault/reference/tech/systems/world/sys-run.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/Run/ and the controller, sys-run-transition updated.
- done_when: build succeeds; tests pass except the 2 known failures; a rendered -game log shows the frame numbers of loading started, first loading paint and travel; doc updated; committed.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 44, cache write 48433, cache read 799626, output 405
- runs: 1
- milestone: M2
- result: URRLoadingScreenWidget NativePaint notifies the transition; OpenLevel waits for a paint in an earlier frame, core ticker (runs while paused), fallback after LoadingPaintWaitMaxFrames 5 (provisional) (e6f3135b, doc c7e14cef). Rendered -game: Continue frame 2210, first paint 2211, travel 2212, 2 frames drawn before the load, about 14 ms cost. Tests 260 pass, 2 known failures.

## Relations
- [[task-208-loading-drawn-before-travel]] — parent global task.
