---
id: task-243-05-card-deal-flip
description: "Import the five back textures and, in WBP_UpgradeChoice, add a back layer per card, the arrival in a fan (ease-out-back, about 0.15 s apart) and the flip one by one (width scaled by |cos|, swap at ..."
parent: task-243-card-deal-flip
kind: task
status: in-progress
---
# 243-05 bp

- goal: Import the five back textures and, in WBP_UpgradeChoice, add a back layer per card, the arrival in a fan (ease-out-back, about 0.15 s apart) and the flip one by one (width scaled by |cos|, swap at half); buttons disabled until all flips end, total about 0.8 s; the reroll replays it. Update the doc node wbp-upgrade-choice.
- agent: bp
- depends_on: task-243-04-card-deal-flip
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md)
- expected_output: imported textures, updated widget, updated doc node
- done_when: widget compiles with warnings as errors and is saved; the animation is driven by the offer events; doc node updated
- editor_state: open
- token_ceiling: 60000 (provisional)
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-243-card-deal-flip]] — parent global task.
