---
id: task-148-xp-disc-fade
description: "Global task: Fade the glow disc with the pickup phase: off in the air, on in 0.2 s at landing, off when magnetised."
parent: tasks-done-index
kind: task
status: done
---
# XP glow disc: fade with the pickup phase

- request: The light must not be in the air when a pickup is ejected: fade to 1 in 0.2 s when it lands, and fade to 0 in 0.2 s when it is magnetised (owner, 2026-10-13) (owner, 2026-10-14)
- goal: Fade the glow disc with the pickup phase: off in the air, on in 0.2 s at landing, off when magnetised.
- acceptance: The project compiles (RogueRigsEditor Win64 Development, editor closed); no existing test broken (only update expectations a requested change breaks, no new test); sys-pickups and sys-pickups-lifecycle updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: none
- result: Done. M_XPGlowDisc: Fade scalar parameter (default 1, Custom Primitive Data index 0) multiplies the emissive; bGlowFade true on BP_PickupXP and BP_PickupXPBig (duration 0.2, index 0); compiles; runtime effect not verified, for the owner.

## Subtasks
1. task-148-01-xp-disc-fade — cpp (the fade in C++, editor closed)
2. task-148-02-xp-disc-fade-material — bp (material reads the fade, Blueprint flags, editor open)

## Relations
- [[tasks-done-index]] — parent index.
- [[task-148-01-xp-disc-fade]] — step 1, cpp.
- [[task-148-02-xp-disc-fade-material]] — step 2, bp.
