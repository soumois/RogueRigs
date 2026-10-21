---
id: task-274-08-far-floor-apply
description: "MPC_PlayerLight defaults set to the values chosen in step 7."
parent: task-274-flat-light-darkening-shader
kind: task
status: done
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
- model: sonnet
- tokens: input 52, cache write 63533, cache read 961184, output 882
- runs: 1
- milestone: M1
- result: MPC_PlayerLight defaults read before 1 / 0.002 / 260 / 4, set FarBrightness 0.03, Radius 250, Exponent 4 (Enable 1 kept), read back, saved (not dirty), commit eb1627e3; owner 2026-10-21 validated 0.03 / 250 / 4 and accepted brighter far rock tops (about 75/255); mpp-player-light-mask updated with previous values. bDriveTunables false on BP_Miner LightMask; only M_PP_PlayerLightMask references the MPC. No PIE or capture.

## Relations
- [[task-274-flat-light-darkening-shader]] — parent global task.
