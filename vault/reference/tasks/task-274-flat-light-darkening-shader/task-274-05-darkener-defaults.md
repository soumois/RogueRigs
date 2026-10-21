---
id: task-274-05-darkener-defaults
description: "MPC_PlayerLight defaults (and exposure or emissive parameters if the sweep requires them) set to the values chosen in step 4."
parent: task-274-flat-light-darkening-shader
kind: task
status: done
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
- model: sonnet
- tokens: input 44, cache write 56037, cache read 771261, output 1117
- runs: 1
- milestone: M1
- result: Commit 1cd5a185: MPC_PlayerLight FarBrightness 0.005 -> 0.002, Radius 200 -> 260, Exponent 3 -> 4, Enable 1; PPV_XPGlow exposure min = max -2.9 -> -3.4 EV100; all read back; bloom, bias, blendable unchanged; MPC and PPV external package saved, not dirty; docs updated.

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
