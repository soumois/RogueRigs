---
id: task-272-17-mask-defaults
description: "MPC_PlayerLight defaults set to the values chosen from the sweep, effect enabled."
parent: task-272-player-light-low-gradient
kind: task
status: done
---
# Mask defaults

- goal: MPC_PlayerLight defaults set to the values chosen from the sweep, effect enabled.
- agent: bp
- depends_on: task-272-16-flat-mask-sweep
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: MPC_PlayerLight defaults set to the sweep choice; doc updated.
- done_when: Defaults read back, saved, doc updated, lint clean, committed by path.
- editor_state: open
- token_ceiling: 30k
- model: sonnet
- tokens: input 20, cache write 34888, cache read 224972, output 265
- runs: 1
- milestone: M1
- result: Commit 37ca9160. MPC_PlayerLight defaults: Enable 0 -> 1, CenterBrightness 3 -> 30, FarBrightness 0.4 -> 0.15, Radius 225 -> 200 uu, Exponent 2 -> 3, WarmTint -> (1, 0.9, 0.06), FarTint -> (1, 0.7, 0.55), TintStrength 1; read back, saved; doc with revert (Enable 0) and tuning notes.

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
