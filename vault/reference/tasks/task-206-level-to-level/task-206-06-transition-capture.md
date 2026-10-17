---
id: task-206-06-transition-capture
description: "cpp: In-game captures of the intermission screen, the loading screen and the start of level 2, sent to the owner.."
parent: task-206-level-to-level
kind: task
status: in-progress
---
# transition-capture

- goal: In-game captures of the intermission screen, the loading screen and the start of level 2, sent to the owner.
- agent: cpp
- depends_on: task-206-04-wbp-intermission, task-206-05-wbp-loading
- inputs: sys-run (vault/reference/tech/systems/world/sys-run.md, written by task-206-01), intermission (vault/reference/gdd/run/intermission.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG captures in Saved/ listed in the report.
- done_when: three captures exist and show the intermission recap, the loading screen and level 2 on arena_l2; the orchestrator sends them to the owner; committed if a doc changed.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-206-level-to-level]] — parent global task.
