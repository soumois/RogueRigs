---
id: task-274-05-darkener-defaults
description: "MPC_PlayerLight defaults (and exposure or emissive parameters if the sweep requires them) set to the values chosen in step 4."
parent: task-274-flat-light-darkening-shader
kind: task
status: in-progress
---
# Darkener defaults

- goal: MPC_PlayerLight defaults (and exposure or emissive parameters if the sweep requires them) set to the values chosen in step 4.
- agent: bp
- depends_on: task-274-04-darkener-sweep
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Defaults set and read back; docs updated.
- done_when: Values read back; saved by path; docs updated with previous values; lint clean; committed by path.
- editor_state: open
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
