---
id: task-490-heal-pickup-30hp
description: "Global task: HealValue of the heal pickup (one per broken heal-ore block) set to 30 HP."
parent: tasks-to-do-index
kind: task
status: todo
---
# Each heal pickup restores 30 HP

- request: Owner, verbatim (French), 2026-11-06: "- faire passer les soins ( chaque petit bloc) à 30 PV récupérer. Augmenter de 20 pourcent le nombre de barils et de soins dans les règles. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Ok, all default" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- commencer à développer le c++ et préparer une map test où je pourrais faire des générations à la volée pour tester moi même le résultat. Il faudra aussi, faire des tests avec l'agent level designer qui valide le résultat. Donc commence juste par le script qui génère le json, valide cette partie la avec l'agent designer. Puis fait un script qui load les assets par rapport au json. Load de manière asynchrone et prépare une variable qui indiquera un pourcentage des objets loader ( ça servira pour l écran de loading plus tard).

Et enfin prépare le niveau test dans ennemi avec une commande pour reload un nouveau level". The executor has standing permission to open and close Unreal and save. This global covers: "faire passer les soins ( chaque petit bloc) à 30 PV récupérer".
- goal: Every heal pickup dropped by a broken heal-ore block restores 30 HP to the miner; nothing else about heal ore changes.
- acceptance: A1 `OreTypes.csv` row `Heal`, column `HealValue` reads 30, read back from the data the game loads. A2 In a game run, collecting one heal pickup below max health raises the miner health by 30 (capped at max), seen in the log or HUD value. A3 Gameplay otherwise unchanged: pickup not taken at full health, drop rules and dig time unchanged; any existing test whose expectation used the old value is updated, no new test.
- gdd_check: Read: heal-ore, arena-grid. No conflict: heal-ore names HealValue as the per-pickup heal; the sentence "a whole vein restores as before" is replaced by 30 HP per pickup, 120 HP per 2 x 2 vein (edit applied at confirmation). No open value. Milestone M2 (heal ore provisional in M2). Verification light: one data value on an existing system.
- verification_level: light
- confirmed_by_owner: yes (2026-11-07, "Ok, all default")
- milestone: M2
- next_step: task-490-01-heal-value
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
1. task-490-01-heal-value (vault/reference/tasks/task-490-heal-pickup-30hp/task-490-01-heal-value.md) — cpp: Set HealValue of the Heal row to 30 and check one pickup heals 30 HP in game.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-490-01-heal-value]] — step 1, cpp.
