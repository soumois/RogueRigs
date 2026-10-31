---
id: task-412-shots-from-rifle-muzzle
description: "Global task: player shots start at the burst rifle Muzzle (horizontal position) instead of in front of the miner; height and rock rules kept."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Shots leave from the rifle muzzle

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "Ok maintenant, j'aimerais que les tirs partent du muzzle de l'arme." Context relayed: today the shot origin is the rule floor + 50 uu in front of the miner capsule (ARRWeapon::GetShotOrigin), not the visual Muzzle; the Muzzle is a component under the BP_BurstRifle root at (52.25, 0, 0), visual only, counter-moved with rr.Debug.RightHandOffset (tasks 410, 411), getter GetMuzzle() (task-411). Plan and questions Q1 to Q6 handed back 2026-10-31.
- goal: In game the burst rifle bullets visibly leave from the barrel tip (the Muzzle) seen from the top-down camera, and follow it when the hand animation moves the rifle; they still hit floor-standing robots and stop on rock blocks exactly as today, including when the miner stands against a rock or a robot touches him. Choices per the Questions (defaults: Muzzle X, Y at floor + 50 uu, parallel to the aim, segment check miner centre to Muzzle, shotgun pellets from the same Muzzle, capture, no Blueprint change).
- acceptance: C++ builds; with the miner holding BP_BurstRifle, every rifle bullet spawns at the Muzzle world X, Y (within 1 uu, read at fire time, so the right hand offset counter-move and the hand animation are followed) and at Z = floor under the miner + ShotHeight 50 (Q1), yaw = aim yaw + spread (Q2); a rock block on the segment from the miner axis to the Muzzle at shot height stops the shot there (no bullet appears beyond the wall), a robot on that segment is damaged as a normal hit counting toward pierce (Q3); shotgun pellets use the same origin while a rifle is held, else today's point (Q4); a weapon without an owning miner still uses its own Muzzle; turrets and robot shots unchanged; aim indicator unchanged; existing automation tests whose expectations the change breaks (RogueRigs.Weapons.MinerStartingWeapon shot position, shotgun origin tests if any) updated, no new test; capture checklist C1, C2 seen (Q5); doc nodes updated; gdd burst-rifle and shotgun lines updated (edits on confirmation); lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[weapons-index]] (approved rule: "No shot passes over a rock block"), [[burst-rifle]] (approved; provisional line "shot origin and height unchanged (provisional, owner 2026-10-30, tasks 400 and 401)": the request supersedes it, edit on confirmation), [[shotgun]] (look line: pellets leave from the same point as every weapon shot, owner task-400 Q2 "les autres tirs partiront du même endroit"), [[miner]] ("Cannot shoot through or over rock blocks"), [[turret]] (unchanged). Tech read: [[sys-weapons]] (GetShotOrigin = floor + ShotHeight 50, moved along the aim by capsule radius + ShotForwardOffset 20; without a miner: the Muzzle; virtual), [[bp-burst-rifle]] (Muzzle under the root at (52.25, 0, 0), visual only). Code read: RRWeapon.cpp GetShotOrigin and FireOnce (flat projectile, aim yaw + spread), RRShotgun.cpp FireOnce (same GetShotOrigin), RRTurret.cpp GetShotOrigin (precedent: Muzzle X, Y at actor Z + ShotHeight), RRMiner.cpp aim (AimDirection from the actor to the mouse point on a plane at the feet; stick gives a direction only), RRArenaGrid BlockHeight 100, capsules: chaser half 62, shooter 88, kamikaze 96.8 hovering, heavy 124. Conflict: the Muzzle in the hand is at chest height, unmeasured (estimated 100 to 130 uu above the floor, above the 100 uu blocks): taking its real height would let shots fly over rocks, against the approved rule; default Q1 keeps the height. Risk: the Muzzle sits about 60 to 90 uu ahead of the miner axis (75 cm rifle), so it can be inside or past a rock or a robot when the miner hugs it (Q3). Open values: none needed. Owner working tree 2026-10-31: BP_BurstRifle.uasset and ABP_Miner.uasset modified, not committed (Saved/Logs/RogueRigs.log 21:00 shows RifleMesh scale 1.5); the cpp step must not touch or commit them (Q6). Milestone M2.
- confirmed_by_owner: no (plan and questions handed back 2026-10-31)
- milestone: M2
- next_step: task-412-01-shot-origin-muzzle
- started: open
- waiting_for: owner confirmation of the plan and answers to Q1-Q6
- result: open

