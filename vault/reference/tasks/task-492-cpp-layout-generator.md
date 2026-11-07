---
id: task-492-cpp-layout-generator
description: "Global task: deterministic C++ port of the 240 x 240 generator recipe that writes a layout JSON, then a 20-seed validation by the Level-Design agent."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# C++ layout generator writing the grid JSON, validated by the Level-Design agent

- request: Owner, verbatim (French), 2026-11-06: "- faire passer les soins ( chaque petit bloc) à 30 PV récupérer. Augmenter de 20 pourcent le nombre de barils et de soins dans les règles. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Ok, all default" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- commencer à développer le c++ et préparer une map test où je pourrais faire des générations à la volée pour tester moi même le résultat. Il faudra aussi, faire des tests avec l'agent level designer qui valide le résultat. Donc commence juste par le script qui génère le json, valide cette partie la avec l'agent designer. Puis fait un script qui load les assets par rapport au json. Load de manière asynchrone et prépare une variable qui indiquera un pourcentage des objets loader ( ça servira pour l écran de loading plus tard).

Et enfin prépare le niveau test dans ennemi avec une commande pour reload un nouveau level". The executor has standing permission to open and close Unreal and save. This global covers: "commencer à développer le c++ ... Donc commence juste par le script qui génère le json, valide cette partie la avec l'agent designer" and "faire des tests avec l'agent level designer qui valide le résultat".
- goal: A C++ generator produces, from a seed and data parameters, a 240 x 240 layout JSON in the level-layout-schema format that follows the generator recipe, and the Level-Design agent has validated its output.
- acceptance: A1 The C++ generator follows every step of layout-generator-recipe "Direct 240 generation" (outline, spawn clearing, main network, hidden pockets, far-cell filler, cleanup, 3-cell passage repair, heal veins and barrels with the task-491 counts, recipe seed acceptance), all parameters as data; FRandomStream; same params and seed give a byte-identical JSON (identity with the Node output is not required). A2 It can be run outside a play session for a given seed, params and optional brief, and writes `<name>.json` in the schema format (and `<name>.brief.json` when a brief is given); generation time per layout is printed. A3 Every layout it writes passes `node Tools/validate_layout.js` with a 240 x 240 brief using the task-491 bounds (minimum passage 3). A4 The Level-Design agent generates 20 passing seeds with it, applies layout-review-rules (24 x 24 window scan, dead zones, too-open windows, straight lanes, pocket distance) and gives a verdict per seed; at least one seed keeps every window in 0.05 to 0.95, and the agent states whether the set is comparable to the Node generator. A defect it finds in the generator is fixed before the global closes; a remaining deviation is listed for the owner. A5 The C++ project builds; existing tests still pass.
- gdd_check: Read: arena-grid (M2 uses a deterministic C++ generator, no runtime AI), layout-generator-recipe ("For the C++ port": FRandomStream, parameters as data), layout-review-rules (not automated; kept manual per Q3 default), level-layout-schema, agent-level-design, milestones (grid generator is M2: in scope). No conflict. No open value. Depends on task-491 (counts).
- verification_level: standard
- confirmed_by_owner: yes (2026-11-07, "Ok, all default")
- milestone: M2
- next_step: task-492-04-doc
- started: 2026-11-07T06:14:54Z
- waiting_for: none
- executor_tokens: input 94, cache write 165133, cache read 5427127, output 84311
- executor_usage: runs 1, turns 47, context start 17k peak 177k, resumes 0, 30 min, units 1.17 M
- planning_tokens: input 6, cache write 13690, cache read 111230, output 4016
- planning_usage: a8f799ff "Plan heals, barrels, procgen test map" 1/5 (tasks 490, 491, 492, 493, 494), units 0.05 M
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
1. task-492-01-gen-carve (vault/reference/tasks/task-492-cpp-layout-generator/task-492-01-gen-carve.md) — cpp: C++ generator, first slice: outline, spawn clearing, main network, hidden pockets, filler, cleanup and 3-cell passage repair on 240 x 240 from data params.
2. task-492-02-gen-place-json (vault/reference/tasks/task-492-cpp-layout-generator/task-492-02-gen-place-json.md) — cpp: C++ generator, second slice: heal veins, barrels, spawn, recipe seed acceptance, JSON writer and an entry point usable outside a play session.
3. task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md) — level-design: Level-Design agent generates 20 passing seeds with the C++ generator and reviews them with layout-review-rules.
4. task-492-04-doc (vault/reference/tasks/task-492-cpp-layout-generator/task-492-04-doc.md) — doc: Document the C++ generator and its entry point.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-492-01-gen-carve]] — step 1, cpp.
- [[task-492-02-gen-place-json]] — step 2, cpp.
- [[task-492-03-ld-validation]] — step 3, level-design.
- [[task-492-04-doc]] — step 4, doc.
