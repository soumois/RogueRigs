---
id: task-245-01-card-hover-reaction
description: "In WBP_UpgradeChoice, replace the instant glow change by a hover animation: tilt following the pointer x (about 4 degrees), scale 1.06, eased return, sheen sweep on blue and above using the existin..."
parent: task-245-card-hover-reaction
kind: task
status: todo
---
# 245-01 bp

- goal: In WBP_UpgradeChoice, replace the instant glow change by a hover animation: tilt following the pointer x (about 4 degrees), scale 1.06, eased return, sheen sweep on blue and above using the existing frame textures; update the doc node.
- agent: bp
- depends_on: task-244-02-card-rarity-suspense
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: updated widget and doc node
- done_when: compiles with warnings as errors, saved, hover graph read back; doc node updated
- editor_state: open
- token_ceiling: 60000 (provisional)
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-245-card-hover-reaction]] — parent global task.
