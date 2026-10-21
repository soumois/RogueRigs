---
id: task-268-dark-ambience-tight-light
description: "Global task: The ambience is dark around a tight warm pool of light on the player, like the reference: shorter light radius, much dimmer ambient, fixed exposure."
parent: tasks-done-index
kind: task
status: done
---
# Dark ambience and tight player light

- request: the light is too wide and the dark ambience around the player of the reference is missing (owner screenshots 2026-10-21) (owner, 2026-10-21)
- goal: The ambience is dark around a tight warm pool of light on the player, like the reference: shorter light radius, much dimmer ambient, fixed exposure.
- acceptance: Values read back before and after (PlayerLight, DirectionalLight_0, SkyLight_0, PPV exposure); compile and save OK; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. PlayerLight radius 1400 -> 650 and intensity 60 -> 120 cd (floor pool ends about 577 uu); DirectionalLight_0 1.0 -> 0.12, SkyLight_0 0.5 -> 0.08 (violet kept, no shadows); exposure fixed on PPV_XPGlow: AutoExposureMinBrightness = MaxBrightness = 1.3 EV100 (method kept AEM_Histogram, Manual would read camera ISO/shutter and be far too dark; one number to tune, +1 halves the brightness); bloom untouched; previous values recorded for revert. Risks: core under the miner may clip to white (about 13 lux), sky light not recaptured. Not seen on screen; owner tunes by eye.

## Subtasks
1. task-268-01-dark-ambience-tight-light — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-268-01-dark-ambience-tight-light]] — step 1, bp.
