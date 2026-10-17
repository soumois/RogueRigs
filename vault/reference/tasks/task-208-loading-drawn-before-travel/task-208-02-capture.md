---
id: task-208-02-capture
description: "cpp: In-game capture"
parent: task-208-loading-drawn-before-travel
kind: task
status: done
---
# capture

- goal: In-game capture (rendered -game, 1920 x 1080, L_ArenaM1) of the first frames after Continue (loading screen on the old map), compared with the task-206-06 capture, sent to the owner.
- agent: cpp
- depends_on: task-208-01-delay-travel-code
- inputs: sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), intermission (vault/reference/gdd/run/intermission.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG captures in Saved/Captures/ listed in the report.
- done_when: the capture shows the first frames after Continue (loading screen on the old map) with the fix; the orchestrator sends it to the owner.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 42, cache write 74615, cache read 838599, output 4455
- runs: 1
- milestone: M2
- result: Rendered -game 1920 x 1080 captures sent to the owner: frames 1946 and 1947 on the old map show the loading screen (DROPPING IN, Level 2, Loading the map) before the blocking load, then the new-world loading screen and level 2 with the HUD. Observations: LoadMap's own redraw after BeginTearingDown captured the old scene without UMG (probably not presented, unproven); dev-only Preparing Shaders messages over the first new-world frames. Saved/Captures/task208_*.png.

## Relations
- [[task-208-loading-drawn-before-travel]] — parent global task.
