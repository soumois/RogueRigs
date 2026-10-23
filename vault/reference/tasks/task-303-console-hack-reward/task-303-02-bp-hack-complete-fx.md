---
id: task-303-02-bp-hack-complete-fx
description: "NS_ConsoleHackComplete and its wiring (bp): console hack reward: xp burst and big particle, step 02."
parent: task-303-console-hack-reward
kind: task
status: todo
---
# NS_ConsoleHackComplete and its wiring

- goal: One-shot Niagara NS_ConsoleHackComplete: a green ring expanding past the console then fading, plus white-gold sparks; spawned once by BP_ObjectiveConsole on hack complete; doc node bp-console-hack-complete under blueprints-fx-index and bp-objective-console updated.
- agent: bp
- depends_on: task-303-01-console-xp-burst
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), color-code (vault/reference/gdd/art-audio/color-code.md), objective-console (vault/reference/gdd/run/objective-console.md); files: none
- expected_output: /Game/RogueRigs/FX/NS_ConsoleHackComplete, BP_ObjectiveConsole wiring, doc nodes
- done_when: compiles without error; values read back; wired event read back; doc nodes and index relation; lint clean; committed
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-303-console-hack-reward]] — parent global task.
