---
id: task-169-04-weapon-card-layout
description: "New-weapon card layout in WBP_UpgradeChoice: New weapon, icon only, name, explanation, lore, unique frame."
parent: task-169-weapon-card-design
kind: task
status: done
---
# New-weapon cards: WBP_UpgradeChoice variant

- goal: Show new-weapon cards in their own layout: "New weapon" on top, the weapon icon only, the weapon name, the explanation, the lore line, inside the unique weapon frame.
- agent: bp
- depends_on: task-168-05-upgrade-choice-layout, task-169-02-weapon-card-frame, task-169-03-weapon-card-lore
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: WBP_UpgradeChoice: for CardType NewWeapon, same card size as stat cards; top "New weapon", image area with the weapon icon only, weapon name (TargetText), explanation (DescriptionText), lore (LoreText, italic), frame T_CardFrame_Weapon untinted; wbp-upgrade-choice doc updated. Lore line always anchored at the bottom of the card content box, whatever the text above (owner 2026-10-14, mockup validated otherwise); the free space goes between the target or explanation and the lore.
- done_when: Compiles with warnings as errors, saved, tree and graphs read back, doc updated; committed. Not seen on screen: say so.
- editor_state: open
- token_ceiling: 40 turns
- model: sonnet
- tokens: input 100, cache write 126109, cache read 4091805, output 1078
- runs: 2
- milestone: M2
- result: NewWeapon variant in WBP_UpgradeChoice: WeaponGroup<i> overlay with FrameWeapon (T_CardFrame_Weapon Box brush, untinted), WeaponHeader 'New weapon' on the plate (Bold 24), weapon icon only (308 x 230, aspect kept), WeaponName (Bold 30), WeaponDesc (DescriptionText, 17 #C9CCD4); lore in CardDesc (LoreText) bottom-anchored; teal border removed. Stat cards unchanged. Compiles with warnings as errors, saved, doc updated. Breaks RRWeaponsIntegrationTests (CardDesc now holds the lore on weapon cards): fixed in task-169-05. Not seen on screen. Commits 0c0c9fe, 1875355.

## Relations
- [[task-169-weapon-card-design]] — parent global task.
