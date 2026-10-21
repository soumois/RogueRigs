---
id: task-274-08-far-floor-apply
description: "MPC_PlayerLight defaults set to the values chosen in step 7."
parent: task-274-flat-light-darkening-shader
kind: task
status: todo
---
# Far floor values applied

- goal: MPC_PlayerLight defaults set to the values chosen in step 7.
- agent: bp
- depends_on: task-274-07-far-floor-sweep
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Defaults set, read back, saved; doc updated.
- done_when: Values read back; saved by path; doc updated with previous values; lint clean; committed by path.
- editor_state: open
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
