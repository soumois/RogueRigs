---
id: task-301-04-bp-console-c2-import
description: "Import the console C2 into BP_ObjectiveConsole (bp): console c2 uplink mast mesh, step 04."
parent: task-301-console-c2-mesh
kind: task
status: done
---
# Import the console C2 into BP_ObjectiveConsole

- goal: Import console_c2_base and console_c2_head with the texture; material M_ConsoleLamp (vector LampColor, scalar BlinkRate and BlinkAmount, emissive) with MI_ConsoleLampAntenna and MI_ConsoleLampOther, material M_ConsoleScreen (vector ScreenColor, default black); in BP_ObjectiveConsole replace the engine cylinder by the base mesh and add a static mesh component AntennaHead on the mast axis with the head mesh, NoCollision, no dynamic shadow; record component and slot names in bp-objective-console (task-302 drives them by these names).
- agent: bp
- depends_on: task-301-03-console-c2-retouch
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), blueprints-world-index (vault/reference/tech/blueprints/world/blueprints-world-index.md), bp-drone-materials (vault/reference/tech/blueprints/player/bp-drone-materials.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md); files: Art/Source/console-meshy/clean/
- expected_output: meshes, texture, M_ConsoleLamp + 2 MI, M_ConsoleScreen under /Game/RogueRigs/; BP_ObjectiveConsole updated; doc node bp-objective-console updated
- done_when: compiles without error; mesh, component and slot assignments read back; footprint size kept (2 x 2 cells); doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 30000
- model: sonnet
- tokens: input 96, cache write 93959, cache read 2694250, output 33970
- usage: turns 48, context start 10k peak 97k, resumes 0, 13 min, units 0.56 M
- runs: 1
- milestone: M2
- result: Imported SM_ConsoleBase, SM_ConsoleHead (headless FbxImportUI scale 1; MCP Interchange import was 100x too small), T_Console_BaseColor, M_ConsoleBody+MI, M_ConsoleLamp (LampColor, BlinkRate, BlinkAmount) with MI_ConsoleLampOther and MI_ConsoleLampAntenna, M_ConsoleScreen (ScreenColor) under /Game/RogueRigs/Art/Console/; BP_ObjectiveConsole Mesh = SM_ConsoleBase at z 0, uniform scale 0.90754 (134 x 115 uu footprint), child AntennaHead at (-4.904, 16.69, 108.0) NoCollision; OnHackProgress GlowAmount call left as no-op for task-302; doc updated; commit 59fbc6ad.

## Relations
- [[task-301-console-c2-mesh]] — parent global task.
