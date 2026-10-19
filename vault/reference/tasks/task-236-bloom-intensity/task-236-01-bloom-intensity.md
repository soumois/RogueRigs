---
id: task-236-01-bloom-intensity
description: "The arena post-process bloom intensity goes back to 1.3 (now 1.8)."
parent: task-236-bloom-intensity
kind: task
status: done
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
- runs: 1
- milestone: M2
- result: The owner sets PPV_XPGlow (L_ArenaM1) BloomIntensity to 1.3 himself (was 1.8); agent stopped before any change; value recorded in ppv-xp-glow.

## Relations
- [[task-236-bloom-intensity]] — parent global task.
