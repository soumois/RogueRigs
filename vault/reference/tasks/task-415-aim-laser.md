---
id: task-415-aim-laser
description: "Global task: replace the ground aim chevrons of the miner by a thin white-gold aim laser from the rifle muzzle, as in the owner video."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Aim laser replaces the white aim chevrons

- request: Owner, 2026-11-02, verbatim (French, relayed by the main session): "analyse de 00:03 à 00:05, ils ont une sorte de laser pour savoir où les joueurs visent. J'aimerais remplacer les fléches blanche par ça". Video C:\Users\soumo\Downloads\snapgram.io_b15ni7cfbcck.mp4 (co-op top-down shooter); frames 00:03 to 00:05 saved in Art/Source/aim-indicator/laser-reference-2026-11-02/ (f01 to f08 at 4 fps, sheet.png). Main session reading of the reference: each player has a thin, straight, bright laser line, one colour per player (yellow, white-blue, purple), from the weapon muzzle along the aim, much longer than the character, with a slight glow, visible through muzzle flashes and explosions; hex-bracket reticle near the aim point. Owner correction 2026-11-02 (relayed): the laser does not go through the floor; it is a line along/above the floor from the muzzle to the aim point. Planner reading: "les flèches blanches" are the white-gold ground chevrons of [[aim-indicator]] (tasks 328 to 331); in the frames the hex brackets stay at the top of the screen while the yellow laser runs past them, so they are the video player cursor, not the laser end. Owner answer 2026-11-02 (French, verbatim, relayed by the main session): "ça s'arrête au premier robot où le premier mur" = Q3: the laser stops at the first robot or the first wall (rock block) on the aim line, whichever is nearer, still capped by the maximum length; it does not pass over robots. Q1, Q2 and Q4 to Q8 not answered yet. Owner answer 2026-11-02 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8 accepted.
- goal: The miner shows a thin white-gold laser line from the rifle Muzzle along his aim, above the floor, stopping at the first robot or rock block; the ground chevrons are gone. Values per the Questions.
- acceptance: M_AimLaser exists in Content/RogueRigs/FX/ and compiles; C++ builds; in game no chevron is drawn; a straight white-gold line (3 uu core, soft glow about 8 uu each side, last 15 % fading to the tip) starts at the shown rifle Muzzle, runs horizontally at Muzzle height along the aim direction, never cut by or sunk into the floor, ends at the first robot or the first block on the aim line, whichever is nearer, or at 1200 uu; it follows mouse and stick aim, hidden at death; no reticle; turrets show none; M_AimChevrons and the chevron decal code removed; BP_Miner not opened; existing tests pass, no new test; capture checklist C1 to C7 seen; doc nodes updated; gdd aim-indicator, miner, color-code and readability-rules updated (edits on confirmation); lint clean; committed by path.
- gdd_check: Read 2026-11-02: [[aim-indicator]] (provisional, M3): today white-gold ground chevrons, fixed 416 uu, stop at rock, no glow, and the line "Not a laser: red is enemy fire and the shooter targeting laser, and the owner finds a laser too classic": CONFLICT with the request (Q1). [[readability-rules]] (rule): red = enemy fire incl. targeting lasers, so the player laser must not be red; the reference uses one colour per player, Rogue Rigs is solo (Q2). [[color-code]]: player shots and mining laser white-gold. [[miner]]: aims with mouse or stick; cannot shoot through rock ([[destruction-rules]]). Open values the work needs: length and stop rule (Q3, stop rule answered 2026-11-02: first robot or first wall), height (Q4), thickness and glow (Q5; the chevrons had "no glow halo", the reference has a slight glow), reticle (Q6), turrets (Q7), fate of the chevron code and asset (Q8). Milestone: aim feedback is M3 (juice) work, as the chevrons were; done now on the owner request. Tech read: [[sys-aim-indicator]] (URRAimIndicatorComponent, a decal with M_AimChevrons, GetClearDistance stop); the shooter targeting laser is a scaled engine mesh with a material driven by C++ (RRShooter LaserBeam), the pattern reused here; the shown rifle Muzzle is the shot origin since task-412 ([[sys-weapons]]); no test references AimIndicator; no uncommitted BP_Miner edit. Visible change: capture subtask planned (owner rule 2026-10-16); no representative slice needed beyond it (a single effect, no expansion).
- confirmed_by_owner: yes (2026-11-02, "Ok, tout par défaut")
- milestone: M3
- next_step: task-415-01-mat-aim-laser
- started: 2026-11-02T17:29:01Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-02 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-02.
- Q1 GDD conflict: the aim-indicator node says "not a laser" (red is the enemy targeting laser, and you found a laser too classic on 2026-10-27). Default: your request replaces that choice; the node is rewritten for the laser, the laser is never red; done now although aim feedback is M3 work. Or: keep the chevrons.
- Q2 Colour: the video has one colour per player; Rogue Rigs is solo. Default: one colour, white-gold sRGB (255, 236, 170) like the chevrons and the player shots. Or: another colour that is not red, orange, acid green or steel blue.
- Q3 Length: stop rule answered 2026-11-02 ("ça s'arrête au premier robot où le premier mur"): the laser ends at the first robot or the first wall (rock block) on the aim line, whichever is nearer. Cap, Default: at most 1200 uu (about 18 cells, the short half of the screen, long like the video). Or: another maximum, or 416 uu like the chevrons.
- Q4 Height: Default: a horizontal line at the rifle Muzzle height, above the floor and parallel to it, never entering it, starting at the Muzzle tip along the aim. Or: drawn flat on the floor (projected like the chevrons), starting under the Muzzle.
- Q5 Thickness and look: Default: bright core 3 uu wide with a soft glow of the same colour about 8 uu each side at low opacity, same brightness along the line, the last 15 % fading to zero at the tip. Or: a sharp line without glow (as the chevrons), or another thickness.
- Q6 Reticle: in the video the hex brackets are the player cursor (the yellow laser runs past them), not the laser end. Default: no reticle, only the laser. Or: a small white-gold bracket reticle at the laser end (added to this task).
- Q7 Turrets: Default: miner only; turrets have no laser (they aim by themselves). Or: turrets too.
- Q8 Old chevrons: Default: removed, the chevron decal code and the M_AimChevrons material are deleted. Or: keep the material on disk unused.

