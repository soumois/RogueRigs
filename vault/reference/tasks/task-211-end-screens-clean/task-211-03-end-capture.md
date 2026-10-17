---
id: task-211-03-end-capture
description: "cpp: In-game capture of the run-won, shuttle-missed and death end screens with no HUD and no subtitle, compared with the task-209-05 captures, se"
parent: task-211-end-screens-clean
kind: task
status: todo
---
# end-capture

- goal: In-game capture of the run-won, shuttle-missed and death end screens with no HUD and no subtitle, compared with the task-209-05 captures, sent to the owner.
- agent: cpp
- depends_on: task-211-02-end-hud-bp
- inputs: sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: PNG captures in Saved/Captures/ listed in the report.
- done_when: captures show each end screen alone; the orchestrator sends them to the owner.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-211-end-screens-clean]] — parent global task.
