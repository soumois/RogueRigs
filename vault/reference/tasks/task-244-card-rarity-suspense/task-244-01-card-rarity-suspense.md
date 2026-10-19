---
id: task-244-01-card-rarity-suspense
description: "In WBP_UpgradeChoice, flip purple and yellow cards last, glow their back while waiting, darken the other cards, then on reveal a scale punch, a flash and rays in the tier colour built from T_CardFr..."
parent: task-244-card-rarity-suspense
kind: task
status: done
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
- runs: 1
- milestone: M2
- result: WBP_UpgradeChoice: purple and yellow cards flip last (non-rare 0.3/0.45/0.6 s, rare at 1.4 + 0.5 j), glow pulse on the rare back, Dim overlay on the others, punch 1.12, tier flash 0.5 to 0 in 0.2 s, two rays (glow texture copies) fading in 0.6 s; end 0.8 s without a rare card, else 0.8 + 1.4 + 0.5 (n-1). Commits d1ee2dd5, f9ba66f7; compiled warnings as errors, saved, graphs read back, nothing seen (capture in 244-02). 1 run.

## Relations
- [[task-244-card-rarity-suspense]] — parent global task.
