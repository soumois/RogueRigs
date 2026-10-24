---
id: task-311-02-bp-console-scale
description: "BP_ObjectiveConsole scale 1.361 (bp): console 50 % bigger, step 02."
parent: task-311-console-bigger-50
kind: task
status: in-progress
---
# BP_ObjectiveConsole scale 1.361

- goal: In BP_ObjectiveConsole set the inherited Mesh relative scale 0.90754 -> 1.361 uniform (= 201 / 147.65, the largest horizontal size 147.65 uu to 3 cells of 67 uu); AntennaHead stays attached under Mesh with its raw relative offset (-4.904, 16.69, 108.0) and scale 1; compile, save, read back; update bp-objective-console.
- agent: bp
- depends_on: task-311-01-console-footprint-3x3
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), objective-console (vault/reference/gdd/run/objective-console.md); files: /Game/RogueRigs/Blueprints/Objectives/BP_ObjectiveConsole
- expected_output: BP_ObjectiveConsole saved with Mesh scale 1.361, doc node updated
- done_when: Mesh scale read back about 1.361 on all three axes; AntennaHead offset and scale unchanged; compiles with no error; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-311-console-bigger-50]] — parent global task.
