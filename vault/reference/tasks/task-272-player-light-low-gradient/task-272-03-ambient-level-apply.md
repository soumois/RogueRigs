---
id: task-272-03-ambient-level-apply
description: "Sun, sky light and PPV_XPGlow exposure of L_ArenaM1 set to the sweep values and documented; skipped if the sweep keeps them."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Level ambient applied

- goal: Level ambient of L_ArenaM1 (sun, sky light, PPV_XPGlow exposure) set to the values chosen from the sweep, read back and documented; skipped if the sweep keeps them.
- agent: bp
- depends_on: task-272-01-player-light-low-gradient
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Level actor values as listed in the delegation; docs updated with before, after and revert values.
- done_when: Values read back; level saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
