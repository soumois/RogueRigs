---
id: task-272-03-ambient-level-apply
description: "Sun, sky light and PPV_XPGlow exposure of L_ArenaM1 set to the sweep values and documented; skipped if the sweep keeps them."
parent: task-272-player-light-low-gradient
kind: task
status: done
---
# Level ambient applied

- goal: Level ambient of L_ArenaM1 (sun, sky light, PPV_XPGlow exposure) set to the values chosen from the sweep, read back and documented; skipped if the sweep keeps them.
- agent: bp
- depends_on: task-272-05-low-light-sweep-r2
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Level actor values as listed in the delegation; docs updated with before, after and revert values.
- done_when: Values read back; level saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 40k
- model: sonnet
- tokens: input 32, cache write 70686, cache read 755404, output 309
- runs: 1
- milestone: M1
- result: Commit 65ac5de9. DirectionalLight_0 intensity 1.5 -> 0.3 (violet colour, no shadows kept); PPV_XPGlow exposure min = max 1.3 -> 1.8 EV100 (= sweep r.ExposureOffset -0.5), bias 1 and bloom unchanged; SkyLight_0 0.08 unchanged; read back after save. Docs lights-dark-ambient, ppv-xp-glow updated (revert sun 1.5, exposure 1.3).

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
