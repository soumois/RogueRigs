---
id: task-288-08-bp-ns-shooter-debris
description: "NS_ShooterDebris with its pieces (bp)."
parent: task-288-robot-death-debris
kind: task
status: done
---
# NS_ShooterDebris with its pieces

- goal: Import the shooter debris FBX into /Game/RogueRigs/Art/Shooter/Debris/ (StaticMeshTools.import_file, centimetres, no collision, Nanite off, one LOD, no material or texture imported) with MI_ShooterDebris on every slot, then create NS_ShooterDebris as a copy of NS_RobotDebris whose mesh renderer lists these pieces in debris_pieces.json order and whose PieceLocal table holds their centre offsets; NS_RobotDebris (v1) untouched.
- agent: bp
- depends_on: task-288-06-bp-debris-material
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md); files: Art/Source/shooter-meshy/debris/ (FBX and debris_pieces.json)
- expected_output: meshes in /Game/RogueRigs/Art/Shooter/Debris/, /Game/RogueRigs/FX/NS_ShooterDebris saved; bp-robot-debris doc updated (one line per new system)
- done_when: mesh bounds read back match the json (Y mirrored), renderer mesh list and offset table read back, system compiles UpToDate (GPU script too), git diff shows NS_RobotDebris and the v1 debris meshes unchanged; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 88, cache write 67792, cache read 2126062, output 18205
- runs: 1
- milestone: M2
- result: 13 SM_ShooterDebris_* in Art/Shooter/Debris (MI_ShooterDebris, sizes = json); NS_ShooterDebris renderer list and PieceLocal read back, UpToDate incl. GPU; v1 unchanged.

## Relations
- [[task-288-robot-death-debris]] — parent global task.
