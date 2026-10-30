---
id: task-384-02-subtitle-lines-capture
description: "Capture: -game check that the subtitle lines stay outside the subtitle on levels 1 and 4."
parent: task-384-level-end-subtitle-lines
kind: task
status: cancelled
---
# Stats screen subtitle lines: capture

- goal: -game windowed captures through capture.js: level 1 and level 4 stats screens at 1920 x 1080 and 1280 x 720; compare with the look before. Report each point seen, not seen or ambiguous with the frame path.
- agent: capture
- depends_on: task-384-01-subtitle-lines-widget
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), Look before: `Saved/LevelEndScreen/task-381/L4_1920x1080_v2.png` (the defect) and the task-383-03 frames under `Saved/LevelEndScreen/task-383/`, Debug commands and procedure: task-381-03-level-end-screen-capture (`rr.Run.EndLevel`, `rr.Run.NextLevel`, `rr.Debug.Shots`)
- expected_output: frames under `Saved/LevelEndScreen/task-384/` (not committed); a report per point C1 to C3.
- done_when: C1 on level 4 at both resolutions neither thin line touches or crosses the subtitle text, each sits a small gap from its end; C2 on level 1 at both resolutions the same, lines closer to the shorter text than before; C3 lines stay level with the subtitle centre and inside the screen, nothing else moved. Each point seen, not seen or ambiguous with its frame path; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: Owner 2026-10-30: "Non, annule"; owner validated the screen as is, subtitle lines not fixed.

## Relations
- [[task-384-level-end-subtitle-lines]] — parent global task.
