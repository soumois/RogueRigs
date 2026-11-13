---
id: task-539-ld-review-pocket-guard-maps
description: "Global task: Level-design visual review of the 7 starts whose accepted seed changed after task-537 (pocket guard release), before against after; review only."
parent: tasks-to-do-index
kind: task
status: todo
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
- next_step: task-539-01-ld-review
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-13 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Perimetre : seulement les 7 departs dont le seed accepte a change, ou aussi les 15 departs qui gardent leur seed avec de petits changements de cellules ? Default: les 7 seulement, comme la question posee ; les 15 autres restent couverts par la revalidation automatique de task-537.
- Q2 Si la revue trouve une zone peu naturelle ou un defaut sur une carte : corriger le generateur dans cette tache (comme task-495-02, avec relance des 24 departs) ou seulement le signaler ? Default: seulement le signaler avec sa position et une image ; une correction devient une nouvelle tache que tu decides.
- Q3 Ce que tu vois : une planche des 7 paires avant/apres (apercus reduits, zones reparees marquees) avec le verdict par depart. Default: oui, une planche PNG temporaire dans Saved/, plus le tableau des mesures.

## Subtasks
1. task-539-01-ld-review (vault/reference/tasks/task-539-ld-review-pocket-guard-maps/task-539-01-ld-review.md) — level-design: Before/after visual and measured review of the 7 starts whose accepted seed changed after task-537.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-539-01-ld-review]] — step 1, level-design.