## Questions
Asked 2026-10-31, each with a default; open until the owner answers.
- Q1 Shot height: Default: the bullet starts at the Muzzle position seen from the camera (X, Y) but keeps the height floor + 50 uu, as the turrets already do; rocks and robots are hit exactly as today. Or: the real Muzzle height (chest height in the hand, about 100 to 130 uu, unmeasured; blocks are 100 uu tall, so shots would fly over rocks, against the rule "no shot passes over a rock block", and could pass over small robots).
- Q2 Direction: Default: parallel to the aim, as today and as the ground chevrons, starting from the Muzzle (the line is shifted sideways by the Muzzle offset, a few cm). Or: from the Muzzle toward the mouse cursor point (with the stick, parallel).
- Q3 Muzzle past a rock or a robot (miner against a wall, robot touching him): Default: the shot is checked from the miner centre to the Muzzle at shot height first: a rock on that segment stops it (no shot through the wall), a robot on it is hit (as today at point-blank), then it flies on from the Muzzle. Or: no check, the shot simply starts at the Muzzle (it may then start behind a wall or skip a robot pressed against the miner).
- Q4 Shotgun: Default: its 7 pellets leave from the burst rifle Muzzle too (your task-400 words: "les autres tirs partiront du même endroit"); without a held rifle, today's point. Or: the shotgun keeps today's point. Turrets (already from their canon Muzzle) and robot shots stay unchanged.
- Q5 Visual check: Default: a -game capture subtask: C1 bullets appear at the barrel tip seen from the camera, C2 miner against a rock, shots stop on it. Or: no capture, you check in PIE.
- Q6 Muzzle placement: Default: no Blueprint change; your uncommitted BP_BurstRifle edit (rifle scale 1.5 in the log) stays yours, you check the Muzzle sits on the barrel tip and move it by hand if not. Or: a bp subtask places the Muzzle on the SM_BurstRifle tip.

## Subtasks
1. task-412-01-shot-origin-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle/task-412-01-shot-origin-muzzle.md) — cpp: ARRWeapon::GetShotOrigin from the held rifle Muzzle X, Y at floor + 50, segment check miner to Muzzle, shotgun uses it; tests expectations
2. task-412-02-capture-shots-from-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle/task-412-02-capture-shots-from-muzzle.md) — capture: -game capture: bullets appear at the barrel tip; miner against a rock, shots stop on it
3. task-412-03-doc-shot-origin-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle/task-412-03-doc-shot-origin-muzzle.md) — doc: sys-weapons, sys-weapons-shotgun, bp-burst-rifle, sys-automation-tests: shot origin at the rifle Muzzle

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-412-01-shot-origin-muzzle]] — step 1, cpp.
- [[task-412-02-capture-shots-from-muzzle]] — step 2, capture.
- [[task-412-03-doc-shot-origin-muzzle]] — step 3, doc.
- [[burst-rifle]] — gdd line on the shot origin this task changes.
- [[shotgun]] — gdd line on where pellets leave from.
- [[sys-weapons]] — GetShotOrigin rule this task changes.
- [[bp-burst-rifle]] — the Muzzle component used as origin.
- [[task-411-right-hand-offset-rifle-stays]] — added GetMuzzle and the Muzzle counter-move.
