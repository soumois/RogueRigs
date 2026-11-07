---
id: task-491-generator-heal-barrel-plus20
description: "Global task: generator parameters give 14 heal veins (56 cells) and 17 to 22 barrels per 240 x 240 layout; committed layouts unchanged."
parent: tasks-to-do-index
kind: task
status: todo
---
# Generator rules: 20 % more heal veins and barrels

- request: Owner, verbatim (French), 2026-11-06: "- faire passer les soins ( chaque petit bloc) à 30 PV récupérer. Augmenter de 20 pourcent le nombre de barils et de soins dans les règles. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Ok, all default" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- commencer à développer le c++ et préparer une map test où je pourrais faire des générations à la volée pour tester moi même le résultat. Il faudra aussi, faire des tests avec l'agent level designer qui valide le résultat. Donc commence juste par le script qui génère le json, valide cette partie la avec l'agent designer. Puis fait un script qui load les assets par rapport au json. Load de manière asynchrone et prépare une variable qui indiquera un pourcentage des objets loader ( ça servira pour l écran de loading plus tard).

Et enfin prépare le niveau test dans ennemi avec une commande pour reload un nouveau level". The executor has standing permission to open and close Unreal and save. This global covers: "Augmenter de 20 pourcent le nombre de barils et de soins dans les règles".
- goal: Layouts generated from now on hold 20 % more heal veins and barrels than the current rules; the layouts already committed are not regenerated.
- acceptance: A1 The Node generator (`Tools/layout_gen/arena_gen_fine.js` with its default params) produces 14 heal veins of 2 x 2 (56 heal_ore cells: 5 in hidden pockets, 9 in walls) and 17 to 22 barrels per layout, with pairs kept for chains; the counts are parameters, not hard-coded. A2 Two test seeds generated with the default params and a brief whose heal_ore bound is [56, 56] pass `node Tools/validate_layout.js` (exit 0, minimum passage 3). A3 Committed layouts in `Content/RogueRigs/Data/Levels/` and their briefs are unchanged. A4 The gdd heal-ore and barrel nodes and the generator recipe node state the new counts.
- gdd_check: Read: heal-ore (12 veins of 2 x 2, 48 cells), barrel (14 to 18 drawn per seed in layout-review-rules; the 2026-10-13 rule was GDD-only too), arena-grid, layout-generator-recipe, layout-review-rules, level-layout-schema. "Dans les règles" read as rules only (Q2), like the 2026-10-13 barrel rule. 12 x 1.2 = 14.4 rounded to 14 veins; 14-18 x 1.2 = 16.8-21.6 rounded to 17-22. No open value. Milestone M2 (grid generator). Gdd edits applied at confirmation. Must come before task-492, whose C++ port reads these params.
- verification_level: light
- confirmed_by_owner: yes (2026-11-07, "Ok, all default")
- milestone: M2
- next_step: task-491-01-gen-params
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-07 (owner "Ok, all default"): every default below is accepted; values provisional, owner 2026-11-07.
- Q1 (task-490) "chaque petit bloc" = chaque bloc de soin cassé = un pickup, qui rend 30 PV (`HealValue` de `OreTypes.csv`, ligne `Heal`). Une veine 2 × 2 rend donc 120 PV. Défaut: oui, 30 PV par pickup.
- Q2 (task-491) +20 % "dans les règles": règles du GDD et paramètres du générateur (script Node actuel et futur générateur C++), sans régénérer les maps déjà commitées (arena_m1_fine, arena_l2 à l4 gardent leurs comptes). Soins: 12 veines 2 × 2 -> 14 veines (56 cellules: 5 en poches cachées, 9 dans les murs). Barils: 14 à 18 par seed -> 17 à 22 par seed, toujours en paires pour les chaînes. Défaut: ces valeurs, règles seulement.
- Q3 (task-492) Générateur C++: portage de la recette de arena_gen_fine.js (240 × 240, paramètres en données, FRandomStream: mêmes paramètres + même seed = même JSON, mais pas identique octet par octet au script Node). Il écrit le JSON au format level-layout-schema et peut être lancé hors du jeu pour une seed donnée. Il garde seulement les contrôles de la recette pour accepter une seed; les règles de review (zones mortes, zones trop ouvertes, longues lignes droites) restent vérifiées par l'agent level designer sur un lot de 20 seeds, pas automatisées. Défaut: oui.
- Q4 (task-493) Chargement asynchrone: le pourcentage (0 à 100) couvre tout, le chargement des assets (meshes, matériaux, classes) puis la construction des blocs de la grille répartie sur plusieurs frames, et il est lisible par un futur écran de chargement. Les maps de jeu actuelles gardent leur chargement actuel pour l'instant; seule la map test utilise le nouveau chargeur. Défaut: oui.
- Q5 (task-494) "dans ennemi" = "sans ennemi": map test avec le mineur et le drone jouables, terrain généré, barils et soins, mais aucun robot, pas d'objectif (console, coffre), pas de navette ni de barre d'alerte. Commande console `rr.Level.New [seed]`: génère un nouveau niveau (seed aléatoire si omise), écrit le JSON dans `Saved/GeneratedLevels/`, le recharge avec le chargeur asynchrone; la seed est affichée à l'écran et dans le log. Pas d'affichage du pourcentage dans cette map (log seulement). Défaut: oui.
- Q6 Niveaux de vérification: task-490 leger, task-491 leger, task-492 standard, task-493 standard, task-494 standard. Défaut: ceux-ci.

## Subtasks
1. task-491-01-gen-params (vault/reference/tasks/task-491-generator-heal-barrel-plus20/task-491-01-gen-params.md) — cpp: Raise heal vein and barrel counts in the Node generator params by 20 % and check two seeds with the validator.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-491-01-gen-params]] — step 1, cpp.
