---
id: task-207-01-hide-hud-code
description: "cpp: While the run transition phase is Intermission or Loading, the player controller hides the HUD widget and the subtitle system clears the cur"
parent: task-207-intermission-hide-hud
kind: task
status: done
---
# hide-hud-code

- goal: While the run transition phase is Intermission or Loading, the player controller hides the HUD widget and the subtitle system clears the current subtitle and stays silent; both are restored on OnLevelReady.
- agent: cpp
- depends_on: none
- inputs: intermission (vault/reference/gdd/run/intermission.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), sys-subtitles (vault/reference/tech/systems/ui/sys-subtitles.md), wbp-subtitles (vault/reference/tech/blueprints/ui/wbp-subtitles.md), hud-refactor-audit (vault/reference/tech/blueprints/ui/hud-refactor-audit.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/ (controller, subtitles), docs sys-run-transition, sys-hud and sys-subtitles updated.
- done_when: build succeeds; existing tests pass except the 2 known failures; a -game log shows HUD hidden and subtitle cleared at intermission, restored at level ready; docs updated; committed.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 100, cache write 98317, cache read 3175596, output 4768
- runs: 1
- milestone: M2
- result: HUD collapsed and subtitles silenced (current line cleared, new lines dropped) on OnIntermissionStarted and OnLoadingStarted, restored on OnLevelReady; a world opened by a transition starts hidden and silenced (1c9fd70a, docs e5e52b73). Rendered -game log confirms. Tests 259 pass; failures: the 2 known plus RogueRigs.Ore.HealVein, flaky (random pop, passed 2 of 6 alone), not run by the new code.

## Relations
- [[task-207-intermission-hide-hud]] — parent global task.
