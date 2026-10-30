---
id: task-381-01-level-end-sprites-import
description: "Bp: import the three level-end sprites from Art/UI/LevelEnd as UI textures in /Game/RogueRigs/UI/LevelEnd."
parent: task-381-level-end-screen-rebuild
kind: task
status: todo
---
# Level-end screen rebuild: import the sprites

- goal: Import `Art/UI/LevelEnd/T_LevelEnd_Frame.png`, `T_LevelEnd_DividerH.png` and `T_LevelEnd_DividerV.png` into `/Game/RogueRigs/UI/LevelEnd/`, names kept, with the settings of task-348-05 (texture group UI, NoMipmaps, sRGB, TC_EditorIcon so alpha is kept, NeverStream true). No widget change.
- agent: bp
- depends_on: none
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), task-348-05-shop-sprites-import (vault/reference/tasks/task-348-shop-screen-redesign/task-348-05-shop-sprites-import.md) (the import settings used for the Spare Parts sprites), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md) (section Sprites: files and sizes)
- expected_output: three texture assets saved; change summary (paths, settings, sizes); one commit `task-381-01-level-end-sprites-import: <summary>`.
- done_when: the three textures exist in /Game/RogueRigs/UI/LevelEnd/, settings read back on each, sizes 942 x 458, 984 x 62 and 58 x 984; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-381-level-end-screen-rebuild]] — parent global task.
