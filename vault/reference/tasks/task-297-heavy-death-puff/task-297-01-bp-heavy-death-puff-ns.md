---
id: task-297-01-bp-heavy-death-puff-ns
description: "Create NS_HeavyDeathPuff, a scaled copy of NS_RobotDeathPuff that covers the heavy, with its doc node (bp)."
parent: task-297-heavy-death-puff
kind: task
status: done
---
# NS_HeavyDeathPuff

- goal: Duplicate NS_RobotDeathPuff as NS_HeavyDeathPuff (NS_RobotDeathPuff untouched): spawn sphere, sprite sizes, speed and count scaled so the puff covers a 248 cm tall, about 150 cm wide robot from its centre for at least the 0.15 s shrink (about 3x the robot puff); same colour, one-shot settings; new doc node bp-heavy-death-puff under blueprints-fx-index.
- agent: bp
- depends_on: none
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), color-code (vault/reference/gdd/art-audio/color-code.md); files: /Game/RogueRigs/FX/NS_RobotDeathPuff
- expected_output: /Game/RogueRigs/FX/NS_HeavyDeathPuff, doc node bp-heavy-death-puff
- done_when: compiles without error; values read back; NS_RobotDeathPuff unchanged; doc node and index relation added; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 30000
- model: sonnet
- tokens: input 60, cache write 62736, cache read 1111748, output 14367
- runs: 1
- milestone: M2
- result: NS_HeavyDeathPuff duplicated from NS_RobotDeathPuff: count 24, sphere 105 cm, speed 180-780, sprite 42-150 cm, same colour and lifetime 0.2-0.3 s; compiles clean; robot puff unchanged; doc bp-heavy-death-puff (62e472e3).

## Relations
- [[task-297-heavy-death-puff]] — parent global task.
