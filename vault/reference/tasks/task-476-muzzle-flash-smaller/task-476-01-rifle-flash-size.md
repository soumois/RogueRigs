---
id: task-476-01-rifle-flash-size
description: "Bp: The burst rifle green muzzle flash is shown about 30 % smaller, look unchanged; turret and shotgun flashes keep their size."
parent: task-476-muzzle-flash-smaller
kind: task
status: done
---
# Player muzzle flash a little smaller: bp

- goal: The burst rifle green muzzle flash is shown about 30 % smaller, look unchanged; turret and shotgun flashes keep their size.
- agent: bp
- depends_on: none
- inputs: burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), task-464-player-shot-colour (vault/reference/tasks/task-464-player-shot-colour.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: see done_when
- done_when: A1 to A5 hold in the editor and a -game frame; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-476
- usage: included in the executor run of task-476
- runs: 1
- milestone: M2
- started: 2026-11-06T19:26:30Z
- waiting_for: none
- result: Done (af6e7912). NS_MuzzleFlash (rifle) every size x0.7: Core streak 11.2 x 210-252 uu, shape 119-140 uu, core 35-42 uu; Sparks 5.6-8.4 x 35-63 uu, speed 280-490 (was 400-700); shapes, colours, M_MuzzleFlashGreen, timing and counts unchanged; compile UpToDate. Turret compensation: Niagara sprite sizes do not follow the component scale, so BP_Turret OnTurretFired now spawns NS_MuzzleFlashTurret, a copy of the task-464 NS_MuzzleFlash at the old sizes, still relative scale 0.6; BP compiled and saved (set_pin_value alone left the BP clean: dirtied by adding and removing a temp function graph). Shotgun NS_MuzzleFlashShotgun untouched. Seen in -game slomo frames Saved/Captures/task476_after (rifle star smaller, turret star same as task476_before, shotgun white-gold). No gameplay value or C++ changed; no new log error.

## Relations
- [[task-476-muzzle-flash-smaller]] — parent global task.
