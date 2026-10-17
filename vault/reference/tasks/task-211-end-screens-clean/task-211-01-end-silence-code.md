---
id: task-211-01-end-silence-code
description: "cpp: When the run ends"
parent: task-211-end-screens-clean
kind: task
status: done
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
- model: opus
- tokens: input 100, cache write 101232, cache read 3111678, output 8623
- runs: 1
- milestone: M2
- result: Subtitles silenced for the rest of the world on OnRunEnded (run won, shuttle missed) and OnGameOver (death), allowed again in the new world after restart; URRHUDWidget OnEndScreenShownDelegate(bRunWon, bGameOver) + BlueprintImplementableEvent OnEndScreenShown + IsEndScreenShown getters (d5862e0e, docs e553915c). Rendered -game logs for the three cases. Tests: known failures plus JuiceKillDropsStreamingCubes once (passed on rerun, flaky).

## Relations
- [[task-211-end-screens-clean]] — parent global task.
