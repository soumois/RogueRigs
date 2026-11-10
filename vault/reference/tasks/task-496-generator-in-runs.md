---
id: task-496-generator-in-runs
description: "Global task: Every run level built from the C++ layout generator: level 1 prepared from the main menu, next level from the extractor contact, hand JSON as fallback."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Layout generator wired into real runs

- request: Context: read-only Code Architect scan of the GDD against the code (2026-11-10); the owner asked for questions on the top 4 gaps, answered them, then said: "lance les 4 tâches, je ne sais pas où sont génerés les json, il s'agit d'un script cpp qui s'en occupe". Point 1 (layout generator in runs), owner decision: "On charge dés le main menu le premier level en json en asynchrone pour ne pas bloquer le main menu. Ensuite, pour les levels suivants, on charge le niveau suivant dés que tu as atteint l'extracteur". About the source of the JSON: "je ne sais pas où sont génerés les json, il s'agit d'un script cpp qui s'en occupe". Owner answer 2026-11-10 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5 accepted.
- goal: In a run, every level terrain comes from the deterministic C++ layout generator with a seed per run: level 1 is generated in the background from the main menu without blocking it; the next level is generated from the moment the miner reaches the extractor; the hand layouts stay as fallback. Answer for the owner on where the JSON comes from: the layouts used in runs today (Content/RogueRigs/Data/Levels: arena_m1_fine, arena_l2 to arena_l4, each with its .brief.json and .params.json) were produced beforehand by the Node scripts of Tools/layout_gen (arena_gen_fine.js and siblings) and committed; the C++ generator (FRRLayoutGenerator, a port of arena_gen_fine.js) is only run today by the RRLayoutGen commandlet, editor closed, which writes Saved/GeneratedLevels/<name>.json (folder absent on disk) and by the generator test map; no run calls it. After this task the run generates its layouts in memory at runtime, no file needed.
- acceptance: A1 In a -game run, level 1 to level 4 terrains come from the C++ generator; the log gives per level the run seed, the level seed, the params and brief used and the generation time; two runs give different terrains, the same seed gives the same terrain. A2 Level 1 generation starts when the main menu opens and runs off the game thread: the menu keeps responding (no frame over 100 ms attributable to it in the log); New Run uses the prepared layout, or waits behind the loading screen until it is ready. A3 Generation of level N+1 starts at the miner's contact with the extractor board zone (log time stamps), not at the cards screen; the stats screen still shows the finished level; Continue behaviour and the in-place rebuild of task-318 unchanged. A4 Generator params and briefs needed in a run are read from packaged content, never from Tools/; a -game run works with Tools/ unused. A5 When generation fails or no seed passes, that level falls back to its hand JSON with an error line; the run continues. A6 Retry and Main Menu then New Run start a new run with a new seed; the generator test map (rr.Level.New) and the RRLayoutGen commandlet keep working. A7 Owner-visible check: a -game capture of level 1 and level 2 terrains after the change, beside the hand layouts before.
- gdd_check: Read arena-grid (M2 generator line), intermission (preload from the cards screen), main-menu (New Run flow), extraction (board zone contact = level validated), sys-layout-generator, sys-run-layout, sys-run-transition-preload. Directory listings only (no code read): Content/RogueRigs/Data/Levels holds arena_m1, arena_m1_fine, arena_l2..l4 with .brief.json and .params.json beside the l2..l4 layouts; Tools/layout_gen holds the Node scripts and params/fine_default.json; Saved/GeneratedLevels does not exist. Why: the owner asked where the JSON is generated. Conflicts: intermission says loading starts when the cards screen opens; the owner moves it to the extractor contact (edit). sys-run-layout says level 1 is never requested (code fact, changes). No open value used. Scope: arena-grid puts the generator at M2; the owner pulls it in. Ambiguity asked: whether level 1 is generator output or the hand arena_m1_fine ("en json"). Edits: arena-grid, intermission, main-menu.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-10, "ok")
- milestone: M2
- next_step: task-496-03-capture
- started: 2026-11-10T20:16:14Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-10 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-10.
- Q1 Niveau 1 : "en json" peut vouloir dire le JSON fait à la main (arena_m1_fine, toujours le même) ou un niveau généré par le générateur C++ comme les suivants. Défaut : le niveau 1 est aussi généré par le C++ (un terrain neuf à chaque run), arena_m1_fine sert de secours.
- Q2 "Dès que tu as atteint l'extracteur" = au contact du miner avec l'extracteur (niveau validé) : la génération du niveau suivant démarre là ; la reconstruction du terrain reste à l'ouverture de l'écran des cartes, pour que l'écran de stats montre encore le niveau fini. Défaut : oui.
- Q3 Réglages par niveau : chaque niveau garde les params et le brief déjà rangés à côté des JSON (arena_l2/l3/l4, niveau 1 : réglages par défaut avec le brief de arena_m1_fine), déplacés dans le contenu packagé ; seule la seed change d'un run à l'autre. Défaut : oui.
- Q4 Seed : une seed tirée au hasard à chaque run (Retry compris), une seed par niveau dérivée d'elle, écrite dans le log seulement ; pas d'affichage joueur ni de commande pour la forcer. Défaut : oui.
- Q5 Échec (aucune seed valide ou erreur) : ce niveau prend son JSON fait main (arena_m1_fine, arena_l2..l4) avec une erreur dans le log, le run continue. Défaut : oui.

## Subtasks
1. task-496-01-generator-provider (vault/reference/tasks/task-496-generator-in-runs/task-496-01-generator-provider.md) — cpp: A run layout provider that generates each level with the C++ generator from a per-run seed, per-level params and brief from packaged content, hand JSON fallback.
2. task-496-02-request-timing (vault/reference/tasks/task-496-generator-in-runs/task-496-02-request-timing.md) — cpp: Level 1 layout requested when the main menu opens and used at New Run; next level requested at the extractor board contact instead of later.
3. task-496-03-capture (vault/reference/tasks/task-496-generator-in-runs/task-496-03-capture.md) — capture: One -game capture of the generated terrains of level 1 and level 2, compared with the hand layouts before.
4. task-496-04-doc (vault/reference/tasks/task-496-generator-in-runs/task-496-04-doc.md) — doc: Document the generator provider, the request timings and where layouts come from.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-496-01-generator-provider]] — step 1, cpp.
- [[task-496-02-request-timing]] — step 2, cpp.
- [[task-496-03-capture]] — step 3, capture.
- [[task-496-04-doc]] — step 4, doc.
