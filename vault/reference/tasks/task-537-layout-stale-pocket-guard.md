---
id: task-537-layout-stale-pocket-guard
description: "Global task: C++ layout generator releases the guard of a removed hidden pocket, live pockets stay guarded; revalidated on 24 starts, idle filler measured."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Layout generator: the guard of a removed hidden pocket no longer blocks carving

- request: "Garde des poches" (owner 2026-11-13, pick of a track proposed by the main session after task-536). Track as presented to the owner: in the C++ generator a hidden pocket marks its cells hidden and a square of `pockets.guard` (3) cells around them as guard; guard cells are never carved by later walks, by disc carving, or picked as a target by the dead-window repair. When a pocket is removed (region refill: region too small or gap over `cleanup.max_gap`; passage repair: narrow cells), its cells go back to rock and stop being hidden, but the guard is never cleared, so the zone of a pocket that no longer exists stays uncarvable for the rest of the generation. The Node script `Tools/layout_gen/arena_gen_fine.js` behaves the same (inherited, not a port mistake). Request: the guard of a removed pocket no longer blocks carving; the guard of pockets that still exist keeps protecting them. Owner told: not output-preserving, layouts to revalidate (24 starts of task-492-03, layout-review-rules), the reference determinism line (level seed 422704489, arena_l3 -> seed 422704499, hash 86c36f6d) may change and then be re-recorded. Whether the stale guard causes the idle filler iterations measured in task-536 (default seed 17: 4971 of 5431 idle; arena_l3 seed 422704496: 4597 of 4818) is an open fact to establish, not an assumption. Not asked: an early exit of the filler loop when no far cell is carvable.
- goal: In the C++ layout generator, a cell is protected by the pocket guard only while a hidden pocket cell it guards still exists; layouts stay valid and deterministic, and the effect on the idle filler iterations is measured.
- acceptance: A1 Guard rule: after any step that puts pocket cells back to rock (region refill, passage repair, or any later step that removes pocket cells), a cell is guarded only if it lies within `pockets.guard` cells (square) of a cell still hidden; cells within that distance of a pocket still present (a pocket whose guard overlaps that of a removed one, the remaining part of a partly removed pocket) stay guarded. Shown on at least one seed where a pocket is removed (automated test or logged check), with the released cells counted. A2 Determinism: same params and seed still give byte-identical cells (two runs); at least one seed where no pocket is ever removed gives the same cells hash as before the change. A3 Revalidation on the 24 starts of task-492-03: each still finds a passing seed within `search.max_seeds` (acceptance, which includes the review checks of layout-review-rules) and passes the level-layout-schema validator; a before/after table gives per start the accepted seed, seeds tried, window min-max, longest lane, farthest pocket and search time. A4 Reference line: level seed 422704489 with arena_l3 params and brief run twice with the commandlet; the accepted seed and cells hash are recorded, new or stated unchanged. A5 Measure, no target: `-carveonly` on default params seed 17 and arena_l3 seed 422704496, before and after: filler time, iterations, idle iterations, pockets removed and guard cells released; the result states whether the stale guard was a cause of the idle iterations (yes, partly or no) with the evidence. A6 Existing tests pass; only expectations the change breaks are updated. The Node script, the committed layouts in Content/RogueRigs/Data/Levels and the params files are unchanged.
- gdd_check: Read: arena-grid (M2 deterministic generator, layouts in runs), layout-generator-recipe (step 3 "never carves next to a hidden pocket", step 6 cleanup refill, pocket guard 3), sys-layout-generator (repairs, determinism, reference line, task-536 timings), layout-review-rules (the 24-start verdict, rules enforced as acceptance), verification-levels. No conflict: the recipe protects existing hidden pockets; a refilled pocket is no longer a pocket. No `open` value needed. In scope (M2 generator, already used in runs). No code read by the Planner. Not visible on a screen element (no widget, HUD or asset change): no capture subtask; the layouts are judged by the generator acceptance and validator (Q2). Level standard: changed behaviour of a system, output not preserved.
- verification_level: standard
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-537-01-guard-release
- started: open
- waiting_for: Owner answers to Q1-Q3 (Node script, revalidation depth, a start that no longer passes).
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q1 Le script Node `arena_gen_fine.js` a le même défaut. On le corrige aussi ? Défaut : non, C++ seul ; le Node ne sert plus dans les runs, les cartes de secours ne sont pas régénérées, l'écart est noté dans la doc.
- Q2 Revalidation : quelle profondeur ? Défaut : vérification automatique des 24 départs de task-492-03 (acceptation du générateur, qui applique déjà toutes les règles de layout-review-rules, plus le validateur), tableau avant/après, sans revue visuelle Level-Design ni capture en jeu. Autre choix : ajouter une revue Level-Design (aperçu réduit) des seeds dont la carte a changé.
- Q3 Si un des 24 départs ne trouve plus de seed valide après le changement : défaut = l'exécuteur s'arrête et te demande (wait-answers), sans toucher aux paramètres. Une hausse du temps de recherche est seulement rapportée.

## Subtasks
1. task-537-01-guard-release (vault/reference/tasks/task-537-layout-stale-pocket-guard/task-537-01-guard-release.md) — cpp: Release the guard of removed hidden pockets in the C++ generator, keep live pockets guarded, revalidate and measure before/after
2. task-537-02-doc (vault/reference/tasks/task-537-layout-stale-pocket-guard/task-537-02-doc.md) — doc: Record the guard release, the new reference line and the measures in the generator docs

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-537-01-guard-release]] — step 1, cpp.
- [[task-537-02-doc]] — step 2, doc.
