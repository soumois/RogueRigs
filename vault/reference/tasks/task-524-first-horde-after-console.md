---
id: task-524-first-horde-after-console
description: "Global task: The first horde front starts a little after the console lands instead of with it."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Première horde un peu après l'arrivée de la console

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « Il faudrait que la premiére vague de monstre ( horde ) soit un peu après l'arrivé de la console. ». Owner answer 2026-11-13 (relayed by the main session, verbatim): "tout par défaut" = the plan and the defaults of Q10 accepted.
- goal: The first horde front starts the delay of Q10 after the console lands, its banner and alert bar "!" consistent with it; the second horde unchanged.
- acceptance: A1 in a -game run the log shows console landed then the first horde banner 4 s before a front starting at the Q10 delay after the landing; A2 the second horde keeps its time; A3 the first "!" sits after the C letter on the alert bar, never hiding it; A4 build green, existing wave and horde tests pass (expectations updated only where this change breaks them); capture and doc done.
- gdd_check: Read enemies-spawn-rules, level-flow, objective-console. Fact: console at 40 % of a 4 min bar (about 1:36) and first front at about 1:35, so today they coincide (image 1: "!" on C). Open: the delay (Q10). Edit: enemies-spawn-rules. No code read.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "tout par défaut")
- milestone: M2
- next_step: task-524-03-doc
- started: 2026-11-13T13:41:04Z
- waiting_for: none
- executor_tokens: input 170, cache write 200148, cache read 12200992, output 46069
- executor_usage: runs 1, turns 85, context start 16k peak 213k, resumes 0, 17 min, units 1.70 M, USD 4.36
- planning_tokens: input 6, cache write 18666, cache read 174110, output 3853
- planning_usage: a163c449 "Plan owner feedback batch" 1/9 (tasks 520, 521, 522, 523, 524, 525, 526, 527, 528), units 0.06 M, USD 0.21
- result: open

## Questions
Decided 2026-11-13 (owner "tout par défaut"): every default below is accepted; values provisional, owner 2026-11-13.
- Q10 La première horde arrive 20 s après l'atterrissage de la console (bannière 4 s avant, comme aujourd'hui) ; la deuxième garde son heure (vers 2:55). Default: oui.

## Subtasks
1. task-524-01-horde-timing (vault/reference/tasks/task-524-first-horde-after-console/task-524-01-horde-timing.md) — cpp: first horde front timed from the console landing per Q10
2. task-524-02-capture (vault/reference/tasks/task-524-first-horde-after-console/task-524-02-capture.md) — capture: alert bar around the console landing and first horde
3. task-524-03-doc (vault/reference/tasks/task-524-first-horde-after-console/task-524-03-doc.md) — doc: wave director doc: first horde timing

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-524-01-horde-timing]] — step 1, cpp.
- [[task-524-02-capture]] — step 2, capture.
- [[task-524-03-doc]] — step 3, doc.
