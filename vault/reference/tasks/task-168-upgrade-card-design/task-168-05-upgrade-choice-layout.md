---
id: task-168-05-upgrade-choice-layout
description: "Rebuild the stat cards of WBP_UpgradeChoice: large cards, icons, effect, target, lore, rarity-tinted accents and inner glow, reroll below."
parent: task-168-upgrade-card-design
kind: task
status: in-progress
---
# Upgrade cards: WBP_UpgradeChoice layout

- goal: Rebuild the stat cards of WBP_UpgradeChoice: large cards, image (stat icon alone, or weapon icon with the stat icon on its right), effect, target, lore, rarity-tinted frame, reroll centred below.
- agent: bp
- depends_on: task-168-02-stat-icons, task-168-03-card-frames, task-168-04-card-icon-data
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), rarities (vault/reference/gdd/weapons/rarities.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md); reference mockup Art/Source/card-mockup/epic-card-mockup.png and epic-card-spec.png (sizes, fonts, colours at 1080p; page epic-card.html?spec)
- expected_output: WBP_UpgradeChoice: cards about 380 x 560 at 1080p with 40 px gap, title kept, reroll centred below the row; per card top to bottom: image area (about 45 % of the card), effect line large (ValueText + stat name, e.g. "+15% Reload speed"), target line small ("All weapons", the weapon name, nothing for player stats), lore line in italic; frame = three overlaid 9-slice brushes of the same margins: T_CardFrame_Stat untinted, T_CardFrame_Stat_Accents and T_CardFrame_Stat_Glow tinted by the existing tier colours (owner A+C mix, 2026-10-14); no rarity label; icons and frame imported with the UI texture group; existing widget names kept where possible; wbp-upgrade-choice doc updated. Weapon cards keep their current look (task-169). Lore line always anchored at the bottom of the card content box, whatever the text above (owner 2026-10-14, mockup validated otherwise); the free space goes between the target or explanation and the lore.
- done_when: Compiles with warnings as errors, saved, tree and graphs read back, doc updated; committed. Not seen on screen: say so.
- editor_state: open
- token_ceiling: 40 turns
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
