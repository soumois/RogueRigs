---
id: task-277-01-xp-glow-revert
description: "The XP pickups glow as before task-269: task-269-01 values reverted, PPV bloom 1.3 / -1 kept."
parent: task-277-xp-glow-revert
kind: task
status: in-progress
---
# XP glow back to before task-269

- goal: The XP pickups glow as before task-269: task-269-01 values reverted, PPV bloom 1.3 / -1 kept.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), perf-xp-pickups-glow (vault/reference/tech/performance/perf-xp-pickups-glow.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Revert of task-269-01 (commit 5b77c051), each value read before, set, read back, saved: M_XPNut_hole GlowAmount 8 -> 25; MI_XPGlowDisc_Nut Radius 22 -> 50; MI_XPGlowDisc_Chip Radius 32 -> 68 and Intensity 0.25 -> 0.5; M_XPCube_core_bright GlowAmount 6 -> 12; M_XPCube_core_dark 3 -> 6. PPV_XPGlow bloom 1.3 / -1 and exposure untouched. bp-pickup-xp gets an xp_glow_revert line (before, after, revert = task-269 values); perf-xp-pickups-glow task-269 note updated.
- done_when: Five values read back at their pre-task-269 values; assets saved; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 80k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-277-xp-glow-revert]] — parent global task.
