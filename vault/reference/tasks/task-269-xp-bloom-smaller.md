---
id: task-269-xp-bloom-smaller
description: "Global task: The XP pickups no longer bloom into big glowing blobs: small bright cyan dots with a small halo."
parent: tasks-done-index
kind: task
status: done
---
# XP bloom reduced

- request: the XP are too bloomed (owner screenshot 2026-10-21), reference: small cyan dots with a small halo (owner, 2026-10-21)
- goal: The XP pickups no longer bloom into big glowing blobs: small bright cyan dots with a small halo.
- acceptance: Before and after values read back; materials and systems compile and are saved; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. XP blob came mainly from the camera-facing glow disc (Radius 50 cm = 100 cm across for a 30 cm nut), plus the emissive nut at GlowAmount 25. Changed: M_XPNut_hole GlowAmount 25 -> 8, MI_XPGlowDisc_Nut Radius 50 -> 22, MI_XPGlowDisc_Chip Radius 68 -> 32 and Intensity 0.5 -> 0.25, M_XPCube_core_bright 12 -> 6 and _dark 6 -> 3; sparkle and trail unchanged (only on collection and while magnetised); bloom intensity and threshold untouched; instance parameter values only, referencers not listed, no recompile. Not seen on screen; next levers: mesh scale, disc Falloff.

## Subtasks
1. task-269-01-xp-bloom-smaller — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-269-01-xp-bloom-smaller]] — step 1, bp.
