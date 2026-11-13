---
id: task-525-02-capture
description: "Capture: alert bar before, between and after the hordes"
parent: task-525-horde-marker-one-at-a-time
kind: task
status: done
---
# Un seul point d'exclamation de horde à la fois: capture

- goal: alert bar before, between and after the hordes
- agent: capture
- depends_on: task-525-01-single-marker
- inputs: agent-capture (vault/reference/agents/agent-capture.md), level-flow (vault/reference/gdd/run/level-flow.md)
- expected_output: see done_when
- done_when: C1 level start: one "!"; C2 after the first horde: its "!" gone, the second shown; C3 after the last: none; C4 letters whole; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-525
- usage: included in the executor run of task-525
- runs: 1
- milestone: M2
- started: 2026-11-13T14:03:33Z
- waiting_for: none
- result: Done. One -game run (L_ArenaM1, 1600x900, pacing bot invulnerable, console never hacked so the fill stays at 0.4), 22 shots every 12 s; log: console landed 96.6 s, horde 0 front 116.3 s, horde 1 front 175.1 s. C1 seen: at 5 s and 107 s one '!' only, after the C letter (hm_00, hm_10). C2 seen: at 116 s the horde 0 '!' faded (front spawning) and no second; at 126 s it is gone and the horde 1 '!' shows (at the B letter, passing behind it) (hm_11, hm_12). C3 seen: at 187 s and 216 s no '!' (hm_18, hm_21; overtime hordes have no mark). C4 seen: C, B, S whole on every frame, a '!' on a letter passes behind it. Frames for the owner: Saved/Captures/task525-marker/bar_sheet.png and contact_sheet.png. Note (unchanged behaviour, as in task-524): with the console pending, the horde 1 '!' slides back from B toward the paused fill (at C by 166 s, hm_16). GameUserSettings.ini unchanged.

## Relations
- [[task-525-horde-marker-one-at-a-time]] — parent global task.
