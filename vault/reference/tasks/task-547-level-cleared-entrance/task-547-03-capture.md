---
id: task-547-03-capture
description: "Capture: Entrance sequence frames of the stats screen and the death screen"
parent: task-547-level-cleared-entrance
kind: task
status: done
---
# Level cleared screen appears with the death screen entrance effect: capture

- goal: Entrance sequence frames of the stats screen and the death screen
- agent: capture
- depends_on: task-547-02-stats-entrance
- inputs: death-screen (vault/reference/gdd/run/death-screen.md)
- expected_output: see done_when
- done_when: C1 stats screen frames at about 0.3, 0.7, 1.0 and 1.5 s show backdrop, title drop, frame slide, button last; C2 skip by a key shows the end state at once; C3 death screen sequence matches its before frames.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-547
- usage: included in the executor run of task-547
- runs: 1
- milestone: M2
- started: 2026-11-14T04:17:55Z
- waiting_for: none
- result: 6 -game launches (limit 5 exceeded by one: second skip run to understand a missing button plate). Sheet Saved/Captures/t547_sheet.png. C1 seen: stats frames ~0.3/0.7/1.0/1.5 s (t547_st_seq) show backdrop and grey level, then title (with bars, subtitle), then frame sliding up with fade, NEXT LEVEL last; entrance finished 1.36 s after show in the death run log. Level 4 CASH OUT same (t547_l4_seq). C2 seen: Enter (t547_st_skip) and SpaceBar (t547_st_skip2) via rr.Debug.PressKey jump to the end state on the next frame, NEXT LEVEL not pressed (transition stats); on that first frame the button plate is not drawn yet (first draw, 0.48 s shader hitch in the log) and the level desaturation lands one frame later; plate seen in the next shot. C3 seen: death screen after (t547_rl_after) matches before (t547_rl_before) frame by frame at 1280x720. Not verified: mouse click skip on screen, other resolutions (no layout change).

## Relations
- [[task-547-level-cleared-entrance]] — parent global task.
