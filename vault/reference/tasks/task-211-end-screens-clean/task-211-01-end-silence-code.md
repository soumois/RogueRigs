---
id: task-211-01-end-silence-code
description: "cpp: When the run ends"
parent: task-211-end-screens-clean
kind: task
status: in-progress
---
# end-silence-code

- goal: When the run ends (run won, shuttle missed, miner death game over), the subtitle system clears the current line and drops new lines until the restart, and an end-of-run event or state is exposed for the HUD widgets to hide everything but the end screen.
- agent: cpp
- depends_on: none
- inputs: extraction (vault/reference/gdd/run/extraction.md), sys-subtitles (vault/reference/tech/systems/ui/sys-subtitles.md), wbp-subtitles (vault/reference/tech/blueprints/ui/wbp-subtitles.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), sys-player (vault/reference/tech/systems/core/sys-player.md), hud-refactor-audit (vault/reference/tech/blueprints/ui/hud-refactor-audit.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/, docs sys-subtitles, sys-hud and sys-objectives-run-flow updated with the event the bp widgets bind to.
- done_when: build succeeds; existing tests pass except the known failures; a -game log shows subtitles silenced at run won, shuttle missed and death; docs updated; committed.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-211-end-screens-clean]] — parent global task.
