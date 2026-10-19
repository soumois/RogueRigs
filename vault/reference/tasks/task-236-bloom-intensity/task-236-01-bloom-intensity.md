---
id: task-236-01-bloom-intensity
description: "The arena post-process bloom intensity goes back to 1.3 (now 1.8)."
parent: task-236-bloom-intensity
kind: task
status: in-progress
---
# Bloom intensity 1.3

- goal: The arena post-process bloom intensity goes back to 1.3 (now 1.8).
- agent: bp
- depends_on: none
- inputs: bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md)
- expected_output: The post-process volume (or camera) that sets Lens > Bloom > Intensity 1.8 found (L_ArenaM1 PostProcessVolume, BP camera or project default; say which); Intensity set to 1.3 with its override checkbox kept; level or asset saved; the doc node of that asset updated (or a line in the relevant world doc if none exists).
- done_when: value read back 1.3; saved; doc updated; committed.
- editor_state: open
- token_ceiling: 15000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-236-bloom-intensity]] — parent global task.
