---
id: task-176-01-lone-stat-icon-larger
description: "Stat icon 25 % larger (156 px) when shown alone on a card; unchanged next to a weapon icon."
parent: task-176-lone-stat-icon-larger
kind: task
status: done
---
# Upgrade cards: lone stat icon larger

- goal: Make the stat icon 25 % larger (125 to 156 px) on cards where it is shown alone (All weapons or player stat); unchanged next to a weapon icon.
- agent: bp
- depends_on: none
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: WBP_UpgradeChoice: StatIcon box 156 x 156 when WeaponBox is collapsed (All weapons, player stat), 100 x 100 kept next to a weapon icon; doc updated.
- done_when: Compiles, saved, sizes read back for both cases, doc updated, committed. Owner checks on screen.
- editor_state: open
- token_ceiling: 40 turns
- model: sonnet
- tokens: input 52, cache write 85363, cache read 1405987, output 572
- runs: 1
- milestone: M2
- result: StyleCard sets StatIcon desired size 156 x 156 when shown alone (Player, All weapons) and 100 x 100 next to a weapon icon; StatIconBox overrides off. Compiled with warnings as errors, saved, read back, doc updated. Not seen on screen. Commit 4d4c472.

## Relations
- [[task-176-lone-stat-icon-larger]] — parent global task.
