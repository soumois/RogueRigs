---
id: task-497-02-capture
description: "Capture: One -game capture of barks and an alert in the subtitle widget."
parent: task-497-barks-in-game
kind: task
status: done
---
# Rook and Vesna barks shown in game: capture

- goal: One -game capture of barks and an alert in the subtitle widget.
- agent: capture
- depends_on: task-497-01-bark-director
- inputs: agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: C1 A Rook bark shown in the subtitle widget with his name. C2 A Vesna bark shown with her name. C3 A Kessler-Voss alert line shown with no bark. C4 Never two lines at once on these frames. Sheet sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-497
- usage: included in the executor run of task-497
- runs: 1
- milestone: M2
- started: 2026-11-11T05:17:51Z
- waiting_for: none
- result: 2 launches (Saved/Captures/task-497-barks, task-497-barks-2). C1 seen: Rook: Free gift from the sky. Taking it. (s_06.png). C2 seen: Vesna: Access point deployed. Go find it. (task-497-barks-2/v_05.png). C3 seen: Kessler-Voss console alert alone, no bark (s_00.png). C4 seen: one subtitle line per frame on these frames and on both contact sheets. Owner sheet: Saved/Captures/task-497-barks/owner_sheet.png. Cost not recorded: agent-usage.js finds no transcript (known tooling issue).

## Relations
- [[task-497-barks-in-game]] — parent global task.
