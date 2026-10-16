---
id: task-203-08-health-bar-size
description: "Fix the tiny health bar after task-203-07: HealthBarRoot and its two bars fill 280 x 28 again, as before the reparent."
parent: task-203-health-bar-juice
kind: task
status: in-progress
---
# Health bar size back to 280 x 28

- goal: In game the health bar is drawn 280 x 28 at 20,232 again (owner screenshot 2026-10-16: since task-203-07 it shows as a tiny green block about 18 x 8 px above the text).
- agent: bp
- depends_on: task-203-07-wbp-health-status
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-health-bar (vault/reference/tech/systems/ui/sys-hud-health-bar.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- spec: Read back the HealthBarRoot canvas slot (anchors, offsets, size, auto size) and the Overlay slots of DamageChunkBar and HealthBar. Likely cause: Overlay slots default to Left / Top alignment, so both ProgressBars take their small desired size; set both to Fill / Fill. HealthBarRoot slot: anchor 0,0, position 20,232, size 280 x 28, auto size off. Change nothing else (C++ owns colours and the render transform). Record the cause found in wbp-health-status.
- expected_output: WBP_HealthStatus saved; wbp-health-status updated; one commit.
- done_when: Blueprint compiles with warnings as errors and is saved; slot values read back match the spec; wbp-health-status updated; lint clean; committed by path. The on-screen check is task-203-09.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-203-health-bar-juice]] — parent global task.
