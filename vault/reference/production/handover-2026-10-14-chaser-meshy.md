---
id: handover-2026-10-14-chaser-meshy
description: "State after the 2026-10-14 Meshy chaser session: built items, open items and owner decisions, files not to commit, how to resume."
parent: production-index
kind: note
---
# Handover — Meshy chaser session (2026-10-14)

Scope: tasks 125 to 165 (XP nut and chip meshes and glow, then the chaser). All tasks are `done` ([[tasks-index]]); task 151 was closed as abandoned. Nothing runs; the editor may still be open (launched by the session, no PIE). Pipeline: [[chaser-meshy-pipeline]]; lessons: [[orchestrator-lessons-2026-10-14]].

## Built
- XP pickups: nut (basic XP) and lilac/magenta chip (big block), floor glow disc with fade, Lumen: see [[perf-xp-pickups-glow]] and [[ppv-xp-glow]].
- Chaser: Meshy mesh, texture corrected, cut into 10 parts, baked run (RunRate 0.7) and attack clips played by the material, random phase per robot (spread 2 s), no shadow, glow only on red texels. Seen in game by the owner (works) and in headless game shots (`vault/assets/chaser-ingame-run.png`, `chaser-ingame-attack.png`).
- Lore rule L3 removed; jokes may touch robots (T2).

## Open items (owner decides unless stated)
- Triangle budget 3000: mesh is 3115 (3263 as parts, 3273 in Unreal): accept or optimise ([[enemy-chaser]]).
- Run foot sliding: rerun the bake with `--thigh-deg 18`. The attack clip is not slowed (gameplay wind-up 0.1 s).
- The left/right arm assignment (Y mirror inside the material) was never proven; hood logo plate is small (59 px) and shows a faint relief ghost of the old logo (flatten the normal map or enlarge the plate).
- Not done: cost check of the animated material at 100, 200, 400 robots (`rr.Stress.Spawn`); death effect with the new part meshes ([[enemy-death]]); shield chaser shell and pop scale unseen.
- Existing test `RogueRigs.Enemies.ShotHitsMovingRobotAndDeathShowsParts` fails: it expects EyeMesh visible, hidden since task 156 (update the expectation).
- XP: lilac chip versus Duskite violet readability; the sparkle and trail Niagara still magenta; optional Detail Tracing for Lumen on pickups.

## Not mine (do not commit)
- The owner's `Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/...` files (test chip actor; PPV_XPGlow bloom he tuned).
- `Config/DefaultEditor.ini` is rewritten by an open editor: restore with `git checkout --` when asked.
- Another session stages vault course nodes (`reference/course/`, `cloud-session-git`): commit by path.
- Commits are local: the branch is ahead of `origin/master`, push is the owner's call.

## Resume (new account or session)
Read `CLAUDE.md`, then [[orchestration-protocol]], [[orchestrator-lessons]] and the two lessons notes. The owner's standing rules also live there: one owner point per global task, a "non" to every option means do nothing more, report an unrelated in-progress task once, French answers, no unrequested tests, bots or screenshot runs. "Lance les tâches jusqu'au bout" authorises running a plan without asking again.

## Relations
- [[production-index]] — parent index.
- [[chaser-meshy-pipeline]] — how the chaser is built.
- [[orchestrator-lessons-2026-10-14]] — pitfalls and tools.
- [[handover-2026-10-12-gameplay-hud]] — the previous handover.
- [[wishlist]] — deferred ideas.
