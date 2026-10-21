---
id: task-270-03-ambience-apply
description: "The light, exposure and readability values picked on the captures are set in the editor, read back and documented with the previous values."
parent: task-270-ambience-capture-tuning
kind: task
status: todo
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
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-270-ambience-capture-tuning]] — parent global task.
