---
id: task-279-drone-eye-glow
description: "Global task: The miner drone camera eye glows a little, orange, through an emissive restricted to the eye texels of M_Drone."
parent: tasks-done-index
kind: task
status: done
---
# Drone eye glow, orange

- request: J'aimerais faire aussi que l'oeil de mon robot (celui qui me suit) glow un peu plus (tu peux mettre de l'orange, c'est juste visuel) (owner, 2026-10-21)
- goal: The miner drone camera eye glows a little, orange, through an emissive restricted to the eye texels of M_Drone.
- acceptance: Mask texel report; parameters read back; M_Drone compiles and is saved; docs and colour decision recorded; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21, owner "ok" to every default: chaser 25 -> 50, drone eye orange (1, 0.45, 0.08) x 6 with the colour deviation accepted, thruster table with white-teal kept, no captures, XP back to pre-269)
- milestone: M1
- next_step: none
- result: Done. Commits 94dea849 (first run: colour test alone left 250 amber texels on 7 body faces, no change) and b12f7147 (retry). M_Drone emissive = Custom EyeGlowMask: amber test on linear base colour x LocalPosition box (x > 10, |y| < 3.5, -4.5 < z < 2.5) x EyeGlowColor (1, 0.45, 0.08) x EyeGlowAmount 6; 4639 eye texels, 0 non-eye texels pass, laser excluded, rust fails the colour test. Read back: before no parameters and no emissive, after EyeGlowAmount 6 and EyeGlowColor (1, 0.45, 0.08) on MI_Drone (no override), Emissive = Custom_0; recompiled, saved. Docs bp-drone-materials (revert: disconnect Emissive or EyeGlowAmount 0), color-code row. Not seen on screen.

## Subtasks
1. task-279-01-drone-eye-glow — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-279-01-drone-eye-glow]] — step 1, bp.
