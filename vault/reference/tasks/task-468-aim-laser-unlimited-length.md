---
id: task-468-aim-laser-unlimited-length
description: "Global task: The aim laser loses its 600 uu cap and ends on the first wall, robot or barrel, never drawn over a wall."
parent: tasks-to-do-index
kind: task
status: todo
---
# Aim laser without length limit, stopping at walls and robots

- request: Owner 2026-11-06 (verbatim): "On va arrêter de limiter la ligne d'AIM en longueur. Juste cela s'arrête lorsque cela touche un mur ou un robot. ( attention sur l'image ça passe au dessus du mur ) Prend la couleur comme référence." With one in-game capture, kept as Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp (owner reference): the miner in the middle of the arena, a thin bright lime-green aim line leaves the rifle Muzzle to the right and runs to the screen edge, drawn over the rock cliffs of the arena border on the right; that crossing of the wall is the defect he points at; the green of the line is the colour reference. Point 1 of 2 (length and stop); the colour is task-469-aim-laser-green. Owner answer 2026-11-06 (relayed by the main session, verbatim): "Pas de capture, ok pour le reste. ne lance rien, prépare juste la tâche" = Q1 and Q2 defaults accepted; Q3 answered "Pas de capture": no capture, the owner checks in game, capture subtask dropped; not to be launched until he says so. Owner launch order (relayed by the main session, verbatim): "Lance les task to do dans l'ordre" = launch now; this lifts the earlier "do not launch" instruction.
- goal: The aim line shows the whole path of the shot: as long as needed, and ending exactly where the first wall, robot or barrel stops it.
- acceptance: A1 in a -game run the aim line has no length limit: aimed across open floor it runs from the Muzzle to the first wall, robot or barrel however far, and past the screen edge when nothing is in the way; A2 aimed at the rock walls and cliffs of the arena border (the case of the owner capture Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp) and at a breakable rock block, the line ends at the rock face and is never drawn over the top of the wall, from any side and aim angle; A3 aimed at a robot (a chaser shield included) or a barrel, it ends on it; pickups, turrets, drone, console and extractor do not stop it; A4 no fade at the tip: the line ends sharp on what it touches; width, glow, Muzzle height, the light-mask darkening of the far part, visibility rules and colour unchanged by this task; A5 still pure visual with no gameplay effect; aim assist (task-463) unchanged; existing tests are updated only where the removed 600 uu cap or tip fade breaks them, and they pass; A6 no capture and no capture subtask (owner Q3, 2026-11-06, "Pas de capture"): the owner checks in game himself; the final report lists what to look at, says plainly that the look is unverified until he has seen it, and gives the token cost per subtask and in total; the owner said "ne lance rien, prépare juste la tâche": prepared only, launched only on his explicit word.
- gdd_check: Read aim-indicator, miner, color-code, task-464-player-shot-colour. Conflicts: aim-indicator Length caps the line at 600 uu (task-423) and fades the last 150 uu; the owner removes the cap. "Mur ou robot": barrels stop shots too (miner, aim assist), so Q1 asks whether they stop the line (default yes) and states that border cliffs count as walls (the capture shows the line over them). Tip fade (task-423) lost its purpose without a range end: Q2, default removed. No open value. Owner image kept in Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp (Q5 in task-469). aim-indicator Length and Look edited at confirmation. Milestone of aim-indicator: M3. No code read; the crossing of the wall is not diagnosed.
- confirmed_by_owner: yes (2026-11-06, "Pas de capture, ok pour le reste. ne lance rien, prépare juste la tâche")
- milestone: M2
- next_step: task-468-01-unlimited-stop
- started: open
- waiting_for: none
- executor_tokens: none (no executor run)
- executor_usage: none
- planning_tokens: input 25, cache write 33914, cache read 560922, output 11927
- planning_usage: aa81a0a3 "Plan unlimited aim line" 1/2 (tasks 468, 469), units 0.16 M
- result: open

## Questions
Decided 2026-11-06 (owner "Pas de capture, ok pour le reste. ne lance rien, prépare juste la tâche"): Q1 and Q2 defaults accepted; Q3 not the default: no capture, the owner checks in game himself; values provisional, owner 2026-11-06.
- Q1 Ce qui arrête la ligne de visée. Default: n'importe quel mur de roche (blocs cassables et murs/falaises du bord de l'arène, le cas de ta capture), n'importe quel robot (bouclier du chaser compris) et les barils (un tir s'arrête aussi sur un baril). Les pickups, tourelles, drone, console et extracteur ne l'arrêtent pas. Si rien n'est touché, elle file jusqu'au bord de l'écran. Autre choix : murs et robots seulement, la ligne traverse les barils.
- Q2 Bout de la ligne. Default: plus de fondu au bout (aujourd'hui les 150 derniers uu s'estompent, tâche 423) : la ligne finit net sur le mur, le robot ou le baril touché, pour voir exactement où elle s'arrête. L'assombrissement de l'écran autour du mineur continue d'éteindre la partie lointaine, comme aujourd'hui. Autre choix : garder un court fondu au bout.
- Q3 Vérification. Default: chacune des deux tâches (longueur, couleur) finit par une capture en jeu qu'on t'envoie (ligne sur sol libre jusqu'au bord de l'écran, arrêt sur une falaise du bord, sur un bloc, sur un robot). Autre choix : pas de capture, tu vérifies en jeu toi-même.

## Subtasks
1. task-468-01-unlimited-stop (vault/reference/tasks/task-468-aim-laser-unlimited-length/task-468-01-unlimited-stop.md) — cpp: Aim laser without length cap, ending on the first wall, robot or barrel, never over a wall, sharp tip.
2. task-468-02-doc (vault/reference/tasks/task-468-aim-laser-unlimited-length/task-468-02-doc.md) — doc: The aim indicator tech doc describes the unlimited line and what stops it.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-468-01-unlimited-stop]] — step 1, cpp.
- [[task-468-02-doc]] — step 2, doc.
