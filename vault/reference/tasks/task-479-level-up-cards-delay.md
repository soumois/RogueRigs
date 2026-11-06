---
id: task-479-level-up-cards-delay
description: "Global task: The upgrade card screen opens about 1.2 s after a level-up instead of 1.8 s; the rest of the sequence is unchanged."
parent: tasks-done-index
kind: task
status: done
---
# Level-up cards after 1.2 s instead of 1.8 s

- request: Owner 2026-11-06 (verbatim), two points after tasks 476-478 closed (478 result not explicitly validated; point 2 supersedes it for the walls). Point 1: "fait passé le temps avant de voir les cartes à 1.2 sec au lieu de 1.8 sec". Point 2 (pasted spec): """Amélioration du rendu des socles rocheux. Owner answer 2026-11-06 (relayed by the main session, verbatim): "Je valide, mais ne lance pas les tâches ( prépare les je vais passer sur un autre compte )" = the plan and the defaults of Q1 accepted.
Le rendu actuel des socles ne correspond pas encore à la direction artistique recherchée.
La forme générale est correcte, mais deux problèmes doivent être corrigés.
A. Bord lumineux orange
Le contour orange actuel ressemble à un trait lumineux posé sur le mesh.
Je souhaite un bord lumineux intégré à la géométrie :
* Créer un léger chanfrein sur l'arête supérieure du socle.
* Intégrer une bande émissive orange-jaune fine sur ce chanfrein.
* Ajouter une diffusion lumineuse subtile sur la surface supérieure, près du bord.
* Introduire de légères variations d'intensité.
* Éviter un contour trop épais, parfaitement régulier ou ressemblant à un néon.
Le résultat doit donner l'impression que la roche possède naturellement une bordure minérale lumineuse.
Ne pas utiliser un simple post-process outline pour produire cet effet.
B. Géométrie des flancs
Le flanc actuel est beaucoup trop lisse et cylindrique.
Je souhaite une apparence rocheuse stylisée low-poly :
* Grandes facettes verticales clairement visibles.
* Plans légèrement inclinés et changements subtils de profondeur.
* Arêtes verticales irrégulières.
* Variations de teintes entre les facettes : violet sombre, violet moyen et quelques zones légèrement plus claires.
* Alternance de zones éclairées et ombragées.
* Quelques reliefs verticaux subtils.
Attention :
* Ne pas ajouter de fissures.
* Ne pas donner l'impression d'une roche détruite ou abîmée.
* Ne pas créer de petits détails bruités partout.
* Ne pas rendre les facettes trop géométriques ou répétitives.
C. Cohérence avec le jeu
Conserver :
* La texture supérieure actuelle.
* La silhouette organique des socles.
* Le violet comme couleur dominante.
* Le contour lumineux orange.
* Les proportions et hauteurs actuelles.
Le dessus doit rester une surface continue, sans lignes visibles entre les dalles.
D. Avant implémentation
Inspecter le mesh et les matériaux actuellement utilisés.
Proposer la solution la plus simple pour obtenir :
1. Un chanfrein supérieur avec matériau émissif.
2. Des flancs avec une géométrie facettée et des variations de matériau.
Ne pas remplacer le système de génération des socles sans nécessité.
Présenter d'abord une proposition technique avant de modifier les ressources existantes.""". Attached image (current state after task-478) kept as the before reference: Art/Source/cartoon-look/reference/owner-2026-11-06-wall-after-478.png (top-down game view: lilac textured slab tops with pink crystals, thin bright orange outline along the top edges, smooth dark vertical-streaked sides). One owner point = one global task: task-479 card delay, task-480 rock slabs. This task: point 1, "fait passé le temps avant de voir les cartes à 1.2 sec au lieu de 1.8 sec".
- goal: After a level-up the game pauses and shows the 3 upgrade cards about 1.2 s later instead of 1.8 s.
- acceptance: A1 the delay between a level-up and the card screen opening is about 1.2 s (measured in a test or PIE, figure in the result); A2 the game keeps running during the delay; several levels gained at once still wait once and then chain their screens; A3 the LEVEL UP text, the gold wave, the heal and the card animation are unchanged; A4 bonus chest choice screens unchanged; A5 build ok, the LevelUp tests pass; tests whose frame budget the shorter delay breaks are updated by budget only; A6 the report gives the token cost per subtask and in total.
- gdd_check: Read xp-level-up (Level-up sequence: about 1.8 s, owner 2026-11-05, task-459; was 0.8 s). Conflict: the value itself, replaced by 1.2 s via the edit on confirmation. The LEVEL UP text lasts about 0.7 s, so it has ended before the cards at 1.2 s. Targeted read of task-459-01 result (why: to name the role): the delay is the C++ default ChoiceDelay of the level-up component, BP_Miner does not override it, so cpp. No open value. Milestone as task-459.
- confirmed_by_owner: yes (2026-11-06, "Je valide, mais ne lance pas les tâches ( prépare les je vais passer sur un autre compte )")
- milestone: M2
- next_step: none
- started: 2026-11-06T20:48:00Z
- waiting_for: none
- executor_tokens: input 26, cache write 88464, cache read 658302, output 10827
- executor_usage: runs 1, turns 12, context start 17k peak 88k, resumes 0, 6 min, units 0.23 M
- planning_tokens: input 15, cache write 35116, cache read 306038, output 10554
- planning_usage: aeff9c58 "Plan card delay and rock slab look" 1/2 (tasks 479, 480), units 0.13 M
- result: Done. The 3-card screen opens 1.2 s after a level-up (ChoiceDelay 1.8 -> 1.2, f72f7825), measured 72 frames = 1.200 s with BP_Miner and the real game mode (Integration.LevelUpUpgradeLoop), game running during the wait; two levels at once wait once then chain (LevelUp.ChainedOffers); burst, push, heal, chest screen code unchanged; 18 tests pass; sys-level-up updated (5ee49665). Cheats.LightHalo fails on a point light count on the miner, unrelated to the delay, not touched. Feel not seen in game by the Executor. Cost: 0.23 M units in the executor run, per subtask included in it.

## Questions
Decided 2026-11-06 (owner "Je valide, mais ne lance pas les tâches ( prépare les je vais passer sur un autre compte )"): every default below is accepted; values provisional, owner 2026-11-06.
- Q1 Délai des cartes : Default: les cartes s'affichent 1,2 s après le passage de niveau (au lieu de 1,8 s) ; le jeu continue pendant ce temps, l'effet LEVEL UP (0,7 s) a fini avant ; rien d'autre ne change, aucun nouveau test, seuls les tests cassés par le délai sont ajustés.

## Subtasks
1. task-479-01-delay (vault/reference/tasks/task-479-level-up-cards-delay/task-479-01-delay.md) — cpp: The card screen opens about 1.2 s after a level-up; nothing else in the sequence changes.
2. task-479-02-doc (vault/reference/tasks/task-479-level-up-cards-delay/task-479-02-doc.md) — doc: The level-up system doc gives the new delay.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-479-01-delay]] — step 1, cpp.
- [[task-479-02-doc]] — step 2, doc.
