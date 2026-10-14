---
id: task-169-04-weapon-card-layout
description: "New-weapon card layout in WBP_UpgradeChoice: New weapon, icon only, name, explanation, lore, unique frame."
parent: task-169-weapon-card-design
kind: task
status: todo
---
# New-weapon cards: WBP_UpgradeChoice variant

- goal: Show new-weapon cards in their own layout: "New weapon" on top, the weapon icon only, the weapon name, the explanation, the lore line, inside the unique weapon frame.
- agent: bp
- depends_on: task-168-05-upgrade-choice-layout, task-169-02-weapon-card-frame, task-169-03-weapon-card-lore
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: WBP_UpgradeChoice: for CardType NewWeapon, same card size as stat cards; top "New weapon", image area with the weapon icon only, weapon name (TargetText), explanation (DescriptionText), lore (LoreText, italic), frame T_CardFrame_Weapon untinted; wbp-upgrade-choice doc updated.
- done_when: Compiles with warnings as errors, saved, tree and graphs read back, doc updated; committed. Not seen on screen: say so.
- editor_state: open
- token_ceiling: 40 turns
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-169-weapon-card-design]] — parent global task.
