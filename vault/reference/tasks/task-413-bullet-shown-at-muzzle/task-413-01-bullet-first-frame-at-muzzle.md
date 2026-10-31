---
id: task-413-01-bullet-first-frame-at-muzzle
description: "Cpp: measure in a -game run where the rifle bullet is first drawn versus the Muzzle, then fix it to draw at the Muzzle."
parent: task-413-bullet-shown-at-muzzle
kind: task
status: done
---
# Bullet shown at the muzzle on its first frame: cpp

- goal: Per the global Questions as decided. (a) Measure in a -game run (temporary log or debug command, removed or left behind a cvar off by default, agent states which): for several rifle bursts, the hand weapon Muzzle world location at fire time, the projectile location right after `SweepTo`, and the ShotMesh world location on the first frame it is rendered (e.g. first `Tick` after spawn or `WasRecentlyRendered`), plus the frame time; write the numbers. (b) From the numbers name the cause (first-frame travel before the first draw, Muzzle read one frame late against the hand animation, ShotMesh offset, other) and fix it in C++ so the first drawn ShotMesh location is within 10 uu of the Muzzle, keeping speed, range, capsule sizing, the Q3 sweep, rock stop and robot hits as today; the fix lives in the shared projectile or ARRWeapon path so shotgun pellets get it too; turret bullets and robot shots unchanged. (c) Re-measure after the fix and write the numbers. Update only the existing automation test expectations the change breaks; add no test. Never touch or stage any .uasset (BP_BurstRifle and ABP_Miner are owner uncommitted edits).
- agent: cpp
- depends_on: none
- inputs: task-413-bullet-shown-at-muzzle (vault/reference/tasks/task-413-bullet-shown-at-muzzle.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-weapons-shotgun (vault/reference/tech/systems/combat/sys-weapons/sys-weapons-shotgun.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-projectile (vault/reference/tech/blueprints/player/bp-projectile.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), Source/RogueRigs/Projectiles/RRProjectile.h, Source/RogueRigs/Projectiles/RRProjectile.cpp, Source/RogueRigs/Weapons/RRWeapon.h, Source/RogueRigs/Weapons/RRWeapon.cpp, Source/RogueRigs/Weapons/RRShotgun.cpp, Source/RogueRigs/Tests/RRWeaponTests.cpp, `C:\Users\soumo\Videos\2026-10-31 14-51-08.mkv` (owner video, 00:03 and 00:04; frames: WinGet ffmpeg `-vf "fps=4,scale=640:-1,tile=4x3" -frames:v 1`)
- expected_output: Edited C++ files; one commit `task-413-01-bullet-first-frame-at-muzzle: <summary>` by path; result with the measured distances before and after; a change summary for the doc step (cause, fix, cvar or log left, tests touched, commit hash, changed paths).
- done_when: Development Editor build succeeds with no new warning in the edited files; the result gives measured Muzzle-to-first-drawn-bullet distances before and after from a real -game run, after <= 10 uu for every bullet of at least 3 bursts; RogueRigs.Weapons and RogueRigs.Shotgun tests pass headless with only broken expectations updated; code review shows speed, range, capsule sizing, sweep and hit paths unchanged and turret bullets and robot shots untouched; no .uasset staged; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: 1
- milestone: M2
- started: 2026-10-31T21:58:10Z
- waiting_for: none
- result: Commit 148676a4. Cause: bullet spawned in the world tick moves one frame (3000 x dt) before first draw. Fix: ARRProjectile::HoldFirstMove (sim off for the spawn frame, TG_PostUpdateWork tick re-enables, life span +1 frame) called from ARRWeapon::SpawnShot for swept Muzzle shots. Measured -game ~100 fps running miner: before 26-34 uu from Muzzle per bullet (first burst 56/172/34 on hitches), after 0.0-0.8 uu for shots 1-40. Build clean, RogueRigs.Weapons+Shotgun 19/19 pass, no test changed. Cvar rr.Debug.ShotFirstFrame left (off). Cost counted under the resumed cpp agent first description (runs=1; ALERT likely: turn limit 60 reached then resumed). Unverified: shotgun in game, 60 fps, visual, motion blur.

## Relations
- [[task-413-bullet-shown-at-muzzle]] — parent global task.
