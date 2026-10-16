---
id: task-203-07-wbp-health-status
description: "WBP_HealthStatus uses the new C++ class and holds the widgets it drives; its doc node matches."
parent: task-203-health-bar-juice
kind: task
status: todo
---
# WBP_HealthStatus layout for the juicy bar

- goal: WBP_HealthStatus uses the new C++ class and holds the widgets it drives; its doc node matches.
- agent: bp
- depends_on: task-203-06-color-states
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), sys-hud-health-bar (created by task-203-01)
- spec: Reparent WBP_HealthStatus to URRHealthStatusWidget. Wrap the bar in HealthBarRoot (SizeBox or Overlay at 20,232, 280 x 28, pivot 0.5/0.5) holding DamageChunkBar (ProgressBar, behind) and HealthBar (front, transparent background). In ShowHealth, remove the writes to HealthBar percent and HealthText (C++ owns them now); keep LowHealthAlpha, the vignette, OnDamaged vignette logic and GAME OVER unchanged. Compile with warnings as errors, save, read back the tree and graphs. Update wbp-health-status (doc rule). One main asset.
- expected_output: WBP_HealthStatus saved, wbp-health-status updated; one commit.
- done_when: Blueprint compiles with warnings as errors and is saved; tree and graphs read back match the spec; wbp-health-status updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-203-health-bar-juice]] — parent global task.
