---
id: task-525-horde-marker-one-at-a-time
description: "Global task: The alert bar shows only the next horde "!", removed when that horde ends, then the next one appears."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Un seul point d'exclamation de horde à la fois

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « On affiche qu'un point d'exclamation à la fois pour les hordes, lorsqu'une horde est fini, on fait disparaitre son point d'exclamation et on indique le prochain point d'exclamation pour indiquer la prochaine vague. ». Owner answer 2026-11-13 (relayed by the main session, verbatim): "tout par défaut" = the plan and the defaults of Q11 accepted.
- goal: The alert bar shows a single horde "!" at a time, per Q11.
- acceptance: A1 at level start one "!" only, at the first horde; A2 when the first horde is over its "!" goes and the second appears; A3 after the last horde no "!"; A4 the C, B, S letters unchanged; A5 build green, existing HUD tests pass (expectations updated only where broken); capture and doc done.
- gdd_check: Read level-flow, enemies-spawn-rules. No conflict (adds to the M1 marker rules). Open: when a horde is "over" and whether the first "!" shows from the start (Q11). Edit: level-flow. No code read.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "tout par défaut")
- milestone: M2
- next_step: task-525-02-capture
- started: 2026-11-13T14:03:33Z
- waiting_for: none
- executor_tokens: input 72, cache write 70009, cache read 2083012, output 14977
- executor_usage: runs 1, turns 36, context start 16k peak 83k, resumes 0, 4 min, units 0.37 M, USD 1.07
- planning_tokens: input 6, cache write 18666, cache read 174110, output 3853
- planning_usage: a163c449 "Plan owner feedback batch" 1/9 (tasks 520, 521, 522, 523, 524, 525, 526, 527, 528), units 0.06 M, USD 0.21
- result: open

## Questions
Decided 2026-11-13 (owner "tout par défaut"): every default below is accepted; values provisional, owner 2026-11-13.
- Q11 Un seul « ! » sur la barre : celui de la prochaine horde, visible dès le début du niveau ; une horde est finie au même moment qu'aujourd'hui pour le remplissage par les kills (fin de son front) : son « ! » disparaît et celui de la suivante apparaît ; après la dernière, plus aucun « ! ». Default: oui.

## Subtasks
1. task-525-01-single-marker (vault/reference/tasks/task-525-horde-marker-one-at-a-time/task-525-01-single-marker.md) — cpp: one horde "!" at a time per Q11
2. task-525-02-capture (vault/reference/tasks/task-525-horde-marker-one-at-a-time/task-525-02-capture.md) — capture: alert bar before, between and after the hordes
3. task-525-03-doc (vault/reference/tasks/task-525-horde-marker-one-at-a-time/task-525-03-doc.md) — doc: alert bar doc: single horde marker

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-525-01-single-marker]] — step 1, cpp.
- [[task-525-02-capture]] — step 2, capture.
- [[task-525-03-doc]] — step 3, doc.
