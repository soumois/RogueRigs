---
id: task-465-05-capture
description: "Capture: Before/after capture of a fight with burst rifle, turret and shotgun impacts, enemy shots and the Shooter present, sent to the owner"
parent: task-465-shot-impact-burst
kind: task
status: done
---
# Player shot impact burst: capture

- goal: Before/after capture of a fight with burst rifle, turret and shotgun impacts, enemy shots and the Shooter present, sent to the owner
- agent: capture
- depends_on: task-465-04-doc
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png
- expected_output: see done_when
- done_when: Frames sent to the owner showing: C1 burst rifle impacts green cartoon burst; C2 turret impacts green burst sized to the turret; C3 shotgun impacts white burst with warm accents; C4 enemy shot impacts unchanged; C5 robots, damage numbers and the Shooter readable during a volley; C6 the same fight before the change.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-465
- usage: included in the executor run of task-465
- runs: 1
- milestone: M2
- started: 2026-11-06T09:49:03Z
- waiting_for: none
- result: Done, 4 launches: same fight before and after at full speed (task465_05_before/after: rifle, shotgun, 2 turrets, 2 Shooters, Heavy, 8 Chasers) and in slow motion 0.25 (task465_05_slow_before/after: rifle, shotgun, turrets, Shooter, 2 Heavies, 2 Chasers); before runs set the three shot Blueprints back to the old sparks for the run only, files restored from git. C1 seen: green burst stars on the Heavies at rifle hits (task465_05_compare_detail.png after_s08). C2 seen: small green burst on a Chaser at a turret hit (after_s07, also task465_03_check/turret.png). C3 seen in task465_03_shotgun2/crop.png: white bursts with gold spikes at pellet hits; not singled out in the 05 fight frames (mixed with rifle hits on the same Heavy). C4 seen: red Shooter shots and targeting lasers as before, no burst where they reach the miner (robot shots have no impact system, code unchanged). C5 seen: Shooter green body and red eye stand out, robots and 6/10/15 damage numbers readable during volleys; chaser and heavy death explosions dominate more than the bursts. C6 seen: before frames of both fights. Sheets for the owner: Saved/Captures/task465_05_compare_fight.png, task465_05_compare_detail.png.

## Relations
- [[task-465-shot-impact-burst]] — parent global task.
