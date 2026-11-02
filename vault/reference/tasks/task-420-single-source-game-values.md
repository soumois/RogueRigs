---
id: task-420-single-source-game-values
description: "Global task: Make the stat CSV tables the only home of a game value: tests read them, gdd nodes cite rows, not numbers."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Game values in one place: the CSV tables

- request: Owner 2026-11-02, verbatim: "Ok, j'aimerais bien que les données (comme les dégâts d'une arme par exemple) ne soient qu'à un seul endroit. Que si je les modifie manuellement, ça ne crée pas de problème pour les tests et une discordance avec le vault gdd.". Owner answer 2026-11-02 (relayed by the main session, verbatim): "Tout par défaut" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- goal: A stat value (example: shotgun damage per pellet) is edited in one place, its CSV row in `Content/RogueRigs/Data/Tables/` (WeaponStats, EnemyStats, PlayerBaseStats, ExplosionStats, UpgradeValues, UpgradeRarities, XPCurve, OreTypes .csv), read by the game at runtime (RRBaseStatsLoader). Changing it by hand breaks no test (tests read the CSV) and creates no gdd discordance (gdd nodes cite the row, not the number).
- acceptance: A1: no test under `Source/RogueRigs/Tests/` holds a literal copy of a CSV value; expected values are read from the CSV row (helper over `URRBaseStatsLoader::GetCachedRow`/`GetCachedTable`) and derived ones computed from them (crit = Damage x CritDamage, 3 pellets = 3 x Damage, shots in 1 s from FireRate). A2: build ok and the full `RogueRigs.*` automation run passes with the committed CSVs. A3: mutation check: with `WeaponStats.csv` Shotgun Damage 10 -> 13, BurstRifle Damage 10 -> 12, TurretLauncher FireRate 2.22 -> 3 and `EnemyStats.csv` Chaser MaxHealth 30 -> 40 edited by hand, the full run still passes; the CSVs are then restored (`git diff` empty on them). A4: no gdd node repeats a number held in a CSV; each cites `<file>.csv` row `<Name>` with its status and decision date. A5: `_authoring` and `cpp-standard` state the rule (edits applied on confirmation).
- gdd_check: Audit 2026-11-02. Runtime source already single: 8 CSVs in `Content/RogueRigs/Data/Tables/`, parsed at runtime by `URRBaseStatsLoader` (no imported asset). Copies found: (1) tests: 19 test files hard-code CSV values (`constexpr float PelletDamage = 6.f`, etc.); the owner's hand edits of WeaponStats.csv (d2549efb, Shotgun 6 -> 10, TurretLauncher FireRate 3.3 -> 2.22) left RRShotgunTests, RRTurretTests and RRWeaponCardTests to be fixed by hand (uncommitted in the working tree); (2) gdd nodes restate the numbers (shotgun, burst-rifle, turret stats lines; enemy, player, explosion, upgrade, rarity, XP, ore nodes likely too); today they match the CSV. Not copies of CSV values: about 300 `UPROPERTY(EditDefaultsOnly)` numeric defaults in C++ headers (shake amplitudes, placement distances, pool sizes), possibly overridden in Blueprints; out of scope by default (Q4). No sync or lint between CSV, tests and vault exists in `vault/scripts/` (lint.js checks vault structure only). Conflict: `_authoring` and the Planner write decided values into gdd nodes; the new rule moves the number to the CSV and leaves the citation in the gdd (Q2, Q5). No open value needed. Milestone M3, same as the current work.
- confirmed_by_owner: yes (2026-11-02, "Tout par défaut")
- milestone: M3
- next_step: task-420-03-gdd-cite-csv
- started: 2026-11-02T19:01:41Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-02 (owner "Tout par défaut"): every default below is accepted; values provisional, owner 2026-11-02.
- Q1 Source unique: les fichiers CSV de `Content/RogueRigs/Data/Tables/` (déjà lus par le jeu au lancement) sont le seul endroit d'une valeur; tu modifies le CSV à la main. Default: oui.
- Q2 Forme dans le vault gdd: (A) les nœuds gdd ne recopient plus les chiffres, ils citent le fichier et la ligne (ex. "stats (provisional, owner 2026-10-31): `WeaponStats.csv` ligne `Shotgun`") ; (B) le lint régénère à chaque commit un tableau de valeurs dans chaque nœud gdd à partir du CSV (chiffres visibles dans le vault, jamais édités à la main, une tâche outil en plus). Default: A.
- Q3 Tests: ils lisent leurs valeurs attendues dans le CSV et calculent les dérivées (crit = dégâts x 1,5 lu aussi dans le CSV, tirs par seconde, santé de survie). Vérification: changer à la main dégâts Shotgun, BurstRifle, cadence Turret et santé Chaser, toute la suite passe, puis remise des valeurs. Fait d'abord sur les tests d'armes (ton exemple), puis sur les autres familles. Default: oui.
- Q4 Périmètre: seulement les valeurs des 8 CSV. Les ~300 réglages `EditDefaultsOnly` du C++ / Blueprint (ex. secousse du slam 8 uu) restent où ils sont, hors de cette tâche. Default: oui.
- Q5 Règle écrite: j'ajoute la règle dans `_authoring` (vault) et `cpp-standard` (C++/tests), deux nœuds règle qui demandent ton accord. Default: oui.
- Q6 Tes modifs non commitées des tests (Shotgun 6 -> 10, Turret 3.3 -> 2.22, WeaponCard) : la tâche part de ces modifs et les remplace par la lecture du CSV; `Config/DefaultEditor.ini` n'est pas touché. Default: oui.

## Subtasks
1. task-420-01-weapon-tests-read-csv (vault/reference/tasks/task-420-single-source-game-values/task-420-01-weapon-tests-read-csv.md) — cpp: slice: a test helper reading CSV rows, weapon test files use it (no literal CSV value), mutation check
2. task-420-02-other-tests-read-csv (vault/reference/tasks/task-420-single-source-game-values/task-420-02-other-tests-read-csv.md) — cpp: expansion: the other test families read EnemyStats, PlayerBaseStats, ExplosionStats, Upgrade*, XPCurve, OreTypes through the helper
3. task-420-03-gdd-cite-csv (vault/reference/tasks/task-420-single-source-game-values/task-420-03-gdd-cite-csv.md) — doc: gdd nodes cite the CSV row instead of copying its numbers
4. task-420-04-doc (vault/reference/tasks/task-420-single-source-game-values/task-420-04-doc.md) — doc: test docs: tests read expected values from the CSV through the helper

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-420-01-weapon-tests-read-csv]] — step 1, cpp.
- [[task-420-02-other-tests-read-csv]] — step 2, cpp.
- [[task-420-03-gdd-cite-csv]] — step 3, doc.
- [[task-420-04-doc]] — step 4, doc.
- [[cpp-standard]] — the data rule this task extends
- [[sys-automation-tests]] — the tests that read the CSV
