---
id: task-511-01-audio-system
description: "Cpp: Audio system with the event table, wired to the sound classes and to the 8 events, mix rules applied."
parent: task-511-audio-system-base-sounds
kind: task
status: done
---
# Audio system and the first 8 sound effects: cpp

- goal: Audio system with the event table, wired to the sound classes and to the 8 events, mix rules applied.
- agent: cpp
- depends_on: none
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), Content/RogueRigs/Audio/, Source/RogueRigs/Settings/RROptionsSubsystem.h
- expected_output: see done_when
- done_when: A1, A3 caps and the call sites of A2 hold with temporary test sounds (any engine sound, not kept): a -game run log shows each of the 8 events firing at its moment; build green, existing tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-511
- usage: included in the executor run of task-511
- runs: 1
- milestone: M2
- started: 2026-11-12T21:01:39Z
- waiting_for: none
- result: Done (commits db961979, a6285d70). URRAudioSubsystem + URRAudioSettings event table in DefaultGame.ini [/Script/RogueRigs.RRAudioSettings] (8 rows, Q5 values, temporary engine test sounds). Calls: ARRWeapon/ARRShotgun shot (Shot.<StatsRowName> falls back to Shot), ARRProjectile impact, ARREnemy death (not self fuse), ARRMiner damage, ARRPickup XP, URRLevelUpComponent level up, Slate hover ticker + click pre-processor for every enabled button of the game window. Turret shots, turret bullet impacts and robot bullets made silent (owner to confirm). -game logs Saved/Captures/task-511-01-audio-b..e: all 8 events play through SC_Effects; peaks Shot 4/4, Impact 4/4, XPPickup 3/3 with cap stops; hover+click in the paused level-up (Reroll) and pause menu (Resume); device volume Effects 0 -> SC_Effects 0 Music 1, Music 0 -> Effects 1, Master 0 -> both 0, restored to 100. Test click cause: down and up in one frame do not activate a UMG button; rr.Debug.Click down / up split it, New Run then starts. Tests Weapons, Shotgun, Turret, Enemies, Pickups, Progression, LevelUp, Player: 64 pass, 1 fail pre-existing RogueRigs.Weapons.ImpactSparkAsset (expects NS_ShotImpactSpark on the rifle shot, the BP uses NS_ShotImpactBurstGreen since task-465; not changed). Log-only: disabled button silence, on-screen panning. ALERT budget standard: 69 turns, 1.53 M units at this closing; cause: owner pause and resume, five -game log runs (first click test and a run-lost run had to be redone), test run and the pre-existing failure check.

## Relations
- [[task-511-audio-system-base-sounds]] — parent global task.
