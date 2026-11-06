---
id: sys-weapons-aim-assist
description: "Burst rifle light aim assist (task-463): when a bullet turns toward a robot, per-weapon AimAssistDeg, test. Open to tune or change it."
parent: sys-weapons
kind: doc
---
# Weapons — light aim assist

- responsibility: turn a miner bullet that would miss by a few degrees toward a robot, without stealing a shot already on a robot or a barrel (task-463, owner 2026-11-06, Q39 to Q45 defaults, provisional; request and decisions in [[task-463-aim-assist]]).
- main_classes: `Weapons/RRAimAssist.h/.cpp` (namespace `RRAimAssist`: `FShotQuery`, `EFirstHit` {Nothing, Robot, Barrel, Block}, `PredictFirstHit`, `FindAssistedDirection`); `ARRWeapon::GetAimAssistedRotation(Placement, ShotRotation, OutTarget)`; commit d7210d03.
- rules (per bullet, in `ARRWeapon::FireOnce` after the random spread, so each bullet of a burst is decided on its own):
  1. `AimAssistDeg` <= 0: the shot is unchanged (no query at all).
  2. The shot as fired is predicted with the real projectile: the default object's capsule radius, the placement capsule half height, the capsule's own collision responses on the Projectile channel, swept from the miner axis to the Muzzle and then up to `Range`; it ignores the weapon, its owner and the miner. First a living robot hit capsule or a barrel cell (`ARRArenaGrid::GetCellFromHit`): unchanged. First another block (rock, ore, wall) or nothing: the assist may act.
  3. Candidates: living robots (not dead, no pending death) found by a sphere overlap of radius `Range` on the Pawn object type, hit capsule centre within `Range` and within `AimAssistDeg` of the shot direction (flat, 2D). Barrels are never candidates.
  4. Smallest turn first, each candidate's turned shot is predicted the same way; the first whose turned shot meets a robot first wins (a rock block or a barrel in the way, real shot width, rules it out). The bullet then flies flat straight at that robot's current position (no lead).
- not affected: shotgun pellets (`ARRShotgun::FireOnce` does not call it, row value 0), turret bullets (`ARRTurret` never copies the value, it stays 0; launcher row 0). The 3 extra shots of 4-direction fire do not exist in code yet. Mouse and stick: same rule (it reads the shot direction only). The aim laser still shows the raw aim ([[sys-aim-indicator]] unchanged). No player option.
- entry_points: `ARRWeapon::GetAimAssistedRotation`, `GetAimAssistDeg()` (BlueprintPure).
- data: `WeaponStats.csv` column `AimAssistDeg`, largest turn in degrees per weapon row, copied to `ARRWeapon::AimAssistDeg` in `ApplyStatsRow` (a CSV constant, not an attribute); 0 turns the assist off for that weapon. Rifle value: row `BurstRifle` (provisional, owner 2026-11-06, Q40).
- depends_on: [[sys-projectiles]] (capsule, ignored actors), [[sys-enemies]] (hit capsule), the arena grid (barrel cells).
- implements: [[burst-rifle]], [[miner]] (aim).
- test: `RogueRigs.Weapons.AimAssist` (spread forced to 0): near miss turned to the robot and the fired bullet hits it; shot already on the robot, robot outside the cone, beyond range, behind a rock, `AimAssistDeg` 0 and a shot on a barrel with a robot beside it are unchanged; CSV rows rifle above 0 and at most 5, shotgun and turret launcher 0.
- gotchas: cost per bullet only when the value is above 0: one or two sweeps for the straight shot, one sphere overlap, one or two sweeps per candidate in the cone. Feel unverified in game (owner judges it).

## Relations
- [[sys-weapons]] — parent hub: FireOnce and the stats row.
- [[task-463-aim-assist]] — the request and the decided values.
- [[sys-projectiles]] — projectile capsule and what a shot ignores.
- [[sys-enemies]] — robot hit capsule.
- [[sys-aim-indicator]] — the laser, left on the raw aim.
- [[burst-rifle]] — the weapon assisted.
- [[miner]] — the aim the assist corrects.
