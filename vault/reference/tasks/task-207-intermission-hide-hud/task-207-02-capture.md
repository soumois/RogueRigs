---
id: task-207-02-capture
description: "cpp: In-game capture"
parent: task-207-intermission-hide-hud
kind: task
status: todo
---
# capture

- goal: In-game capture (rendered -game, 1920 x 1080, L_ArenaM1) of the intermission screen, compared with the task-206-06 capture, sent to the owner.
- agent: cpp
- depends_on: task-207-01-hide-hud-code
- inputs: sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), intermission (vault/reference/gdd/run/intermission.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG captures in Saved/Captures/ listed in the report.
- done_when: the capture shows the intermission screen with the fix; the orchestrator sends it to the owner.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-207-intermission-hide-hud]] — parent global task.
