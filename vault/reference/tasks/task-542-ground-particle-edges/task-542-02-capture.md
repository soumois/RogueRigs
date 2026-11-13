---
id: task-542-02-capture
description: "Capture: Before/after capture of the effect on the floor"
parent: task-542-ground-particle-edges
kind: task
status: done
---
# Ground particles: no visible sprite edges: capture

- goal: Before/after capture of the effect on the floor
- agent: capture
- depends_on: task-542-01-fix-edges
- inputs: Saved/Scratch/owner-ui-batch-2026-11-13/2.webp
- expected_output: see done_when
- done_when: C1 the ground particles of image 2 visible on the floor; C2 no straight or square edge around any of them; C3 their look otherwise matches the before frame.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-542
- usage: included in the executor run of task-542
- runs: 1
- milestone: M2
- started: 2026-11-13T23:29:03Z
- waiting_for: none
- result: Done. 2 launches before the fix (task542-before, a diag run with DOF and AA off: edges also there, so not a post-process artefact), 2 after (one paused by focus loss, one used). C1 seen: arrival extractor takeoff at 1920x1080, ground puffs, streaks and dot clusters on the floor (Saved/Captures/task542-after2/after_04.png). C2 seen: no dotted quad outline around any of them; before frames show them on every puff (Saved/Captures/task542-before/before_02.png). C3 seen: same beige puffs, sizes and satellite dots as the before frame. Comparison crops: Saved/Scratch/task542/crop-compare.png (temporary), sheet Saved/Scratch/task542/task542-before-after.png (temporary). The after frames still show 'Compiling Shaders' on other assets (extractor hull flat), unrelated to the ground particles.

## Relations
- [[task-542-ground-particle-edges]] — parent global task.
