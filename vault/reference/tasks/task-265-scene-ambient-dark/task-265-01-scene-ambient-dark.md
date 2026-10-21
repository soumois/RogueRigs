---
id: task-265-01-scene-ambient-dark
description: "A dark violet ambient with the sun lowered, so the player light is the main light and bright bodies are no longer burnt."
parent: task-265-scene-ambient-dark
kind: task
status: done
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
- model: sonnet
- tokens: input 50, cache write 57308, cache read 989015, output 2615
- runs: 1
- milestone: M1
- result: DirectionalLight_0 intensity 6 -> 1.0 with colour (0.75, 0.62, 1.0); SkyLight_0 intensity 0.3 -> 0.5 with colour (0.35, 0.25, 0.6); castShadows stay false; no exposure, bloom, Lumen or material change; node lights-dark-ambient with previous values for revert. Risks: default auto exposure may brighten the dark scene back; the sky light (captured scene, real-time capture off, no cubemap) may contribute nothing until recaptured. Saved with the documented empty save_assets route (only the two light files changed). Not seen on screen.

## Relations
- [[task-265-scene-ambient-dark]] — parent global task.
