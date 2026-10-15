---
id: task-168-upgrade-card-design
description: "Global task: large stat upgrade cards with image, effect, pure lore line and rarity-tinted frame, reroll below."
parent: tasks-done-index
kind: task
status: done
---
# Upgrade cards: new design

- request: On va retravailler le design des cartes: l'image, ce que ça fait, la phrase lore en dessous, la bordure de la couleur de la qualité; les trois cartes prennent une bonne partie de l'écran, le bouton reroll en dessous; icone de reload seule pour toutes les armes, à droite de l'icone de l'arme pour une arme spécifique. Defaults accepted by the owner: cards about 380 x 560 px at 1080p, 40 px gap, title kept, reroll centred below; dark panel, rarity-tinted frame (style chosen among 3 art proposals), no rarity label; order image, effect, target, lore; 12 white flat stat icons; lore rewritten as pure lore, one line per stat (owner, 2026-10-14)
- goal: Redesign the stat upgrade cards: large cards with an image (stat icon, weapon icon plus stat icon when targeted), the effect, a pure lore line and a rarity-tinted frame, reroll below.
- acceptance: Stat cards about 380 x 560 at 1080p, 40 px apart, reroll centred below; each card shows its image (stat icon alone for all weapons or a player stat, weapon icon with the stat icon on its right for one weapon), the effect, the target and a pure lore line, inside the owner-chosen frame tinted by rarity; every subtask done_when passed; docs updated; lint clean; committed. Visual result not seen by the crew.
- gdd_check: upgrades, rarities, sys-upgrades, wbp-upgrade-choice, readability-rules, sys-text-content read (2026-10-14). No conflict. Notes: flavor lines described the effect (rewritten as pure lore); UI text stays English; 12 offered stats (no magazine size in M1). Owner defaults accepted 2026-10-14; frame style chosen among 3 art proposals (owner).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: none
- result: Done. WBP_AlertObjectives binds OnUpgradeOfferShown/Closed in BindToMiner (UnbindOffer on the previous miner): HideForOffer collapses AlertBar, AlertLabel and the 7 alert icons; RestoreAfterOffer shows them again (Visible, their only designed state); OnAlertBarEventsChanged skips the icons while bOfferShown. No tick. Compiled with warnings as errors, saved, graph read back, doc updated. Left: about 45 empty unconnected custom event nodes from failed DSL writes (delete_node refused by the permission classifier; harmless, compile clean). Not seen on screen. Commits 89895fb, d5418fd.

## Subtasks
1. task-168-01-card-lore-lines — text
2. task-168-02-stat-icons — art
3. task-168-03-card-frames — art
4. task-168-04-card-icon-data — cpp
5. task-168-05-upgrade-choice-layout — bp
6. task-168-06-card-layout-fix — bp
7. task-168-07-hide-alert-bar — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-168-01-card-lore-lines]] — step 1, text.
- [[task-168-02-stat-icons]] — step 2, art.
- [[task-168-03-card-frames]] — step 3, art.
- [[task-168-04-card-icon-data]] — step 4, cpp.
- [[task-168-05-upgrade-choice-layout]] — step 5, bp.
- [[task-168-06-card-layout-fix]] — step 6, bp (owner test fixes).
- [[task-168-07-hide-alert-bar]] — step 7, bp.
