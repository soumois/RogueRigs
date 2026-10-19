---
id: task-244-01-card-rarity-suspense
description: "In WBP_UpgradeChoice, flip purple and yellow cards last, glow their back while waiting, darken the other cards, then on reveal a scale punch, a flash and rays in the tier colour built from T_CardFr..."
parent: task-244-card-rarity-suspense
kind: task
status: in-progress
---
# 244-01 bp

- goal: In WBP_UpgradeChoice, flip purple and yellow cards last, glow their back while waiting, darken the other cards, then on reveal a scale punch, a flash and rays in the tier colour built from T_CardFrame_Stat_Glow_Baked; extra delay about 0.6 s; update the doc node.
- agent: bp
- depends_on: task-243-06-card-deal-flip
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: updated widget and doc node
- done_when: compiles with warnings as errors, saved, order and effects read back; doc node updated
- editor_state: open
- token_ceiling: 60000 (provisional)
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-244-card-rarity-suspense]] — parent global task.
