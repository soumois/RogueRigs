---
id: task-002-02-delete-template-content
description: "Delete the TopDown, Variant_Strategy and Cursor content folders through the editor and fix up redirectors, keeping TwinStick intact."
parent: task-002-cleanup-twinstick
kind: task
status: done
---
# Delete template content

- goal: the TopDown and Strategy template assets are gone and nothing kept references them.
- agent: bp
- depends_on: task-002-01-twinstick-defaults
- inputs: [[unreal-mcp]], the reference report of task-002-01.
- expected_output: deleted through the editor (asset tools, not the file system): `/Game/TopDown/` (including `Lvl_TopDown` and its `__ExternalActors__`/`__ExternalObjects__` data), `/Game/Variant_Strategy/` (including `LVL_Strategy`), `/Game/Cursor/`. Kept untouched: `/Game/Variant_TwinStick/`, `/Game/Characters/`, `/Game/LevelPrototyping/`, `/Game/RogueRigs/`, `/Game/Collections/`, `/Game/Developers/`. First move `/Game/TopDown/MI_Colorway` to `/Game/Variant_TwinStick/Materials/MI_Colorway` (rename/move with redirector fixup) and move its parent material too if it lives in a deleted folder, because Variant_TwinStick assets use it (task-002-01). Then check that no kept asset references a deleted one; if one still does, stop and report it instead of deleting. Fix up redirectors in the kept folders afterwards. Save all.
- done_when: the three folders no longer exist on disk; `L_ArenaM1`, `BP_TwinStickCharacter` and `BP_TwinStickGameMode` load without missing-reference warnings; no unsaved asset left; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: MI_Colorway moved to `/Game/Variant_TwinStick/Materials/` (parent MI_DefaultColorway stays in LevelPrototyping), referencers re-pointed by hand (no fixup-redirectors tool), no redirector left. Deleted through the editor: TopDown, Variant_Strategy, Cursor and their __ExternalActors__/__ExternalObjects__ folders. 195 kept assets scanned, no reference to deleted folders; key assets load. About 92k tokens (over the 40k ceiling: manual redirector fixup). MCP quirks for [[unreal-mcp]] passed to task-002-05.

## Relations
- [[task-002-cleanup-twinstick]] — parent global task.
