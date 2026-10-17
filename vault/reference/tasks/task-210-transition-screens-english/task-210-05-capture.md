---
id: task-210-05-capture
description: "cpp: In-game capture"
parent: task-210-transition-screens-english
kind: task
status: done
---
# capture

- goal: In-game capture (rendered -game, 1920 x 1080, L_ArenaM1) of the intermission and loading screens in English, compared with the task-206-06 capture, sent to the owner.
- agent: cpp
- depends_on: task-210-03-intermission-bp, task-210-04-loading-bp
- inputs: sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG captures in Saved/Captures/ listed in the report.
- done_when: the capture shows the intermission and loading screens in English with the fix; the orchestrator sends it to the owner.
- editor_state: closed
- token_ceiling: 40000
- model: none
- tokens: none
- runs: 0
- milestone: M2
- result: No separate run: the task-208-02 rendered captures (after 210-03 and 210-04) show the intermission (LEVEL 1 CLEARED, Next claim: level 2 of 4, Ore secured this level: 30, Ore secured this run: 30, Shop opens later, Continue) and the loading screen (DROPPING IN, Level 2, Loading the map / Prepping the level) in English; sent to the owner. Saved/Captures/task208_intermission_00.png, task208_oldworld_01.png, task208_newworld_01.png.

## Relations
- [[task-210-transition-screens-english]] — parent global task.
