---
id: task-465-02-slice-capture
description: "Capture: Capture the burst rifle impact slice in a fight and send it to the owner before expansion"
parent: task-465-shot-impact-burst
kind: task
status: done
---
# Player shot impact burst: capture

- goal: Capture the burst rifle impact slice in a fight and send it to the owner before expansion
- agent: capture
- depends_on: task-465-01-laser-impact-slice
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png
- expected_output: see done_when
- done_when: Frames sent to the owner showing: C1 a burst rifle hit on a robot with the green cartoon burst (core, spikes, glow, triangles); C2 a hit on rock; C3 a full volley on a group with robots and damage numbers readable; C4 the Shooter visible among the impacts; C5 the same fight before the change for comparison.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-465
- usage: included in the executor run of task-465
- runs: 1
- milestone: M2
- started: 2026-11-06T09:39:36Z
- waiting_for: none
- result: Done, 4 launches (same fight before and after: task465_02_before/after slow motion 0.3 on a Heavy then aim at the rock wall; task465_02_volley_before/after full speed with HUD on a line of 4 Chasers and a Heavy, Shooter present). Before runs set BP_Projectile back to NS_ShotImpactSparkGreen for the run only, file restored from git. C1 seen: green radial star with pale centre and a triangle at each rifle hit on the Heavy (Saved/Captures/task465_02_robot_compare.png; before: no visible spark). C2 seen: green burst with triangle where the shot hits the rock wall (task465_02_rock_compare.png; before: nothing visible). C3 seen: full-speed volley, robots and 10 damage numbers readable, small green stars beside them; the chaser death explosions dominate (task465_02_volley_compare.png). C4 seen: Shooter (green body, red targeting laser) top right in every volley frame. C5 seen: before frames of both fights. Style-guide (01): A8 sprite bound fail fixed in 01 (8 sprites, 0.24 s); its round green dots on the numbers are the rifle shot bolts in flight, not the burst. Not seen: barrel hit (none in frame). Frames for the owner: Saved/Captures/task465_02_robot_compare.png, task465_02_rock_compare.png, task465_02_volley_compare.png.

## Relations
- [[task-465-shot-impact-burst]] — parent global task.
