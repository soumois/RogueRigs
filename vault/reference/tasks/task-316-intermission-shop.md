---
id: task-316-intermission-shop
description: "Global task: The intermission screen sells 5 upgrade cards priced by rarity, with an escalating reroll and the Continue button."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Upgrade shop between levels

- request: "Entre les niveaux, il faut un écran qui propose des améliorations (par 5, il faudra revoir la taille des cartes, mais tu as accès à tout l'écran). Le reroll qui sera présent fonctionne comme la montée de niveau et un bouton pour continuer la traversée. Coûts : 25 blanche, 40 verte, 60 bleue, 80 épique, 100 légendaire" (owner, 2026-10-26)
- goal: The intermission screen sells 5 upgrade cards priced by rarity, with an escalating reroll and the Continue button.
- acceptance: the intermission shows the recap at the top, 5 smaller stat cards in one row with their price, the ore balance, a Reroll button with its cost and Continue; prices grey 25, green 40, blue 60, purple 80, yellow 100; buying spends secured ore, applies the card, carries it into the next level and leaves its slot empty; unaffordable cards and reroll greyed; shop reroll redraws the 5 slots at 15 then +5, reset at each intermission; -game capture of the intermission sent to the owner; docs, run-shop and intermission updated; lint clean; committed.
- gdd_check: run-shop, intermission, ore-economy, rarities, xp-level-up, run-structure read (2026-10-26). run-shop catalogue, prices and reroll cost were open: decided by this request. Rarity mapping (Q5): blanche = grey, verte = green, bleue = blue, épique = purple, légendaire = yellow. Owner defaults accepted 2026-10-26: Q6 cards drawn as a level-up offer with owner luck, stat cards only; Q7 several buys, a bought card leaves its slot empty, reroll redraws the 5 slots; Q8 own reroll count, back to 15 at each intermission; Q9 level-up screen unchanged (3 cards), shop cards smaller on one row, same card look without deal, suspense or fly animations, recap kept at the top, ore balance shown. Depends on task-315 (shared reroll cost rule).
- confirmed_by_owner: yes (owner 2026-10-26: "ok" to questions 1-9, every default accepted; plan task-315 then task-316 confirmed)
- milestone: M2
- next_step: task-316-04-bp-intermission-shop
- result: open

## Subtasks
1. task-316-01-intermission-shop — cpp: shop offer, prices, buy, reroll, Blueprint API
2. task-316-02-text-shop-labels — text: shop labels in screen_texts.json
3. task-316-03-bp-shop-card — bp: new WBP_ShopCard (one smaller priced card)
4. task-316-04-bp-intermission-shop — bp: WBP_Intermission hosts 5 shop cards, balance, reroll, Continue
5. task-316-05-capture-shop — cpp: capture the intermission in game

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-316-01-intermission-shop]] — step 01, cpp.
- [[task-316-02-text-shop-labels]] — step 02, text.
- [[task-316-03-bp-shop-card]] — step 03, bp.
- [[task-316-04-bp-intermission-shop]] — step 04, bp.
- [[task-316-05-capture-shop]] — step 05, cpp.
