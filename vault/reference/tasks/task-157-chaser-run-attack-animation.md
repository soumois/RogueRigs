---
id: task-157-chaser-run-attack-animation
description: "Animate the chaser in the game: run loop and attack clip from a baked texture."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: cut into rigid parts

- request: The owner asks for the run and attack animations of the chaser (owner, 2026-10-14); the technique chosen on 2026-10-13 is a baked animation texture (one transform per part and per frame, no skeletal mesh at run time), which needs the mesh cut into rigid parts first (owner, 2026-10-14)
- goal: Animate the Meshy chaser in the game with a run loop and an attack clip, using a baked animation texture played by the material.
- acceptance: In the game, every chaser (and shield chaser) plays the run loop while it moves and the attack clip at each strike (0.1 s wind-up then the strike), driven by the material from a baked texture; no skeletal mesh; no per-frame C++ cost; the doc nodes updated; the owner is told how to see it.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-157-02-chaser-clips-bake
- result: open

## Subtasks
1. task-157-01-chaser-run-attack-animation — 3d
2. task-157-02-chaser-clips-bake — 3d (clips and bake)
3. task-157-03-chaser-anim-cpp-hook — cpp (C++ hook)
4. task-157-04-chaser-anim-unreal — bp (import, material, Blueprints)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-157-01-chaser-run-attack-animation]] — step 1, 3d.
- [[task-157-02-chaser-clips-bake]] — step 2, 3d.
- [[task-157-03-chaser-anim-cpp-hook]] — step 3, cpp.
- [[task-157-04-chaser-anim-unreal]] — step 4, bp.
