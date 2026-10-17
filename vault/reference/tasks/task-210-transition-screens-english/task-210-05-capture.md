---
id: task-210-05-capture
description: "cpp: In-game capture"
parent: task-210-transition-screens-english
kind: task
status: todo
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-210-transition-screens-english]] — parent global task.
