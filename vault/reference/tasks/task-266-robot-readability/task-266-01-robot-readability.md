---
id: task-266-01-robot-readability
description: "The robot bodies read better: the yellow is no longer burnt and the robots separate from the floor."
parent: task-266-robot-readability
kind: task
status: todo
---
# Robot readability

- goal: The robot bodies read better: the yellow is no longer burnt and the robots separate from the floor.
- agent: bp
- depends_on: task-263-02-player-light-shadows, task-264-02-blob-shadows, task-265-01-scene-ambient-dark
- inputs: lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), bp-kamikaze (vault/reference/tech/blueprints/enemies/bp-kamikaze.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md)
- expected_output: After 263 to 265: the chaser body materials (M_Chaser_Meshy_Anim and the shield chaser variants) get a slightly darker, less saturated yellow base, higher roughness, and a faint cool rim (Fresnel, low intensity, not bloomed) that separates the body from the floor, all as scalar parameters the owner can tune; eyes and logo plate unchanged (red eyes, yellow plate per color-code); the other robots checked for the same burn and only adjusted if they show it; documented; no bloom exclusion (decided later by the owner).
- done_when: Material parameters read back with before and after values; compile OK; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-266-robot-readability]] — parent global task.
