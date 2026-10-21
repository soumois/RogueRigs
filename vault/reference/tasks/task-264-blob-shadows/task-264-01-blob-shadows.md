---
id: task-264-01-blob-shadows
description: "Simple fake round shadows under the enemies, the miner and the drone."
parent: task-264-blob-shadows
kind: task
status: done
---
# Blob shadows

- goal: Simple fake round shadows under the enemies, the miner and the drone.
- agent: bp
- depends_on: none
- inputs: lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-enemies (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-drone-parts (vault/reference/tech/systems/combat/sys-drone/sys-drone-parts.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), perf-robot-components-audit (vault/reference/tech/performance/perf-robot-components-audit.md), perf-lead-lumen-robots (vault/reference/tech/performance/perf-leads/perf-lead-lumen-robots.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md)
- expected_output: M_BlobShadow at /Game/RogueRigs/FX/BlobShadow/: unlit translucent dark radial gradient (soft round edge), usable on an engine plane lying on the floor; node bp-blob-shadow-material; then a cpp subtask adds a cheap blob component (shared engine Plane mesh, no collision, no shadow, scaled to the actor footprint, kept on the floor under the actor) to the robot base class, the miner and the drone.
- done_when: Material compiles with read-back of its parameters (opacity, softness, colour); doc node written; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: sonnet
- tokens: input 42, cache write 79907, cache read 1137981, output 2943
- runs: 1
- milestone: M1
- result: M_BlobShadow (unlit translucent, two sided, radial soft disc from the Plane UVs, no texture) and MI_BlobShadow (Opacity 0.45, Softness 1.6, Tint (0.02, 0, 0.04)) under /Game/RogueRigs/FX/BlobShadow/; compiles; node bp-blob-shadow-material. Translucency sort priority is not a material property in 5.8: set it on the component in 264-02. Not seen on screen.

## Relations
- [[task-264-blob-shadows]] — parent global task.
