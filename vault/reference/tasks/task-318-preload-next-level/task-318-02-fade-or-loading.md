---
id: task-318-02-fade-or-loading
description: "Continue fades into the ready level, or shows the loading screen until it is ready, then fades in."
parent: task-318-preload-next-level
kind: task
status: todo
---
# Continue fades into the ready level, or shows the loading screen until it is ready, then fades in

- goal: Continue fades into the ready level, or shows the loading screen until it is ready, then fades in. No new test (owner rule): only fix existing tests the change breaks.
- agent: cpp
- depends_on: task-318-01-preload-next-level
- inputs: intermission (vault/reference/gdd/run/intermission.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/ (Run, Player controller): fade path and loading path, fade durations in URRRunSettings; doc sys-run-transition updated
- done_when: build passes; Continue with the level ready: fade out 0.3 s, switch, fade in 0.5 s (provisional, data), no loading screen; not ready: loading screen shown (OnLoadingStarted) with the progress value available to the widget, then the fade in when ready; HUD and subtitles hidden and restored as before; gameplay held until ready; log lines for both paths; lint clean.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-318-preload-next-level]] — parent global task.
