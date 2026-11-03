---
id: task-422-shooter-laser-range-fade
description: "Global task: Shooter targeting laser gets a maximum range and fades out at its end, at that range or at the first rock block; before/after captures for three cases."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Shooter targeting laser: maximum range and fade-out

- request: Owner 2026-11-03, verbatim: "Shooter targeting laser: add a maximum range and a fade-out. The Shooter's targeting laser already exists. I want it to have a maximum range and to fade out at its end until it disappears. If it hits something before the maximum range (a rock block, for example), it should fade out there too. I want before/after screenshots shown in the conversation (not stored in the vault), for these cases: 1. Target far away in open terrain: the laser reaches its maximum range and fades out. 2. A rock block between the Shooter and the target: the laser fades out at the impact point. 3. Target close, well within range: the laser stays clearly readable up to the target." Captures: explicitly requested, shown in the conversation only, stored as temporary files in `Saved/Scratch/task-422-shooter-laser/` (git-ignored), never in the vault or `Art/Source/`; the before shots are taken on the current build before any change.
- goal: The Shooter laser runs from the muzzle along the aim line up to a maximum range, stops at the first rock block before it, and its end fades to zero opacity; before/after frames of the three owner cases are shown to the owner.
- acceptance: A1 (case 1, open terrain, miner at the range edge): the red laser reaches the maximum range from the muzzle and its last part fades to invisible, no hard end. A2 (case 2, rock on the aim line before the maximum range): the laser ends at the rock face and fades to zero there, nothing drawn beyond or inside the block. A3 (case 3, miner about 400 uu away): the laser is fully opaque red from the cannon tip to the miner; the fade lies only in its last part, well past the miner. A4 game rules unchanged: aim only with line of sight, in range and on screen; same WindUp, cadence, shot and charge glow; laser hidden outside Aim. A5 same red colour and about the same thickness (4 uu) as before. A6 before and after frames of the three cases shown to the owner, stored only under Saved/Scratch/task-422-shooter-laser/ (temporary), none committed. A7 the C++ build succeeds and the Shooter automation tests pass, the laser length expectation updated to the new rule.
- gdd_check: enemy-shooter (approved): red targeting laser before each shot, shown only with line of sight; shots stop on rock and never hit other robots; stats in EnemyStats.csv row Shooter (AttackRange = shot reach: lifetime AttackRange / ProjectileSpeed). No laser range or fade value exists in the GDD: open, asked as Q2 and Q3. As built (sys-enemies-shooter, RRShooter.cpp GetLaserLength): the laser ends at the miner (aim point), clipped at the first rock block (ARRArenaGrid::GetClearDistance); the Shooter aims only within AttackRange with line of sight, so it never aims at a miner beyond the range (case 1 is a miner near the range edge) and never aims through a rock fully between it and the miner. Conflict for case 2 as worded: with a rock between the Shooter and the miner there is no aim, hence no laser (Q4). Reusable pattern: the miner aim laser (aim-indicator, sys-aim-indicator, bp-aim-laser-material): engine Plane plus a dynamic instance of M_AimLaser, last TipFadeFrac 0.15 fading to the tip. Colour: red, color-code (MI_GB_Laser vector Color linear (2, 0, 0, 1), bp-drone). Rules versus rendering (orchestrator-lessons): visual change only, no rule change unless the owner says so. Milestone: M3 (juice), as aim-indicator. Existing test RRShooterTests "laser length = muzzle to aim point in open sight" breaks under Q1 default and is updated, no new test.
- confirmed_by_owner: no (plan and questions handed back 2026-11-03)
- milestone: M3
- next_step: task-422-01-capture-before
- started: open
- waiting_for: owner answers to Q1 to Q4 (laser through the miner, maximum range, fade shape, rock case staging)
- result: open

## Questions
Asked 2026-11-03, each with a default; open until the owner answers.
- Q1 Laser past the miner. Today the laser ends at the miner. Default: it no longer stops at the miner; it runs along the aim line through the miner and other robots (shots never hit robots) up to the maximum range, and stops only at the first rock block. Otherwise it keeps ending at the miner and the fade shows only at a rock or when the miner stands beyond the maximum range.
- Q2 Maximum range (open in the GDD). Default: the shot reach, `AttackRange` of `EnemyStats.csv` row `Shooter`, no new value: the laser shows exactly how far a shot flies. Note: the Shooter aims only within that range, so in case 1 the miner stands near the range edge, inside the fading end. Alternative: a separate longer value, for example 1200 uu like the miner aim laser.
- Q3 Fade (open). Default: the last 15 % of the visible laser fades linearly from full opacity to zero, at the range end or at the rock, same shape as the miner aim laser (provisional); otherwise the current look kept: red, about 4 uu, solid, no glow added.
- Q4 Case 2 conflict. The GDD says the laser is shown only with line of sight: with a rock fully between the Shooter and the miner the Shooter does not aim, so no laser at all. Default: keep the rule; case 2 is staged with the rock block on the aim line just behind the miner, within range, so the laser passes the miner and fades at the rock face. Alternative: change the rule so the Shooter also aims at a miner hidden behind rock (gameplay change, a separate task).

## Subtasks
1. task-422-01-capture-before (vault/reference/tasks/task-422-shooter-laser-range-fade/task-422-01-capture-before.md) — capture: before frames of the three cases on the current build
2. task-422-02-laser-range-fade (vault/reference/tasks/task-422-shooter-laser-range-fade/task-422-02-laser-range-fade.md) — cpp: ARRShooter laser: range, rock stop, fading Plane with M_AimLaser
3. task-422-03-capture-after (vault/reference/tasks/task-422-shooter-laser-range-fade/task-422-03-capture-after.md) — capture: after frames of the three cases, checklist against the before frames
4. task-422-04-doc (vault/reference/tasks/task-422-shooter-laser-range-fade/task-422-04-doc.md) — doc: shooter laser docs: system, BP_Shooter runtime override, laser material use, tests

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-422-01-capture-before]] — step 1, capture.
- [[task-422-02-laser-range-fade]] — step 2, cpp.
- [[task-422-03-capture-after]] — step 3, capture.
- [[task-422-04-doc]] — step 4, doc.
- [[enemy-shooter]] — the robot whose laser it changes; laser range and fade decision.
- [[sys-enemies-shooter]] — the laser code as built.
- [[aim-indicator]] — the miner aim laser whose fade shape is reused.