## Subtasks
1. task-415-01-mat-aim-laser (vault/reference/tasks/task-415-aim-laser/task-415-01-mat-aim-laser.md) — bp: create the M_AimLaser material (white-gold core, soft glow, tip fade) in Content/RogueRigs/FX/
2. task-415-02-aim-laser-component (vault/reference/tasks/task-415-aim-laser/task-415-02-aim-laser-component.md) — cpp: replace the chevron decal of the miner by the aim laser component (Muzzle origin, stop at rock, 1200 uu max)
3. task-415-03-delete-aim-chevrons (vault/reference/tasks/task-415-aim-laser/task-415-03-delete-aim-chevrons.md) — bp: delete the unused M_AimChevrons material
4. task-415-04-capture-aim-laser (vault/reference/tasks/task-415-aim-laser/task-415-04-capture-aim-laser.md) — capture: capture the aim laser in a -game run and check C1 to C7
5. task-415-05-doc-aim-laser (vault/reference/tasks/task-415-aim-laser/task-415-05-doc-aim-laser.md) — doc: rewrite sys-aim-indicator for the laser; update sys-player

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-415-01-mat-aim-laser]] — step 1, bp.
- [[task-415-02-aim-laser-component]] — step 2, cpp.
- [[task-415-03-delete-aim-chevrons]] — step 3, bp.
- [[task-415-04-capture-aim-laser]] — step 4, capture.
- [[task-415-05-doc-aim-laser]] — step 5, doc.
- [[aim-indicator]] — gdd node rewritten on confirmation.
- [[sys-aim-indicator]] — the chevron system this task replaces.
- [[task-328-aim-chevrons]] — the chevrons being replaced.
