---
id: task-246-01-card-pick-destination
description: "Create the dissolve (burn) material and instance for the discarded cards in /Game/RogueRigs/UI, with its doc node."
parent: task-246-card-pick-destination
kind: task
status: in-progress
---
# 246-01 bp

- goal: Create the dissolve (burn) material and instance for the discarded cards in /Game/RogueRigs/UI, with its doc node.
- agent: bp
- depends_on: task-245-02-card-hover-reaction
- inputs: bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md)
- expected_output: material, instance, doc node
- done_when: material compiles, saved, parameters read back; doc node written
- editor_state: open
- token_ceiling: 40000 (provisional)
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-246-card-pick-destination]] — parent global task.
