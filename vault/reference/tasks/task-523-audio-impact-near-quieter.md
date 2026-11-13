---
id: task-523-audio-impact-near-quieter
description: "Global task: Robot impact sound a little quieter near the player, unchanged near the screen edge."
parent: tasks-done-index
kind: task
status: done
---
# Impact : un peu moins fort au corps à corps

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « Réduire un peu le bruit d'impact lorsqu'on presque au corps à corps, mais garder le même volume à sa distance presque bord écran ». Owner answer 2026-11-13 (relayed by the main session, verbatim): "tout par défaut" = the plan and the defaults of Q8, Q9 accepted.
- goal: The robot impact sound is quieter close to the player and unchanged near the screen edge (Q8, Q9), through the distance sound base class of task-522 (runs after task-522-01).
- acceptance: A1 rr.Audio.Play Impact with Verbose LogRRAudio at 0, 150, 500, 900, 1075 uu logs the factors of Q9; A2 the wall impact logs unchanged levels; A3 build green, existing audio tests pass; sys-audio documents the near reduction.
- gdd_check: Read audio-direction, sys-audio. Two readings of "bruit d'impact": robot impact (constant level today) or wall impact (already silent at the screen edge, so "same at the edge" fits only the robot impact): asked Q8. Values open: Q9. Edit: audio-direction. Depends on the base class of task-522.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-13T14:38:34Z
- waiting_for: none
- executor_tokens: input 62, cache write 82929, cache read 1990698, output 14046
- executor_usage: runs 1, turns 31, context start 16k peak 96k, resumes 0, 4 min, units 0.37 M, USD 1.09
- planning_tokens: input 6, cache write 18666, cache read 174110, output 3853
- planning_usage: a163c449 "Plan owner feedback batch" 1/9 (tasks 520, 521, 522, 523, 524, 525, 526, 527, 528), units 0.06 M, USD 0.21
- result: Done. Commit f4c8d58c: sys-audio gets near_reduction (row fields, GetNearFactor, Impact values Q9, checked factors), the Near fields in data, the (near f) log in debug, relation to task-523. sys-audio-distance-sounds not listed in inputs: left unchanged (its GetFade entry stays true, the near rule is in sys-audio). Lint clean.

## Questions
Decided 2026-11-13 (owner "tout par défaut"): every default below is accepted; values provisional, owner 2026-11-13.
- Q8 « Bruit d'impact » = l'impact des balles du joueur sur un robot (aujourd'hui même volume à toute distance) ; l'impact sur mur, qui baisse déjà jusqu'au silence au bord de l'écran, ne change pas. Default: oui.
- Q9 Valeurs : 0,6 fois le volume actuel (environ -4 dB) jusqu'à 150 uu du joueur, remontée linéaire jusqu'au volume actuel à 900 uu, inchangé au-delà (bord écran gauche/droite à 1075 uu). Default: oui.

## Subtasks
1. task-523-01-impact-curve (vault/reference/tasks/task-523-audio-impact-near-quieter/task-523-01-impact-curve.md) — cpp: robot impact quieter near the player per Q9
2. task-523-02-doc (vault/reference/tasks/task-523-audio-impact-near-quieter/task-523-02-doc.md) — doc: sys-audio: the near reduction of the robot impact

## Relations
- [[tasks-done-index]] — parent index.
- [[task-523-01-impact-curve]] — step 1, cpp.
- [[task-523-02-doc]] — step 2, doc.
