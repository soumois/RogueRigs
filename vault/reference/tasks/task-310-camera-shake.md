---
id: task-310-camera-shake
description: "Global task: A shared C++ camera shake any actor can play on the player camera; first used by the bonus chest lid slam (task-309)."
parent: tasks-to-do-index
kind: task
status: todo
---
# Shared camera shake

- request: "309 - D E F G et H" (owner, 2026-10-23), E = at the chest lid slam, a short freeze of the chest then a light camera shake; no camera shake exists in Source/ (checked 2026-10-23: no CameraShake, time dilation or hitstop code).
- goal: A shared C++ camera shake any actor can play on the player camera; first used by the bonus chest lid slam (task-309).
- acceptance: one C++ call plays a light shake on the local player's top-down camera; strength and duration EditDefaultsOnly, provisional owner 2026-10-23; no global time dilation; nothing else wired to it (heavy slam and explosions stay as they are unless the owner asks, Q21); doc node written; build green; existing tests pass; committed. The shake itself is seen in the task-309 capture.
- gdd_check: sys-player (camera) read 2026-10-23. Separate global task by the splitting rule: a reusable camera system is its own system, not part of the chest. Pending owner answers, defaults: Q20 new global task, Perlin-noise shake defined in C++, chest preset 0.2 s, about 4 uu translation, no rotation, no distance falloff; Q21 not wired to the heavy slam or explosions.
- confirmed_by_owner: yes (owner 2026-10-23: "ok pour tous les défauts (Q1 à Q25)"; every default below applies; not launched yet)
- milestone: M2
- next_step: task-310-01-camera-shake
- result: open

## Subtasks
1. task-310-01-camera-shake — cpp: Shared camera shake in C++

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-310-01-camera-shake]] — step 01, cpp.
