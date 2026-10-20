---
id: task-246-01-card-pick-destination
description: "Create the dissolve (burn) material and instance for the discarded cards in /Game/RogueRigs/UI, with its doc node."
parent: task-246-card-pick-destination
kind: task
status: done
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
- runs: 1
- milestone: M2
- result: M_CardBurn (UI domain, translucent) and MI_CardBurn in /Game/RogueRigs/UI/Cards: Burn 0..1 dissolve bottom to top with noise and an orange fire edge (EdgeWidth 0.08, FireIntensity 4); doc node m-card-burn; commit 5b0d60e6; compiled, saved; look not seen, noise range assumption unverified. 1 run.

## Relations
- [[task-246-card-pick-destination]] — parent global task.
