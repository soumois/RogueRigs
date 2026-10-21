---
id: task-263-02-player-light-shadows
description: "A movable warm point light on BP_Miner is the only cast-shadow source."
parent: task-263-player-light-shadows
kind: task
status: todo
---
# Player light on BP_Miner

- goal: A movable warm point light on BP_Miner is the only cast-shadow source.
- agent: bp
- depends_on: task-263-01-player-light-shadows
- inputs: lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: BP_Miner gets a PointLightComponent `PlayerLight`: movable, cast shadows true, warm colour about (1.0, 0.62, 0.30), intensity tuned so the pool of light reads strongly next to the dim violet ambient of task-265 (state the value and the unit), attenuation radius about 1400 uu, source radius small, relative height about 300 uu above the capsule centre, no light function; doc bp-miner updated with every value and the revert; values provisional, the owner tunes by eye.
- done_when: BP_Miner compiles; the light component and its values read back; DirectionalLight and SkyLight still cast no shadow; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-263-player-light-shadows]] — parent global task.
