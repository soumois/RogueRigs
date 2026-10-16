---
id: task-197-barrel-explosion-feedback
description: "Global task: barrel looks unstable during its fuse, and its explosion plays a particle burst and an orange shockwave ring on the ground."
parent: tasks-done-index
kind: task
status: done
---
# Barrel explosion feedback

- request: Code Architect run 2 (`/architect-score`, then assignments/A5-goal-oriented-coding-agent/blackboard.md) recommended the barrel particle burst; owner answer: "1. Va pour le baril. Il faudrait que pendant que la zone grandit, on voit qu'il est instable. Ensuite il faudrait une partie d'explosion et aussi le cercle d'explosion. ( On pourra re utiliser pour le kamikaze)"; defaults accepted with "Ok" (owner, 2026-10-16)
- goal: during the 1.5 s fuse the barrel shakes more and more and blinks orange-white faster and faster; at the explosion a Niagara burst (flash, fireball, sparks, smoke) plays on top of the orange sphere and an orange shockwave ring grows on the ground to the explosion radius; barrel-only FX class so the kamikaze is unchanged and can reuse it later with one table line; no cheat (owner 2026-10-16); visual check in a `-game` run (owner 2026-10-16).
- acceptance: build clean; existing tests pass; a fuse lit in PIE through `ARRArenaGrid::DestroyCell` raises the blink custom data 0 to 1, the barrel explodes with `BP_ExplosionFX_Barrel` (burst active, ring progress rising); a kamikaze explosion still uses `BP_ExplosionFX`; docs updated; frames of a `-game` run show the fuse and the explosion effects (subtask 7); committed.
- gdd_check: [[barrel]] (approved, M1), [[explosions]], [[ground-telegraph]], [[readability-rules]], [[enemy-kamikaze]] read 2026-10-16. No conflict; no `open` value needed; owner rule added to [[barrel]] (provisional, 2026-10-16).
- confirmed_by_owner: yes (2026-10-16: "Pas besoin de l'instruction test en un clic. Mais tu peux au moins vérifier visuellement si les effets se lance bien in game. Tu peux lancer.")
- milestone: M1
- next_step: none
- result: Done. Fusing barrel shakes (2 to 12 cm, render only) and blinks orange to white-hot 2 to 12 Hz (custom data 4, M_BarrelPaint); ARRExplosionFX optional BurstSystem and ShockwaveMaterial; NS_BarrelExplosion (45 particles), M_ExplosionShockwave ring; BP_ExplosionFX_Barrel on the Barrel row, kamikaze unchanged. Build exit 0; 15/15 explosion tests; -game frames show blink, danger circle, sphere and burst; ring hardly visible under the sphere, burst small, green decal square on first decal frame (probable shader compile): reported to the owner. Commits 297aeafe, 09946e84, 4f491333, 50fdc0d8, 8433d794, 7003f613, 62c44756.

## Subtasks
1. task-197-01-barrel-fuse-instability — cpp
2. task-197-02-explosion-fx-burst-shockwave — cpp
3. task-197-03-barrel-fuse-blink-material — bp
4. task-197-04-barrel-explosion-niagara — bp
5. task-197-05-shockwave-decal-material — bp
6. task-197-06-barrel-explosion-fx-blueprint — bp
7. task-197-07-barrel-fx-visual-check — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-197-01-barrel-fuse-instability]] — step 1, cpp.
- [[task-197-02-explosion-fx-burst-shockwave]] — step 2, cpp.
- [[task-197-03-barrel-fuse-blink-material]] — step 3, bp.
- [[task-197-04-barrel-explosion-niagara]] — step 4, bp.
- [[task-197-05-shockwave-decal-material]] — step 5, bp.
- [[task-197-06-barrel-explosion-fx-blueprint]] — step 6, bp.
- [[task-197-07-barrel-fx-visual-check]] — step 7, cpp (visual check).
- [[barrel]] — the rule it implements.
