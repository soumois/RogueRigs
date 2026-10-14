---
id: task-157-03-chaser-anim-cpp-hook
description: "Drive the animation from C++ with Custom Primitive Data on the body mesh: clip, start time, no per-frame cost."
parent: task-157-chaser-run-attack-animation
kind: task
status: todo
---
# Chaser: animation hook in C++

- goal: Drive the animation from C++ with Custom Primitive Data on the body mesh: clip, start time, no per-frame cost.
- agent: cpp
- depends_on: task-157-02-chaser-clips-bake
- inputs: sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: In the enemy and chaser C++ (ARREnemy / ARRChaser, files in Source/RogueRigs/Enemies/): the body mesh gets Custom Primitive Data at spawn (and again whenever a pooled enemy is reused): index 0 = clip id (0 run, 1 attack), index 1 = clip start time in world seconds (the material plays the clip as t = Time - start; run loops with a random start offset per enemy so a crowd does not step in sync); when the enemy starts its melee wind-up (the 0.1 s telegraph before each hit) the hook sets clip 1 and start = now, and when the attack clip is over (0.5 s) or the wind-up is cancelled it sets clip 0 again with a fresh offset; the values are set only on those events, never per tick; the existing telegraph, damage timing, knockback, pooling and the shield variant behave as before; no new tests; docs sys-enemies-chaser (and sys-enemies-base if the base class changed) updated with the Custom Primitive Data indices (0 and 1 reserved for the chaser animation)
- done_when: The project builds with the command line toolchain without warnings from the changed files; the existing automation tests that the change touches still pass or their expectations are updated; the doc nodes updated; committed by name; the editor stays closed
- rules: editor closed (build from the command line); do not change gameplay values; commit by name with message `task-157-03-chaser-anim-cpp-hook: <summary>`
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-157-chaser-run-attack-animation]] — parent global task.
