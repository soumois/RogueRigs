---
id: task-288-09-bp-ns-chaser-v2-debris
description: "NS_ChaserV2Debris with its pieces (bp)."
parent: task-288-robot-death-debris
kind: task
status: done
---
# NS_ChaserV2Debris with its pieces

- goal: Import the chaser v2 debris FBX into /Game/RogueRigs/Art/ChaserV2/Debris/ (StaticMeshTools.import_file, centimetres, no collision, Nanite off, one LOD, no material or texture imported) with MI_ChaserV2Debris on every slot, then create NS_ChaserV2Debris as a copy of NS_RobotDebris whose mesh renderer lists these pieces in debris_pieces.json order and whose PieceLocal table holds their centre offsets; NS_RobotDebris (v1) untouched.
- agent: bp
- depends_on: task-288-06-bp-debris-material
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md); files: Art/Source/chaser-v2-meshy/debris/ (FBX and debris_pieces.json)
- expected_output: meshes in /Game/RogueRigs/Art/ChaserV2/Debris/, /Game/RogueRigs/FX/NS_ChaserV2Debris saved; bp-robot-debris doc updated (one line per new system)
- done_when: mesh bounds read back match the json (Y mirrored), renderer mesh list and offset table read back, system compiles UpToDate (GPU script too), git diff shows NS_RobotDebris and the v1 debris meshes unchanged; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 98, cache write 71003, cache read 2516682, output 19595
- runs: 1
- milestone: M2
- result: 13 SM_ChaserV2Debris_* in Art/ChaserV2/Debris (MI_ChaserV2Debris, sizes = json); NS_ChaserV2Debris renderer list and PieceLocal read back, UpToDate incl. GPU; v1 unchanged.

## Relations
- [[task-288-robot-death-debris]] — parent global task.
