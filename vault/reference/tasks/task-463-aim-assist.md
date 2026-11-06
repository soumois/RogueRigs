---
id: task-463-aim-assist
description: "Global task: A rifle bullet is deflected a few degrees toward a nearby robot, unless the shot already hits a robot or a barrel."
parent: tasks-to-do-index
kind: task
status: todo
---
# Light aim assist on miner shots

- request: Owner 2026-11-05 (verbatim): "J'aimerais préparer une sorte de AIM assist ( qui se désactive si on voit qu'on va toucher un enemi ou un baril ). Comme c'est léger on dévie de quelques degrés le projectile" Restated: a light aim assist: when a shot is fired, the projectile is deflected by a few degrees toward a nearby target; the assist does not apply when the shot as fired is already going to hit an enemy or a barrel. Owner answer 2026-11-06 (relayed by the main session, verbatim): "Je valide le choix, mais ne lance pas la tâche" = the plan and the defaults of Q39, Q40, Q41, Q42, Q43, Q44, Q45 accepted.
- goal: Shots that would narrowly miss a robot hit it, without the player losing control of where he shoots and without stealing shots aimed at a barrel.
- acceptance: A1 per Q39 to Q41, a burst rifle bullet whose straight path would miss, with a robot in range, in line of sight and within 5 degrees of the shot direction, leaves deflected toward the robot by at most 5 degrees and hits it if it does not move; read in a PIE run or in a test, not from a constant alone; A2 per Q42, a bullet whose straight path first hits a robot or a barrel is not deflected (a shot aimed at a barrel next to a robot still hits the barrel); A3 no deflection when no robot qualifies: robot outside the cone, beyond range, or behind a rock block; barrels never attract a bullet; A4 each bullet of a burst is decided on its own; shotgun pellets, turret bullets and the 3 extra shots of 4-direction fire are unchanged; A5 per Q43, same behaviour with mouse and stick; A6 per Q44, the aim laser still follows the raw aim, no new visual; A7 per Q45, the angle is a per-weapon data value the owner can change without code, 0 turns the assist off for that weapon; A8 builds; existing tests pass, only expectations this change breaks are updated; A9 No capture and no stress run in this task unless the owner asks: he judges the feel in game himself; the final report lists what to try and says plainly that the feel is unverified until he has played it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read burst-rifle (projectile, SpreadDeg, Range, pierce, 14 uu collision and Muzzle capsule), shotgun (pellet fan, damages barrels), turret (already locks the nearest enemy), miner (aim), aim-indicator (laser on the raw aim, stops at first robot or block), barrel (has health, shots damage it), gdd-pillars. No conflict. P2 (manual aim) holds because the assist is a few degrees and never overrides a shot already on a robot or a barrel; P3 holds because barrels are never pulled toward. Not specified by the request and decided by Q39 to Q45: weapons affected, max angle, target selection, the meaning of "va toucher", mouse versus stick, laser feedback, player option. miner edited at confirmation. Milestone: not set by the GDD; pulled into current work by the request.
- confirmed_by_owner: yes (2026-11-06, "Je valide le choix, mais ne lance pas la tâche")
- milestone: M2
- next_step: task-463-01-aim-assist
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-06 (owner "Je valide le choix, mais ne lance pas la tâche"): every default below is accepted; values provisional, owner 2026-11-06.
- Q39 Armes concernées : Default: le fusil à rafales seulement, chaque balle de la rafale décidée à part, tir principal seulement (pas les 3 tirs en plus du « tir 4 directions »). Pas le fusil à pompe (sa gerbe de plombs reste régulière), pas les tourelles (elles visent déjà seules). L'angle est une valeur par arme dans la table des stats (0 = pas d'assistance), pour l'activer plus tard sur une autre arme sans code. Autre choix : le fusil à pompe aussi (toute la gerbe déviée d'un bloc).
- Q40 Déviation maximale : Default: 5 degrés. Autres choix : 3 degrés (très discret) ou 8 degrés (bien visible).
- Q41 Choix de la cible : Default: seulement les robots, à portée de l'arme, sans bloc de roche entre le bout du fusil et le robot, à 5 degrés au plus de la direction du tir ; parmi eux celui qui demande la plus petite correction ; la balle part droit vers la position actuelle du robot (pas d'anticipation de son mouvement). Les barils ne sont jamais des cibles de l'assistance (les faire sauter reste ton choix). Autre choix : les barils attirent aussi les balles.
- Q42 « On voit qu'on va toucher » : Default: on regarde le tir tel qu'il part (dispersion comprise), avec la vraie taille de la balle, jusqu'à la portée : s'il touche d'abord un robot ou un baril, aucune déviation. S'il touche d'abord un bloc de roche ou rien, l'assistance peut le dévier vers un robot visible. Autre choix : on prend la ligne de visée (le laser) au lieu du tir réel.
- Q43 Souris et manette : Default: même assistance pour les deux. Autre choix : manette seulement (la souris vise déjà précisément).
- Q44 Retour visuel : Default: aucun ; le laser de visée continue de montrer la visée brute, seule la balle dévie. Autre choix : le laser se plie vers la cible choisie.
- Q45 Réglage joueur : Default: toujours active, pas d'option dans les menus ; seul le réglage par arme dans les données. Autre choix : une case « Aide à la visée » dans les options.

## Subtasks
1. task-463-01-aim-assist (vault/reference/tasks/task-463-aim-assist/task-463-01-aim-assist.md) — cpp: Burst rifle bullets get the light aim assist decided in the global node; other shots unchanged.
2. task-463-02-doc (vault/reference/tasks/task-463-aim-assist/task-463-02-doc.md) — doc: Record the aim assist and its data value.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-463-01-aim-assist]] — step 1, cpp.
- [[task-463-02-doc]] — step 2, doc.
