---
id: task-494-generator-test-map
description: "Global task: a test map without robots where the owner generates a new layout on the fly with a console command and plays it."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Enemy-free test map with a command to generate and reload a new level

- request: Owner, verbatim (French), 2026-11-06: "- faire passer les soins ( chaque petit bloc) à 30 PV récupérer. Augmenter de 20 pourcent le nombre de barils et de soins dans les règles. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Ok, all default" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- commencer à développer le c++ et préparer une map test où je pourrais faire des générations à la volée pour tester moi même le résultat. Il faudra aussi, faire des tests avec l'agent level designer qui valide le résultat. Donc commence juste par le script qui génère le json, valide cette partie la avec l'agent designer. Puis fait un script qui load les assets par rapport au json. Load de manière asynchrone et prépare une variable qui indiquera un pourcentage des objets loader ( ça servira pour l écran de loading plus tard).

Et enfin prépare le niveau test dans ennemi avec une commande pour reload un nouveau level". The executor has standing permission to open and close Unreal and save. This global covers: "préparer une map test où je pourrais faire des générations à la volée pour tester moi même le résultat" and "Et enfin prépare le niveau test dans ennemi avec une commande pour reload un nouveau level" ("dans ennemi" read as "sans ennemi", Q5).
- goal: The owner opens a test map, plays the miner and drone on a generated layout with no robots, and types a command to generate and load a new layout as often as he wants.
- acceptance: A1 A test map starts on a generated 240 x 240 layout built with the task-493 loader; miner and drone playable (move, aim, dig, barrels explode, heal pickups heal). A2 No robot ever spawns, no console, bonus chest, shuttle or alert bar, during at least 2 minutes of play. A3 `rr.Level.New [seed]` generates a new layout with the task-492 generator (random seed when omitted, the given seed otherwise), writes its JSON to `Saved/GeneratedLevels/`, reloads the level with the async loader and puts the miner at the new spawn; the seed is shown on screen and logged. The same seed twice gives the same layout. A4 Several reloads in a row work without crash or leftover blocks from the previous layout. A5 Capture checklist passes. A6 Normal game maps unchanged.
- gdd_check: Read: arena-grid, level-layout-schema, agent-level-design; gloss only for level-flow (robots, consoles, chests, shuttle placed during play: all off in this map). "dans ennemi" taken as "sans ennemi" (Q5). Not a gdd feature: a dev test map; no gdd edit. No open value. Milestone M2. Depends on task-492, task-493 and task-495 (generator review rules enforced first, owner 2026-11-07).
- verification_level: standard
- confirmed_by_owner: yes (2026-11-07, "Ok, all default")
- milestone: M2
- next_step: task-494-01-test-mode-command
- started: 2026-11-07T08:11:11Z
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
1. task-494-01-test-mode-command (vault/reference/tasks/task-494-generator-test-map/task-494-01-test-mode-command.md) — cpp: Enemy-free test mode (no robot spawn, objectives, shuttle or alert) and the rr.Level.New [seed] command that generates, saves and reloads a layout through the async loader.
2. task-494-02-test-map (vault/reference/tasks/task-494-generator-test-map/task-494-02-test-map.md) — bp: The test map asset using the test mode, starting on a generated layout.
3. task-494-03-capture (vault/reference/tasks/task-494-generator-test-map/task-494-03-capture.md) — capture: Visual check of the test map and of one reload.
4. task-494-04-doc (vault/reference/tasks/task-494-generator-test-map/task-494-04-doc.md) — doc: Document the test mode, the command and the test map.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-494-01-test-mode-command]] — step 1, cpp.
- [[task-494-02-test-map]] — step 2, bp.
- [[task-494-03-capture]] — step 3, capture.
- [[task-494-04-doc]] — step 4, doc.
