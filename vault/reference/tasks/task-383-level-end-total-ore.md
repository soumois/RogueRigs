---
id: task-383-level-end-total-ore
description: "Global task: the stats screen shows one Total Ore line instead of two ore lines, in a thinner band giving the columns more room."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Level-end stats screen: one Total Ore line

- request: Owner, 2026-10-30, verbatim (relayed by the main session, after task-381 was done with its Q1 default, two stacked ore lines): "Il n'y a pas de ore secured this level ou this run, tu as juste le ore total, d'ailleurs sur la maquette que tu as reçu, il n'y ait pas et ça laisse un peu plus de place pour les robots destroyed et damage by weapon". Reading (main session and Planner): the owner takes the other choice of task-381 Q1: one line "Total Ore" as the mockup `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp`, no more "Ore secured this level" nor "Ore secured this run", and a thinner ore band so the columns Robots destroyed / Damage by weapon get more room; the GDD intermission is changed. What "Total Ore" counts is asked in Q1 (the GDD run total, RunSecuredOre, is the default). One logical point, one global. The level 4 subtitle crossing the thin lines is a separate point: task-384. Not in scope: C++ unless Q1 takes the balance, the shop screen, other texts. Owner answer 2026-10-30 (relayed by the main session, verbatim): "Ok, par défaut" = the plan and the defaults of Q1, Q2, Q3, Q4 accepted.
- goal: After an extraction, the ore band of WBP_Intermission shows the ore icon, "Total Ore:" and the run total as one line in a thinner band; the dividers, column labels, lines and lists move up so each column holds about two more rows; the gdd intermission and screen_texts.json say one Total Ore line.
- acceptance: screen_texts.json item intermission_recap: lines hold the single Total Ore label (Q2), the two old ore lines gone, labels indices unchanged, validator passes; WBP_Intermission: OreLevelText and OreRunText replaced by the Total Ore label and value (Q2), value = the recap RunSecuredOre (Q1), band thinner and DividerH, DividerV top, KillsLabel, DamageLabel, KillsLine, DamageLine, KillsRows, DamageRows tops moved up by the same amount, list bottoms unchanged (Q3); graphs updated, nothing else in the widget changed; compiled with warnings as errors, saved; no C++ (Q1 default); capture checklist C1 to C6 seen (Q4); wbp-intermission doc updated; gdd intermission updated; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[intermission]] (provisional: stats screen "ore secured in this level and run total"; Look sentence from task-381 Q1 "top band holds both ore lines"): conflict with the owner words, changed by this task (edits applied on confirmation). [[wbp-intermission]] (task-381 tree: OreIcon (0.40,0.3385) 88 x 88, OreTexts VerticalBox of OreLevelText + OreRunText from Lines[0..1] with {ORE}; DividerH (0.2273,0.3866)-(0.7727,0.4466); labels 0.481; lines 0.5207; rows 0.545-0.765; DividerV 0.425-0.775). screen_texts.json intermission_recap: lines ["Ore secured this level: {ORE}", "Ore secured this run: {ORE}"], read only by WBP_Intermission (no C++ or test reads it); there is no "Total Ore" string yet. "Total Ore" in the code: FRRIntermissionRecap has OreSecuredThisLevel and RunSecuredOre; RunSecuredOre = bank minus the bank at run start, with the baseline lowered on each purchase so spending does not reduce it (RRRunSubsystem.h line 156): the ore secured since the run start; the bank balance (what the shop can spend) is not in the recap. Both are equal on level 1 and differ once the owner has bought cards: which one "Total Ore" means is not in the GDD, asked (Q1), default RunSecuredOre, the GDD "run total". Open values: none other. Repo state 2026-10-30: WBP_Intermission last committed by task-381 (892ae8c1), no uncommitted asset; task-379 done; task-382 (miner FBX, art, no editor asset in common) todo. Scope M2.
- confirmed_by_owner: yes (2026-10-30, "Ok, par défaut")
- milestone: M2
- next_step: task-383-01-total-ore-text
- started: 2026-10-30T05:00:22Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "Ok, par défaut"): every default below is accepted; values provisional, owner 2026-10-30.
- Q1 Ce que compte « Total Ore » : le minerai sécurisé depuis le début du run (le « total du run » actuel du GDD ; les achats au Spare Parts ne le font pas baisser ; aucun C++). Autre choix : le solde actuel, ce qui reste à dépenser (le chiffre du compteur de Spare Parts), qui baisse après les achats : une petite sous-tâche C++ en plus pour le mettre dans le récapitulatif. Les deux sont identiques au niveau 1. Default: minerai sécurisé depuis le début du run.
- Q2 Texte et rendu, comme la maquette : l'icône de minerai, « Total Ore: » en taille 32 et le nombre à côté en plus gros (48), sur la même ligne ; dans screen_texts.json, les deux lignes « Ore secured ... » sont remplacées par le seul libellé « Total Ore: » (écrit par l'agent text). Autre choix : un seul texte « Total Ore: 61 » en taille 32. Default: comme la maquette.
- Q3 Bandeau plus fin : le séparateur horizontal remonte d'environ 60 px en 1920 x 1080 (son centre passe de 41,7 % à 36 % de la hauteur), titres des colonnes, filets, listes et séparateur vertical remontent d'autant, le bas des listes ne bouge pas : environ deux lignes de plus par colonne. Default: oui.
- Q4 Vérification : capture -game niveau 1 avec minerai et robots détruits aux quatre résolutions (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080 ; ça refait aussi les deux résolutions restées non recapturées après le correctif du bouton de task-381) et le niveau 4 en 1920 x 1080. Default: oui.

## Subtasks
1. task-383-01-total-ore-text (vault/reference/tasks/task-383-level-end-total-ore/task-383-01-total-ore-text.md) — text: screen_texts.json intermission_recap: the two ore lines replaced by the single Total Ore label
2. task-383-02-total-ore-widget (vault/reference/tasks/task-383-level-end-total-ore/task-383-02-total-ore-widget.md) — bp: WBP_Intermission: one Total Ore label and value in a thinner band, dividers, labels, lines and lists moved up
3. task-383-03-total-ore-capture (vault/reference/tasks/task-383-level-end-total-ore/task-383-03-total-ore-capture.md) — capture: -game captures of the stats screen with the Total Ore line at four resolutions plus level 4
4. task-383-04-doc-total-ore (vault/reference/tasks/task-383-level-end-total-ore/task-383-04-doc-total-ore.md) — doc: update the wbp-intermission doc: Total Ore line, moved anchors, graph, capture result

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-383-01-total-ore-text]] — step 1, text.
- [[task-383-02-total-ore-widget]] — step 2, bp.
- [[task-383-03-total-ore-capture]] — step 3, capture.
- [[task-383-04-doc-total-ore]] — step 4, doc.
- [[intermission]] — the stats screen spec, changed to one Total Ore line.
- [[wbp-intermission]] — the widget changed here.
- [[task-381-level-end-screen-rebuild]] — previous task: the rebuild with two ore lines (its Q1).
- [[task-384-level-end-subtitle-lines]] — next task on the same widget: the subtitle lines.
