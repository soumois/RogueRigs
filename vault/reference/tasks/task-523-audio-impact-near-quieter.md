---
id: task-523-audio-impact-near-quieter
description: "Global task: Robot impact sound a little quieter near the player, unchanged near the screen edge."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Impact : un peu moins fort au corps à corps

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « Réduire un peu le bruit d'impact lorsqu'on presque au corps à corps, mais garder le même volume à sa distance presque bord écran »
- goal: The robot impact sound is quieter close to the player and unchanged near the screen edge (Q8, Q9), through the distance sound base class of task-522 (runs after task-522-01).
- acceptance: A1 rr.Audio.Play Impact with Verbose LogRRAudio at 0, 150, 500, 900, 1075 uu logs the factors of Q9; A2 the wall impact logs unchanged levels; A3 build green, existing audio tests pass; sys-audio documents the near reduction.
- gdd_check: Read audio-direction, sys-audio. Two readings of "bruit d'impact": robot impact (constant level today) or wall impact (already silent at the screen edge, so "same at the edge" fits only the robot impact): asked Q8. Values open: Q9. Edit: audio-direction. Depends on the base class of task-522.
- verification_level: standard
- family: comportement
- confirmed_by_owner: no (plan and questions handed back 2026-11-13)
- milestone: M2
- next_step: task-523-01-impact-curve
- started: open
- waiting_for: owner answers Q1 to Q15 and confirmation
- result: open

## Questions
Asked 2026-11-13, each with a default; open until the owner answers.
- Q8 « Bruit d'impact » = l'impact des balles du joueur sur un robot (aujourd'hui même volume à toute distance) ; l'impact sur mur, qui baisse déjà jusqu'au silence au bord de l'écran, ne change pas. Default: oui.
- Q9 Valeurs : 0,6 fois le volume actuel (environ -4 dB) jusqu'à 150 uu du joueur, remontée linéaire jusqu'au volume actuel à 900 uu, inchangé au-delà (bord écran gauche/droite à 1075 uu). Default: oui.

## Subtasks
1. task-523-01-impact-curve (vault/reference/tasks/task-523-audio-impact-near-quieter/task-523-01-impact-curve.md) — cpp: robot impact quieter near the player per Q9
2. task-523-02-doc (vault/reference/tasks/task-523-audio-impact-near-quieter/task-523-02-doc.md) — doc: sys-audio: the near reduction of the robot impact

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-523-01-impact-curve]] — step 1, cpp.
- [[task-523-02-doc]] — step 2, doc.
