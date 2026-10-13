---
id: task-144-01-xp-chip-glow-12-6
description: "Set the big XP chip core glow to 12 and 6, a starting point to tune by eye."
parent: task-144-xp-chip-glow-12-6
kind: task
status: todo
---
# XP chip: glow 12 and 6

- goal: Set the big XP chip core glow to 12 and 6, a starting point to tune by eye.
- agent: bp
- depends_on: none
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: M_XPChip_core_bright GlowAmount 12 and M_XPChip_core_dark GlowAmount 6 (colours and PulseAmount unchanged), read back, saved with save_assets (these two assets only); if a save fails, STOP and report: no UI automation, no key presses, nothing else saved; doc bp-pickup-xp updated (values, provisional starting point, owner 2026-10-13).
- done_when: GlowAmount reads 12 and 6, both assets saved and modified in git, nothing else changed, doc updated
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-144-xp-chip-glow-12-6]] — parent global task.
