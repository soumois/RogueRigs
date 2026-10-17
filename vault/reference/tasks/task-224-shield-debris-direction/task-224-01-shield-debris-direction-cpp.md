---
id: task-224-01-shield-debris-direction-cpp
description: "cpp: ARRShieldChaser GetShieldDebrisDirection (breaking hit turned 20 degrees toward the shield side unless from behind); debug break records a miner hit."
parent: task-224-shield-debris-direction
kind: task
status: done
---
# Shield debris direction (C++)

- goal: a BlueprintPure direction the Blueprint feeds to NS_ShieldDebris as User.HitDirection.
- agent: agent-cpp
- depends_on: none
- inputs: `Source/RogueRigs/Enemies/RREnemy.h/.cpp` (RecordHitDirection, GetLastHitDirection, LaunchDebris: how the robot debris direction is chosen), `Source/RogueRigs/Enemies/RRShieldChaser.h/.cpp`, `Source/RogueRigs/Debug/RRVisualCheckCommands.cpp` (rr.Debug.BreakShields), `Art/Source/shield-chaser/debris/shield_debris_pieces.json` (fragment offsets: shield side); `vault/reference/tasks/task-224-shield-debris-direction.md` (request and definitions), `vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shield-chaser.md`, `vault/reference/tech/systems/core/sys-cheats.md`, `vault/reference/agents/agent-cpp.md`.
- method:
  - Check the order: is the direction of the hit that breaks the shield already recorded when the health set fires OnShieldBroken? If not, make it so (record before the effect is applied, or store the direction at the break), without changing the robot death debris.
  - `ARRShieldChaser`: store the break direction when the shield breaks; `UFUNCTION(BlueprintPure) FVector GetShieldDebrisDirection() const`: base = that horizontal direction, else the robot backward; if dot(base, actor forward) > 0 (hit from behind) return base; else rotate base about Z by `ShieldDebrisSideAngleDeg` (EditDefaultsOnly, default 20) toward the shield side, `ShieldSide` (EditDefaultsOnly, -1 = robot left, default from the json: negative Unreal Y offsets, so left). Name the rotation sign in a comment with a worked example (hit straight from the front of a robot facing +X).
  - `rr.Debug.BreakShields`: record, for each chaser, a hit direction from the first living miner toward the chaser (as if the miner shot it) before draining the shield.
  - Build with the editor closed; update the two doc nodes; no new test unless an existing one breaks.
- done_when: build succeeds; doc nodes updated; commit by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 30, cache write 42541, cache read 396785, output 2475
- runs: 1
- milestone: M1
- result: Commit dd939326: the breaking hit direction is already recorded before the effect (projectile and explosion paths); HandleShieldBroken now freezes it in ShieldBreakDirection (cleared on reuse); BlueprintPure GetShieldDebrisDirection: base = ShieldBreakDirection else robot backward; from behind (dot > 0) unchanged; else yaw = -ShieldSide x ShieldDebrisSideAngleDeg (20, ShieldSide -1 = left): hit from the front of a +X robot gives (-0.94, -0.34, 0); rr.Debug.BreakShields records a miner-to-chaser hit before draining. Build exit 0; docs sys-enemies-shield-chaser and sys-cheats.

## Relations
- [[task-224-shield-debris-direction]] — parent global task.
