---
id: task-204-01-empty-bar-color
description: "Set the empty part of the health bar to dark grey #0B0C14 in WBP_HealthStatus, through DamageChunkBar's background tint."
parent: task-204-health-bar-dark-empty
kind: task
status: done
---
# Empty bar colour

- goal: the empty part of the health bar shows #0B0C14 (sRGB) instead of the default light grey.
- agent: bp
- depends_on: none
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-health-bar (vault/reference/tech/systems/ui/sys-hud-health-bar.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- spec: In WBP_HealthStatus, HealthBar's background is transparent (task-203-07) and DamageChunkBar (behind it) draws the empty part with the default ProgressBar background. Set DamageChunkBar WidgetStyle.BackgroundImage tint to #0B0C14 read as sRGB (linear about 0.0033, 0.0037, 0.0070, alpha 1); keep its image, draw type and margins, so the 1 px top and bottom border stays. Do not touch fills (C++ sets them), slots or render transforms. Read back the value. Update wbp-health-status.
- expected_output: WBP_HealthStatus saved; wbp-health-status updated; one commit.
- done_when: Blueprint compiles with warnings as errors and is saved; tint read back; doc updated; lint clean; committed by path. Screen check is task-204-02.
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 32, cache write 39794, cache read 403076, output 657
- runs: 1
- milestone: M1
- result: DamageChunkBar background tint white -> linear (0.0033, 0.0037, 0.0070, 1) = #0B0C14 sRGB; image ProgressBar_Background (Box 0.4167) unchanged; compiled, saved, read back. Risk: tint multiplies the grey image (~0.49 linear), so the screen may be darker than #0B0C14; checked in task-204-02. Commit 84c6d4e2.

## Relations
- [[task-204-health-bar-dark-empty]] — parent global task.
