---
id: task-270-03-ambience-apply
description: "The light, exposure and readability values picked on the captures are set in the editor, read back and documented with the previous values."
parent: task-270-ambience-capture-tuning
kind: task
status: done
---
# Ambience values applied

- goal: The light, exposure and readability values picked on the captures are set in the editor, read back and documented with the previous values.
- agent: bp
- depends_on: task-270-02-ambience-sweep
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Values set on BP_Miner PlayerLight, DirectionalLight_0, SkyLight_0, PPV_XPGlow as listed in the delegation; docs updated with before and after.
- done_when: Values read back; compiled and saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 80k
- model: sonnet
- tokens: input 46, cache write 67895, cache read 827734, output 826
- runs: 1
- milestone: M1
- result: Commit 8ae872b9. BP_Miner PlayerLight attenuationRadius 650 -> 520, intensity 120 -> 650 cd, lightColor (1, 0.62, 0.30) -> (1, 0.80, 0.59), height 300 and shadows kept; DirectionalLight_0 intensity 0.12 -> 1.5 (violet, no shadows); SkyLight_0 0.08 and PPV_XPGlow exposure 1.3 / bloom 1.3 -1 unchanged (read back). Docs bp-miner and lights-dark-ambient updated with revert values.

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
