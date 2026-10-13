---
id: task-136-xp-pickups-stronger-glow
description: "Global task: Strengthen the emissive and widen the pulse range of the XP nut and chip so the bloom is stronger."
parent: tasks-done-index
kind: task
status: done
---
# XP pickups: stronger glow and wider pulse

- request: Increase the bloom effect and the intensity range of the new XP pickups (owner, 2026-10-13); read as: stronger emissive on the pickups and a wider pulse, with no global post-process change (owner, 2026-10-13)
- goal: Strengthen the emissive and widen the pulse range of the XP nut and chip so the bloom is stronger.
- acceptance: Parameters read back with the new values; no new asset beyond parameter changes; doc node updated; bloom read-only report; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. GlowAmount 10 (nut hole, chip core_bright) and 5 (core_dark), PulseAmount 0.4 on the three instances (parent default stays 0.15), period unchanged; no global bloom change; engine default camera bloom 0.675, threshold -1, no overrides.

## Subtasks
1. task-136-01-xp-pickups-stronger-glow — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-136-01-xp-pickups-stronger-glow]] — step 1, bp.
