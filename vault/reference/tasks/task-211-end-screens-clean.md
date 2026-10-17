---
id: task-211-end-screens-clean
description: "Global task: end screens (run won, run lost, death) show alone, with no HUD and no subtitle."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# End screens clean

- request: "oui" (owner, 2026-10-17) to a task cutting subtitles and hiding the HUD on the end screens, after the task-209-05 capture showed a Kessler-Voss subtitle and the dimmed HUD over the run-won screen; then "fait le 1" confirming the plan.
- goal: the end screens (run won, run lost, death) show alone: no HUD element and no subtitle.
- acceptance: rendered captures of the run-won, shuttle-missed and death end screens show only the end screen; restarting brings the HUD and subtitles back; tests pass except the known failures; docs updated; captures sent to the owner; lint clean; committed.
- gdd_check: [[extraction]] (M1 end screens with restart button), [[run-structure]]. No conflict. The end screen is drawn inside WBP_AlertObjectives, a child of WBP_HUD, so the task-207 whole-HUD hide cannot be reused. Owner 2026-10-17: same rule for the death screen; the repeated speaker name in the Kessler-Voss line is a separate point.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-211-01-end-silence-code
- result: open

## Subtasks
1. task-211-01-end-silence-code — cpp
2. task-211-02-end-hud-bp — bp
3. task-211-03-end-capture — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-207-intermission-hide-hud]] — the same fix for the intermission.
- [[task-211-01-end-silence-code]] — step 1, cpp.
- [[task-211-02-end-hud-bp]] — step 2, bp.
- [[task-211-03-end-capture]] — step 3, cpp.
