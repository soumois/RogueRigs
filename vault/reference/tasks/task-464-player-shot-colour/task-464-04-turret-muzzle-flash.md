---
id: task-464-04-turret-muzzle-flash
description: "Bp: Turrets show the green muzzle flash of task-464-03 at their barrel on each shot, sized to the turret"
parent: task-464-player-shot-colour
kind: task
status: done
---
# Player shot colour: bp

- goal: Turrets show the green muzzle flash of task-464-03 at their barrel on each shot, sized to the turret
- agent: bp
- depends_on: task-464-03-muzzle-flash-green
- inputs: turret (vault/reference/gdd/weapons/turret.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), task-464-03-muzzle-flash-green (vault/reference/tasks/task-464-player-shot-colour/task-464-03-muzzle-flash-green.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: see done_when
- done_when: In a -game run each turret shot shows the green muzzle flash at the end of the turret barrel, sized to the turret, same look as the rifle flash; turret behaviour and values unchanged; robots still have no flash.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-464
- usage: included in the executor run of task-464
- runs: 1
- milestone: M2
- started: 2026-11-06T09:13:51Z
- waiting_for: none
- result: Done. BP_Turret EventGraph: OnTurretFired(Origin, Direction) now calls Niagara SpawnSystemAttached(NS_MuzzleFlash, Muzzle, KeepRelativeOffset, bAutoDestroy true) then SetRelativeScale3D 0.6 on the returned component (0.6 = turret mesh scale 0.6148, provisional); the old chain (MuzzleFlash sphere visible, Delay 0.05, hidden) was deleted, the MuzzleFlash sphere component (MI_GB_Shot) stays in the Blueprint, hidden, unused. Compiled (only the pre-existing BP_TurretBullet CollisionSphere fixup warning in the log), read back, saved. Seen in a slow-motion -game frame (Saved/Captures/task464_04_check/flash.png): green flash with pale core spikes at the end of the turret barrel, along the barrel, no white sphere; its blades reach past the turret width. Turret C++, data and robots untouched.

## Relations
- [[task-464-player-shot-colour]] — parent global task.
