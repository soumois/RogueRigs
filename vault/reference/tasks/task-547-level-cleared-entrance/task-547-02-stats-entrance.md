---
id: task-547-02-stats-entrance
description: "Bp: Wire the shared entrance to the stats screen and its level 4 variant"
parent: task-547-level-cleared-entrance
kind: task
status: done
---
# Level cleared screen appears with the death screen entrance effect: bp

- goal: Wire the shared entrance to the stats screen and its level 4 variant
- agent: bp
- depends_on: task-547-01-shared-entrance
- inputs: intermission (vault/reference/gdd/run/intermission.md), death-screen (vault/reference/gdd/run/death-screen.md)
- expected_output: see done_when
- done_when: A1 to A3 hold.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-547
- usage: included in the executor run of task-547
- runs: 1
- milestone: M2
- started: 2026-11-14T04:11:56Z
- waiting_for: none
- result: 923df28a: WBP_RunLost graph emptied of its entrance (Tick, ApplyAppearance function, SkipButton click, OnSkipAppearanceRequested body, end of OnRunLostShown) since URRScreenEntrance drives it; WBP_Intermission Backdrop alpha 0.85 -> 0.75 (A1 0 to 75 %), no graph change (C++ starts the entrance after OnRecapShown, skip button created at runtime). Level 4 variant = same WBP_Intermission (bRunWon), confirmed via BP_RRPlayerController IntermissionWidgetClass. Both compiled, saved. A1-A3 seen in 547-03 frames; Enter key skip left the stats phase open (rr.Run.Status: transition stats).

## Relations
- [[task-547-level-cleared-entrance]] — parent global task.
