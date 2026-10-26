---
id: task-315-02-bp-reroll-button
description: "WBP_UpgradeChoice reroll button shows its ore cost and is greyed when unaffordable (bp): paid level-up reroll, step 02."
parent: task-315-paid-reroll
kind: task
status: done
---
# Reroll button with its cost

- goal: In WBP_UpgradeChoice the RerollButton shows "Reroll" with the ore icon (T_Icon_Ore) and the current cost from the level-up component (GetRerollCost); it is enabled when CanReroll is true and greyed (disabled) otherwise, also after a reroll; the deal sequence still decides when the button becomes clickable (RerollAllowed). Cards, animations and other graphs unchanged. Update the doc node wbp-upgrade-choice.
- agent: bp
- depends_on: task-315-01-paid-reroll
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md, as updated by task-315-01)
- expected_output: /Game/RogueRigs/UI/WBP_UpgradeChoice changed, doc node wbp-upgrade-choice updated
- done_when: compiled with warnings as errors and saved; tree and graph wiring read back; doc node updated; lint clean; committed
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 62, cache write 82911, cache read 1727912, output 15470
- usage: turns 31, context start 10k peak 83k, resumes 0, 5 min, units 0.35 M
- runs: 1
- milestone: M2
- result: RerollButton holds RerollRow (RerollText, RerollOre T_Icon_Ore 32x32, RerollCost variable); RefreshReroll sets the cost from GetRerollCost on every ShowCards; enabled state still bCanReroll via RerollAllowed at the end of the deal; compiled, saved, read back; not seen in PIE; commit 9abff278

## Relations
- [[task-315-paid-reroll]] — parent global task.
