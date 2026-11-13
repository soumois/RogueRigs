---
id: task-527-03-complete-event
description: "Cpp: one distance sound at chest and console validation per Q12, Q13"
parent: task-527-audio-zone-objective-complete
kind: task
status: done
---
# Son à la validation d'un coffre ou d'une console: cpp

- goal: one distance sound at chest and console validation per Q12, Q13
- agent: cpp
- depends_on: task-527-02-sound-import
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A2 to A4 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-527
- usage: included in the executor run of task-527
- runs: 1
- milestone: M2
- started: 2026-11-13T16:16:57Z
- waiting_for: none
- result: ARRConsole::CompleteHack plays CompleteSoundEvent (EditDefaultsOnly, default ObjectiveComplete; None = silent) via FRRDistanceSound at the actor location, console and chest; row ObjectiveComplete (SW_ObjectiveComplete, Volume 1.0, bSpatial, FadeInner 200, FadeOuter 1075). Build green; RogueRigs.Console 6/6 pass (no RogueRigs.Audio tests exist). -game Verbose LogRRAudio (Saved/Captures/task527_objective_sound2/run.log): console complete 1 play at 400 uu fade 0.77, chest complete 1 play at 350 uu fade 0.83, no play during either hack. Owner GameUserSettings.ini restored after the runs.

## Relations
- [[task-527-audio-zone-objective-complete]] — parent global task.
