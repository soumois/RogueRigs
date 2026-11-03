---
id: task-422-02-laser-range-fade
description: "Cpp: ARRShooter laser: range, rock stop, fading Plane with M_AimLaser"
parent: task-422-shooter-laser-range-fade
kind: task
status: todo
---
# Shooter targeting laser: maximum range and fade-out: cpp

- goal: Change the ARRShooter targeting laser per the decided answers (request and Questions of the global): length = min(maximum range, first rock block along the aim line from the muzzle), no stop at the miner; LaserBeam drawn as the engine Plane with a dynamic instance of M_AimLaser set at BeginPlay (pattern of URRAimIndicatorComponent), red Color linear (1, 0, 0) with EmissiveGain 2 (MI_GB_Laser look), Width LaserThickness 4 uu, CoreFrac 1, GlowOpacity 0, TipFadeFrac 0.15, as EditDefaultsOnly defaults; same game rules (aim, sight, range, screen, shot, cadence).
- agent: cpp
- depends_on: task-422-01-capture-before
- inputs: task-422-shooter-laser-range-fade (vault/reference/tasks/task-422-shooter-laser-range-fade.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md), sys-enemies-shooter (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shooter.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), bp-aim-laser-material (vault/reference/tech/blueprints/fx/bp-aim-laser-material.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), Source/RogueRigs/Enemies/RRShooter.h, Source/RogueRigs/Enemies/RRShooter.cpp, Source/RogueRigs/AimIndicator/RRAimIndicatorComponent.cpp, Source/RogueRigs/Tests/RRShooterTests.cpp
- expected_output: Commit task-422-02-laser-range-fade with RRShooter.h/.cpp and the updated RRShooterTests.cpp; change summary (properties, defaults, length rule, how BP_Shooter Cube/MI_GB_Laser are overridden at runtime) for the doc subtask.
- done_when: Build succeeds; the RRShooter automation tests pass, the laser length check now expects min(AttackRange, clear distance) along the aim line; GetLaserLength and the LaserBeam scale follow that rule; the material instance gets TipFadeFrac 0.15; no other test expectation changed.
- editor_state: Unreal editor closed (C++ build)
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-422-shooter-laser-range-fade]] — parent global task.
