---
id: task-469-aim-laser-green
description: "Global task: The aim laser turns from white-gold to the bright lime green of the owner in-game capture; only the colour changes."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Aim laser in the lime green of the owner capture

- request: Owner 2026-11-06 (verbatim): "On va arrêter de limiter la ligne d'AIM en longueur. Juste cela s'arrête lorsque cela touche un mur ou un robot. ( attention sur l'image ça passe au dessus du mur ) Prend la couleur comme référence." With one in-game capture, kept as Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp (owner reference): the miner in the middle of the arena, a thin bright lime-green aim line leaves the rifle Muzzle to the right and runs to the screen edge, drawn over the rock cliffs of the arena border on the right; that crossing of the wall is the defect he points at; the green of the line is the colour reference. Point 2 of 2 (colour); the length and stop are task-468-aim-laser-unlimited-length. Owner answer 2026-11-06 (relayed by the main session, verbatim): "Pas de capture, ok pour le reste. ne lance rien, prépare juste la tâche" = the plan and the defaults of Q4, Q5 accepted. Owner launch order (relayed by the main session, verbatim): "Lance les task to do dans l'ordre" = launch now; this lifts the earlier "do not launch" instruction.
- goal: The aim line has the green of the owner capture.
- acceptance: A1 in a -game run the aim laser is the bright lime green of Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp (pale yellow-green core, lime glow), no longer white-gold; A2 width, glow width, brightness profile, Muzzle height and length rules unchanged by this task; A3 with a Shooter (acid green) on screen it is still spotted at once and not confused with the aim line; A4 no capture and no capture subtask (owner Q3, 2026-11-06, "Pas de capture"): the owner checks in game himself; the final report lists what to look at, says plainly that the look is unverified until he has seen it, and gives the token cost per subtask and in total; the owner said "ne lance rien, prépare juste la tâche": prepared only, launched only on his explicit word; A5 aim-indicator and color-code state the new colour, and the aim-laser part of task-464 A6 no longer asks for white-gold.
- gdd_check: Read aim-indicator, color-code, task-464-player-shot-colour. Conflicts: aim-indicator and color-code say the aim laser is white-gold and never acid green; task-464 (todo) A6 checks the aim laser stays white-gold. The owner reference is a bright lime green close to the green of the player laser shots of task-464 and not far from the Shooter acid green; the owner accepted a similar green for shots (task-464 A8 checks the Shooter); Q4 default: the lime green of his capture, single colour core plus glow, A3 checks the Shooter. Edits at confirmation: aim-indicator Colour, color-code (aim laser leaves the white-gold row, own row), task-464 A6. Owner image kept in Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp (Q5). No open value. No code read.
- confirmed_by_owner: yes (2026-11-06, "Pas de capture, ok pour le reste. ne lance rien, prépare juste la tâche")
- milestone: M2
- next_step: task-469-02-doc
- started: 2026-11-06T10:29:46Z
- waiting_for: none
- executor_tokens: input 40, cache write 44398, cache read 747082, output 8195
- executor_usage: runs 1, turns 19, context start 18k peak 56k, resumes 0, 3 min, units 0.17 M
- planning_tokens: input 25, cache write 33914, cache read 560922, output 11927
- planning_usage: aa81a0a3 "Plan unlimited aim line" 1/2 (tasks 468, 469), units 0.16 M
- result: open

## Questions
Decided 2026-11-06 (owner "Pas de capture, ok pour le reste. ne lance rien, prépare juste la tâche"): Q4 and Q5 defaults accepted (Q3 in task-468: no capture, the owner checks in game himself); values provisional, owner 2026-11-06.
- Q4 Couleur. Default: la ligne de visée prend le vert citron vif de ta capture (cœur clair jaune-vert, halo vert citron), relevé sur l'image ; seule la couleur change (épaisseur, halo, luminosité inchangés). Cela remplace le blanc-or et corrige la tâche 464 (son point A6 ne demande plus le viseur blanc-or ; le laser de minage reste blanc-or). Le Shooter étant vert acide, la capture vérifie qu'on le repère toujours. Autre choix : le même vert varié que les tirs laser du joueur de la tâche 464.
- Q5 Ta capture est gardée comme référence ("keep") dans Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp et citée dans le code couleur. Default: oui, keep.

## Subtasks
1. task-469-01-green (vault/reference/tasks/task-469-aim-laser-green/task-469-01-green.md) — bp: Aim laser colour set to the lime green of the owner capture, look otherwise unchanged.
2. task-469-02-doc (vault/reference/tasks/task-469-aim-laser-green/task-469-02-doc.md) — doc: The aim indicator tech doc states the new colour values.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-469-01-green]] — step 1, bp.
- [[task-469-02-doc]] — step 2, doc.
