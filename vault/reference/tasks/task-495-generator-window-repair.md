---
id: task-495-generator-window-repair
description: "Global task: a post-generation repair pass in the C++ layout generator that fixes dead and too-open 24 x 24 windows, so every generated layout meets the window rule."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Repair pass so every generated layout meets the 24 x 24 window rule

- request: Owner, 2026-11-07, after task-492-03 (24/24 C++ seeds playable, only 4/24 meet the 24 x 24 window rule of layout-review-rules; table Saved/GeneratedLevels/task-492/review_cpp.md). Options offered by the main session: A) a post-generation repair pass in FRRLayoutGenerator that locally fixes offending windows (carve into dead zones, add rock into too-open zones), like the existing 3-cell passage repair, so every generated layout meets the window rule without extra seed retries; B) auto-reject seeds; C) relax the rule. Owner answer (verbatim): "A." Main session constraints: run before task-494; keep the validator constraints (passage 3, reachability, heal and barrel counts) and determinism; level-design re-review on the same 24 seed starts as the check.
- goal: Every layout the C++ generator returns keeps every void-free 24 x 24 window between 0.05 and 0.95 open, by local repair rather than extra seed retries, while staying valid, playable and deterministic.
- acceptance: A1 On the 24 seed starts of task-492-03 (review_cpp.md), every layout the generator returns has every void-free 24 x 24 window (stride 2, floor, barrel and spawn open, missing cells floor) within 0.05 to 0.95 open; the generator reports min and max window fraction per layout. A2 All 24 pass validate_layout.js and the recipe acceptance and brief bounds: minimum passage 3, one reachable piece, reachable floor and coverage in bounds, max distance to floor within maxd, hidden pockets kept, heal_ore 56 and barrels 17 to 22. A3 Same params and seed give byte-identical JSON on two runs (at least 2 seeds). A4 Repaired areas look like the rest of the terrain (organic walls and outcrops, no straight cut or square block) on the downscaled previews, judged by the level-design review. A5 Seed retries per start and generation time per seed reported against task-492-03; most starts accept the same first seed as before (the repair fixes, it does not search). A6 Existing generator and grid tests still pass; committed layouts (arena_m1_fine, arena_l2 to l4) untouched.
- gdd_check: Read: sys-layout-generator, layout-generator-recipe, layout-review-rules, task-494-generator-test-map. Not a gdd feature (dev generator, tech pipeline); no gdd node touched. No conflict: Q3 of task-494 kept review rules manual; the owner now asks this one rule (windows) to be enforced by repair, recorded in layout-review-rules by edit. Thresholds 0.05 and 0.95 and scan method taken from layout-review-rules (Q1). No open value. Milestone M2. Dependency: task-494-01 now depends on this task (edit). task-493 in progress, not touched. No code read.
- verification_level: standard
- confirmed_by_owner: no (plan and questions handed back 2026-11-07)
- milestone: M2
- next_step: task-495-01-repair-pass
- started: open
- waiting_for: owner answers to Q1-Q4 of task-495
- result: open

## Questions
Asked 2026-11-07, each with a default; open until the owner answers.
- Q1 Règle à garantir: chaque fenêtre 24 × 24 sans vide (pas de 2, sol, baril et spawn comptent comme ouverts) entre 0.05 et 0.95 d'ouverture, exactement le scan de la review level design. Les fenêtres qui touchent le vide (bord de l'astéroïde) restent hors règle. Défaut: oui.
- Q2 Filet de sécurité: après la réparation, la règle des fenêtres devient un contrôle d'acceptation de la seed; si une seed reste hors règle (ou si la réparation casse un autre contrôle: passage 3, atteignabilité, poches, soins, barils), le générateur passe à la seed suivante comme aujourd'hui. Le but reste que la réparation suffise presque toujours. Défaut: oui.
- Q3 Périmètre: seulement la règle des fenêtres, seulement le générateur C++. La règle des longues lignes droites et la distance des poches restent vérifiées à la main; le script Node n'est pas modifié; les maps déjà commitées ne sont pas régénérées. Défaut: oui.
- Q4 Niveau de vérification: standard (nouveau comportement du générateur, revue level design sur les 24 seeds, pas de capture car rien de visible en jeu). Défaut: standard.

## Subtasks
1. task-495-01-repair-pass (vault/reference/tasks/task-495-generator-window-repair/task-495-01-repair-pass.md) — cpp: Window repair pass in FRRLayoutGenerator (carve into dead windows, add rock into too-open windows) plus the window check in seed acceptance, keeping every recipe and validator constraint and determinism.
2. task-495-02-ld-review (vault/reference/tasks/task-495-generator-window-repair/task-495-02-ld-review.md) — level-design: Level-design re-review of the 24 seed starts of task-492-03 with layout-review-rules, compared with review_cpp.md.
3. task-495-03-doc (vault/reference/tasks/task-495-generator-window-repair/task-495-03-doc.md) — doc: Document the repair pass and the new acceptance check.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-495-01-repair-pass]] — step 1, cpp.
- [[task-495-02-ld-review]] — step 2, level-design.
- [[task-495-03-doc]] — step 3, doc.
