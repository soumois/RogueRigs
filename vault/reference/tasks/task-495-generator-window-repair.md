---
id: task-495-generator-window-repair
description: "Global task: the C++ layout generator enforces every layout review rule (24 x 24 windows, straight lanes, pocket distance) by repair and acceptance checks."
parent: tasks-to-do-index
kind: task
status: todo
---
# C++ generator enforces the layout review rules (windows, straight lanes, pocket distance)

- request: Owner, 2026-11-07, after task-492-03 (24/24 C++ seeds playable, only 4/24 meet the 24 x 24 window rule of layout-review-rules; table Saved/GeneratedLevels/task-492/review_cpp.md). Options offered by the main session: A) a post-generation repair pass in FRRLayoutGenerator that locally fixes offending windows (carve into dead zones, add rock into too-open zones), like the existing 3-cell passage repair, so every generated layout meets the window rule without extra seed retries; B) auto-reject seeds; C) relax the rule. Owner answer (verbatim): "A." Main session constraints: run before task-494; keep the validator constraints (passage 3, reachability, heal and barrel counts) and determinism; level-design re-review on the same 24 seed starts as the check. Owner remark 2026-11-07 (relayed): in game there is no level-design agent, it only serves to refine the script. Main session offer: also automate the two rules left manual in Q3 (straight lane over 30 cells, hidden pocket distance from the main paths) as seed-acceptance checks in the C++ generator, with a repair for long straight lanes where feasible, so the generator guarantees every layout-review-rules point at runtime without an agent. Owner answer (verbatim): "Oui". Main session reading: Q1, Q2, Q4 default; Q3 changed to window + straight lane + pocket distance, all in the C++ generator; Node script unchanged, committed maps not regenerated. Owner answer 2026-11-07 (relayed by the main session, verbatim): "Oui" = the plan and the defaults of Q1, Q2, Q4 accepted.
- goal: Every layout the C++ generator returns meets every point of layout-review-rules without any agent: every void-free 24 x 24 window between 0.05 and 0.95 open, no straight lane over 30 cells, no hidden pocket farther than 15 cells from the main network; by local repair where feasible rather than extra seed retries, with acceptance checks as safety net, while staying valid, playable and deterministic.
- acceptance: A1 On the 24 seed starts of task-492-03 (review_cpp.md), every layout the generator returns has every void-free 24 x 24 window (stride 2, floor, barrel and spawn open, missing cells floor) within 0.05 to 0.95 open; the generator reports min and max window fraction per layout. A1b On the same 24 starts, no straight lane (corridor 3 to 10 cells wide running straight, ±1 cell tolerance on jagged walls) longer than 30 cells, and no hidden pocket farther than 15 cells (max_far) from the main network; the generator reports the longest straight lane and the farthest pocket distance per layout. A2 All 24 pass validate_layout.js and the recipe acceptance and brief bounds: minimum passage 3, one reachable piece, reachable floor and coverage in bounds, max distance to floor within maxd, hidden pockets kept, heal_ore 56 and barrels 17 to 22. A3 Same params and seed give byte-identical JSON on two runs (at least 2 seeds). A4 Repaired areas (windows and broken lanes) look like the rest of the terrain (organic walls and outcrops, no straight cut or square block) on the downscaled previews, judged by the level-design review. A5 Seed retries per start and generation time per seed reported against task-492-03; most starts accept the same first seed as before (the repairs fix, they do not search). A6 Existing generator and grid tests still pass; committed layouts (arena_m1_fine, arena_l2 to l4) untouched.
- gdd_check: Read: sys-layout-generator, layout-generator-recipe, layout-review-rules, task-494-generator-test-map. Not a gdd feature (dev generator, tech pipeline); no gdd node touched. No conflict: Q3 of task-494 kept review rules manual; the owner now asks all three review rules to be enforced by the C++ generator, recorded in layout-review-rules by edit. Thresholds 0.05 and 0.95 and scan method taken from layout-review-rules (Q1). Straight lane: layout-review-rules said flagged, not rejecting, and left a hard rule to the owner; his "Oui" makes it a hard rule at 30 cells with ±1 tolerance (provisional, owner 2026-11-07; arena_l2 36 and arena_l4 44 cells would now fail, committed maps not regenerated). Pocket distance: max_far 15 from layout-review-rules. No open value. Milestone M2. Dependency: task-494-01 now depends on this task (edit). task-493 in progress, not touched. No code read.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-07, "Oui")
- milestone: M2
- next_step: task-495-01-repair-pass
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-07 (owner "Oui"): the defaults of Q1, Q2, Q4 are accepted; values provisional, owner 2026-11-07.
- Q1 Règle à garantir: chaque fenêtre 24 × 24 sans vide (pas de 2, sol, baril et spawn comptent comme ouverts) entre 0.05 et 0.95 d'ouverture, exactement le scan de la review level design. Les fenêtres qui touchent le vide (bord de l'astéroïde) restent hors règle. Défaut: oui.
- Q2 Filet de sécurité: après la réparation, la règle des fenêtres devient un contrôle d'acceptation de la seed; si une seed reste hors règle (ou si la réparation casse un autre contrôle: passage 3, atteignabilité, poches, soins, barils), le générateur passe à la seed suivante comme aujourd'hui. Le but reste que la réparation suffise presque toujours. Défaut: oui.
- Q3 Périmètre: seulement la règle des fenêtres, seulement le générateur C++. La règle des longues lignes droites et la distance des poches restent vérifiées à la main; le script Node n'est pas modifié; les maps déjà commitées ne sont pas régénérées. Défaut: oui. Réponse owner 2026-11-07 ("Oui" à la proposition de la session principale): défaut changé, les trois règles (fenêtres, lignes droites > 30 cellules, poches à plus de 15 cellules) sont garanties par le générateur C++ (réparation quand c'est faisable, contrôle d'acceptation en filet); script Node inchangé, maps commitées non régénérées.
- Q4 Niveau de vérification: standard (nouveau comportement du générateur, revue level design sur les 24 seeds, pas de capture car rien de visible en jeu). Défaut: standard.

## Subtasks
1. task-495-01-repair-pass (vault/reference/tasks/task-495-generator-window-repair/task-495-01-repair-pass.md) — cpp: Repair passes and seed-acceptance checks in FRRLayoutGenerator for the three review rules (windows, straight lanes, pocket distance), keeping every recipe and validator constraint and determinism.
2. task-495-02-ld-review (vault/reference/tasks/task-495-generator-window-repair/task-495-02-ld-review.md) — level-design: Dev-time level-design re-review of the 24 seed starts of task-492-03 against all layout-review-rules, compared with review_cpp.md.
3. task-495-03-doc (vault/reference/tasks/task-495-generator-window-repair/task-495-03-doc.md) — doc: Document the repair passes and the new acceptance checks.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-495-01-repair-pass]] — step 1, cpp.
- [[task-495-02-ld-review]] — step 2, level-design.
- [[task-495-03-doc]] — step 3, doc.
