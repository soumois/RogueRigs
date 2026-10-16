---
id: task-197-barrel-explosion-feedback
description: "Global task: barrel looks unstable during its fuse, and its explosion plays a particle burst and an orange shockwave ring on the ground."
parent: tasks-to-do-index
kind: task
status: todo
---
# Barrel explosion feedback

- request: Code Architect run 2 (`/architect-score`, then assignments/A5-goal-oriented-coding-agent/blackboard.md) recommended the barrel particle burst; owner answer: "1. Va pour le baril. Il faudrait que pendant que la zone grandit, on voit qu'il est instable. Ensuite il faudrait une partie d'explosion et aussi le cercle d'explosion. ( On pourra re utiliser pour le kamikaze)"; defaults accepted with "Ok" (owner, 2026-10-16)
- goal: during the 1.5 s fuse the barrel shakes more and more and blinks orange-white faster and faster; at the explosion a Niagara burst (flash, fireball, sparks, smoke) plays on top of the orange sphere and an orange shockwave ring grows on the ground to the explosion radius; barrel-only FX class so the kamikaze is unchanged and can reuse it later with one table line; one-click test `rr.Cheat.FuseBarrel 1`.
- acceptance: build clean; existing tests pass; `rr.Cheat.FuseBarrel 1` in PIE lights the nearest fuse, the blink custom data rises 0 to 1, the barrel explodes with `BP_ExplosionFX_Barrel` (burst active, ring progress rising); a kamikaze explosion still uses `BP_ExplosionFX`; docs updated; on-screen look listed as unverified unless seen; committed.
- gdd_check: [[barrel]] (approved, M1), [[explosions]], [[ground-telegraph]], [[readability-rules]], [[enemy-kamikaze]] read 2026-10-16. No conflict; no `open` value needed; owner rule added to [[barrel]] (provisional, 2026-10-16).
- confirmed_by_owner: open
- milestone: M1
- next_step: task-197-01-barrel-fuse-instability
- result: open

## Subtasks
1. task-197-01-barrel-fuse-instability — cpp
2. task-197-02-explosion-fx-burst-shockwave — cpp
3. task-197-03-barrel-fuse-blink-material — bp
4. task-197-04-barrel-explosion-niagara — bp
5. task-197-05-shockwave-decal-material — bp
6. task-197-06-barrel-explosion-fx-blueprint — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-197-01-barrel-fuse-instability]] — step 1, cpp.
- [[task-197-02-explosion-fx-burst-shockwave]] — step 2, cpp.
- [[task-197-03-barrel-fuse-blink-material]] — step 3, bp.
- [[task-197-04-barrel-explosion-niagara]] — step 4, bp.
- [[task-197-05-shockwave-decal-material]] — step 5, bp.
- [[task-197-06-barrel-explosion-fx-blueprint]] — step 6, bp.
- [[barrel]] — the rule it implements.
