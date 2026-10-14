---
id: task-168-05-upgrade-choice-layout
description: "Rebuild the stat cards of WBP_UpgradeChoice: large cards, icons, effect, target, lore, rarity-tinted accents and inner glow, reroll below."
parent: task-168-upgrade-card-design
kind: task
status: done
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
- model: sonnet
- tokens: input 132, cache write 163746, cache read 6314526, output 1175
- runs: 3
- milestone: M2
- result: WBP_UpgradeChoice stat cards rebuilt from the epic mockup: 392 x 572 CardSize (380 x 560 card), 40 px gap, reroll centred below; frame = FrameBase + FrameAccents + FrameGlow Box brushes (accents and glow tinted by tier); image area 308 x 252 with weapon icon (keeps aspect) + stat icon for one weapon, stat icon alone otherwise (URRIconLibrary); effect (tier colour value + white stat name), target, lore from LoreText anchored at the bottom. 12 stat icons and 4 frame textures imported. Widget names kept. Compiles with warnings as errors, saved, doc updated. Not seen on screen: Box margins drawn size, hover feedback removed (button states NoDrawType), runtime clicks. Commits de6b06c, 03e47dc, 04a28f5, a5792e8.

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
