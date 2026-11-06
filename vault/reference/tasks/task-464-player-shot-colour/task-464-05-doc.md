---
id: task-464-05-doc
description: "Doc: Update the bp-* and sys-* nodes of the changed effects and check the gdd nodes match the result"
parent: task-464-player-shot-colour
kind: task
status: done
---
# Player shot colour: doc

- goal: Update the bp-* and sys-* nodes of the changed effects and check the gdd nodes match the result
- agent: doc
- depends_on: task-464-04-turret-muzzle-flash
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), turret (vault/reference/gdd/weapons/turret.md)
- expected_output: see done_when
- done_when: Every bp-* or sys-* node of a changed asset states the new look; color-code, burst-rifle and turret match what is on screen; the reference image is listed; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-464
- usage: included in the executor run of task-464
- runs: 1
- milestone: M2
- started: 2026-11-06T09:16:19Z
- waiting_for: none
- result: Done (commit e6a370ca). New bp-player-laser-shot-material (M_PlayerLaserShot values), listed in blueprints-fx-index; bp-projectile and bp-turret state the green shot material and sparks and the turret flash graph; bp-muzzle-flash states M_MuzzleFlashGreen colours, the turret spawn and lists the owner reference image; bp-shot-impact-spark lists NS_ShotImpactSparkGreen; sys-weapons, sys-turret, sys-projectiles and sys-cheats (rr.Debug.AddWeapon) updated. color-code, readability-rules, burst-rifle and turret already state the new look (planner edits) and match the frames; not changed. Lint clean (bp-muzzle-flash was already over 400 words before this task).

## Relations
- [[task-464-player-shot-colour]] — parent global task.
