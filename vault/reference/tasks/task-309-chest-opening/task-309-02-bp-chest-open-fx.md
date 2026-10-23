---
id: task-309-02-bp-chest-open-fx
description: "NS_BonusChestOpen and its wiring (bp): bonus chest opening: latch, lid, particle and xp burst, step 02."
parent: task-309-chest-opening
kind: task
status: todo
---
# NS_BonusChestOpen and its wiring

- goal: One-shot Niagara NS_BonusChestOpen: gold-yellow sparks and a short flash rising out of the opened lid (default Q12); spawned once by BP_BonusChest on OnLidOpened; doc node bp-bonus-chest-open under blueprints-fx-index and bp-bonus-chest updated.
- agent: bp
- depends_on: task-309-01-chest-opening-sequence
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), color-code (vault/reference/gdd/art-audio/color-code.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md); files: none
- expected_output: /Game/RogueRigs/FX/NS_BonusChestOpen, BP_BonusChest wiring, doc nodes
- done_when: compiles without error; values read back; wired event read back; doc nodes and index relation; lint clean; committed
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-309-chest-opening]] — parent global task.
