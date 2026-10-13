---
id: task-142-01-xp-bloom-post-process
description: "Add a PostProcessVolume with explicit bloom settings to the arena level so the halo effect can be tuned and tested."
parent: task-142-xp-bloom-post-process
kind: task
status: in-progress
---
# Arena: post process bloom volume

- goal: Add a PostProcessVolume with explicit bloom settings to the arena level so the halo effect can be tuned and tested.
- agent: bp
- depends_on: none
- inputs: unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md)
- expected_output: One PostProcessVolume actor, Infinite Extent (Unbound), in the persistent level /Game/RogueRigs/Maps/L_ArenaM1 (find the exact level path with SceneTools or AssetTools), labelled PPV_XPGlow, with bOverride_BloomMethod and bOverride_BloomIntensity and bOverride_BloomThreshold on, BloomMethod standard (Gaussian blur), BloomIntensity 1.0 (engine default 0.675), BloomThreshold -1 (engine default), nothing else overridden (no exposure, no Lumen settings); level saved; read back; a doc line in the most fitting node (the world or level Blueprint docs under tech/blueprints, or a new small doc node in the same folder if none fits, English, with the values and how to tune them). It is global to the level (it also affects the ore crystals): say so in the doc. Editor OPEN (the owner is using it; keep it open, save only the assets you name), MCP up, no CaptureEditorImage, Screenshot or CaptureViewport; read every value back; update the doc node bp-pickup-xp; commit by name; the three XP tasks run one after the other, never together.
- done_when: The volume exists with the listed overrides read back, level saved and git shows it modified, doc written, lint clean
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-142-xp-bloom-post-process]] — parent global task.
