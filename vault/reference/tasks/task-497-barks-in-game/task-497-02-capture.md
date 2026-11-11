---
id: task-497-02-capture
description: "Capture: One -game capture of barks and an alert in the subtitle widget."
parent: task-497-barks-in-game
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-11T05:17:51Z
- waiting_for: none
- result: open

## Relations
- [[task-497-barks-in-game]] — parent global task.
