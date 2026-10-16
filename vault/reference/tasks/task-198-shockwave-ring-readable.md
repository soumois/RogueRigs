---
id: task-198-shockwave-ring-readable
description: "Global task: Explosion shockwave ring readable: grows over about 0.6 s, independent of the sphere, then fades; 60 uu wide."
parent: tasks-done-index
kind: task
status: done
---
# Shockwave ring readable

- request: Code Architect run 2 follow-up of task-197: owner answered "Ok" to the proposals (2026-10-16): ring wider, about 60 uu, and lasting after the sphere (growth 0.25 s to about 0.6 s) (owner, 2026-10-16)
- goal: Explosion shockwave ring readable: grows over about 0.6 s, independent of the sphere, then fades; 60 uu wide.
- acceptance: build exit 0; explosion tests pass (only expectations broken by this change updated, no new test); ring growth and fade seconds named in sys-explosions; ring still hidden at the end of the actor life; `RingWidth` 60 read back on the material; committed by path; committed.
- gdd_check: [[barrel]] owner rule 2026-10-16 (ring and burst provisional), [[explosions]] read 2026-10-16; no conflict, no `open` value.
- confirmed_by_owner: yes (2026-10-16: "Ok" to the proposals)
- milestone: M1
- next_step: none
- result: Done. Ring on its own timeline (grow 0.6 s, fade 0.2 s, outlives the 0.6 s sphere) and 60 uu wide (RingWidth parameter); build ok, explosion tests pass; look checked in task-200 frames. Commits c532ff13, d09f80d0, 1edbf66e.

## Subtasks
1. task-198-01-shockwave-ring-timing — cpp
2. task-198-02-shockwave-ring-width — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-198-01-shockwave-ring-timing]] — step 1, cpp.
- [[task-198-02-shockwave-ring-width]] — step 2, bp.
- [[barrel]] — the owner rule it refines.
