---
id: task-265-01-scene-ambient-dark
description: "A dark violet ambient with the sun lowered, so the player light is the main light and bright bodies are no longer burnt."
parent: task-265-scene-ambient-dark
kind: task
status: todo
---
# Dark violet ambient

- goal: A dark violet ambient with the sun lowered, so the player light is the main light and bright bodies are no longer burnt.
- agent: bp
- depends_on: none
- inputs: lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md)
- expected_output: On L_ArenaM1: DirectionalLight intensity 6 -> about 1 with a violet tint, SkyLight intensity and tint tuned to a dark violet ambient, castShadows stays false on both (task-261); no exposure or Lumen change beyond what is needed (state any); values read back and documented in lights-no-shadows (or a sibling node) with the previous values for revert; provisional, owner tunes by eye.
- done_when: Lights read back with the new values and the previous ones recorded for revert; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-265-scene-ambient-dark]] — parent global task.
