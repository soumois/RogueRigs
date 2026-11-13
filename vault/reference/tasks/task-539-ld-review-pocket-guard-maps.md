---
id: task-539-ld-review-pocket-guard-maps
description: "Global task: Level-design visual review of the 7 starts whose accepted seed changed after task-537 (pocket guard release), before against after; review only."
parent: tasks-done-index
kind: task
status: done
---
# Revue visuelle Level-Design des cartes changées par la libération du garde de poche

- request: Main session question: "Veux-tu une revue visuelle Level-Design des 7 départs dont la carte a changé ?" Owner answer: "Oui, revue visuelle". Context: task-537-layout-stale-pocket-guard (code c324273f, result 64780ad8) changed the generated layouts; its Q2 default kept automatic revalidation only, no visual review. 7 of the 24 starts of task-492-03 now accept another seed: 1500 (1502 -> 1500), 1700 (1705 -> 1702), 1900 (1901 -> 1900), 2000 (2000 -> 2006), 2700 (2702 -> 2700), 2800 (2806 -> 2803), 7000 (7002 -> 7000). 15 other starts keep their seed with smaller cell changes; 1000 and 2600 are identical. Owner answer 2026-11-13 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: Judge, as the Level-Design role, whether the new maps of the 7 starts whose accepted seed changed read as good arenas under layout-review-rules, compared with their map before task-537, and show the owner the before and after pairs.
- acceptance: A1 the 7 starts (1500, 1700, 1900, 2000, 2700, 2800, 7000) give, with the default params and brief of the task-537 revalidation, the accepted seeds listed in request and pass validate_layout.js; a different seed is reported and not reviewed further; A2 per start, before and after, a table from a scan independent of the generator (as task-495-02-ld-review): accepted seed, every void-free 24 x 24 window min and max (rule 0.05 to 0.95), longest straight lane (30 cells, +/-1), farthest hidden pocket (15 cells); A3 per start, a visual verdict on the downscaled previews before and after: dead or too-open zones, repaired areas, pocket readability, any unnatural area named with its place, verdict ok / a surveiller / defaut; A4 one sheet of the 7 before/after pairs is shown to the owner (path in result); no change to the generator, params, briefs or committed levels; verdict written in the subtask node and one line in layout-review-rules Automation.
- gdd_check: Read: layout-review-rules, agent-level-design, task-492-03-ld-validation, task-495-02-ld-review, request-families, verification-levels. No gdd node touched: dev-time check of the C++ generator output. No conflict, no open value needed. Milestone M2 (as task-492 and task-495). Family: comportement as relayed; a review-only request fits no family exactly, comportement is the planner path closest. Level light: no new asset or system, nothing changed. Saved/GeneratedLevels/task-537/ checked to exist with before/ and after/ (start_*.json and briefs, logs); temporary. Not in scope: in-game check (not asked), the 15 starts with smaller changes (Q1).
- verification_level: light
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "ok")
- milestone: M2
- next_step: none
- started: 2026-11-13T20:18:51Z
- waiting_for: none
- executor_tokens: input 92, cache write 102910, cache read 2741116, output 30279
- executor_usage: runs 1, turns 44, context start 17k peak 103k, resumes 2, 7 min, units 0.55 M, USD 1.67
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. A1: the 7 starts regenerated at HEAD give 1500, 1702, 1900, 2006, 2700, 2803, 7000 with the task-537/after cell hashes, and all 14 maps pass validate_layout.js. A2: independent scan (scan539.js) matches the generator: after windows 0.063-0.929, lanes 19-28, farthest pocket 6-10, all within the rules. A3 verdicts: 1500 ok; 2000 ok (west lane of 28 at 62-70,122-149); 1700 a surveiller (six pockets packed in the east third at 150-225,60-200, straight carved bar at 117-140,72-80); 1900 a surveiller (7 carved repairs read as straight bars); 7000 a surveiller (open mass south of the spawn at 70-140,120-200 kept under 0.95 by outcrops); 2700 defaut (merged hidden region of 1609 cells at 166-228,76-153, hidden floor 0.146 to 0.239); 2800 defaut (merged hidden region of 1225 cells at 28-86,125-209). The window rule counts hidden pocket floor as open, so it misses these areas; windows with network floor under 0.05: 2700 536 to 1099, 2800 526 to 791, 1700 526 to 940. Reported only, nothing changed. Repairs are marked on the after maps only (the before code cannot be rebuilt). Sheet (temporary): Saved/GeneratedLevels/task-539/task-539_before_after_sheet.png; table Saved/GeneratedLevels/task-539/review_task539.md.

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Perimetre : seulement les 7 departs dont le seed accepte a change, ou aussi les 15 departs qui gardent leur seed avec de petits changements de cellules ? Default: les 7 seulement, comme la question posee ; les 15 autres restent couverts par la revalidation automatique de task-537.
- Q2 Si la revue trouve une zone peu naturelle ou un defaut sur une carte : corriger le generateur dans cette tache (comme task-495-02, avec relance des 24 departs) ou seulement le signaler ? Default: seulement le signaler avec sa position et une image ; une correction devient une nouvelle tache que tu decides.
- Q3 Ce que tu vois : une planche des 7 paires avant/apres (apercus reduits, zones reparees marquees) avec le verdict par depart. Default: oui, une planche PNG temporaire dans Saved/, plus le tableau des mesures.

## Subtasks
1. task-539-01-ld-review (vault/reference/tasks/task-539-ld-review-pocket-guard-maps/task-539-01-ld-review.md) — level-design: Before/after visual and measured review of the 7 starts whose accepted seed changed after task-537.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-539-01-ld-review]] — step 1, level-design.
