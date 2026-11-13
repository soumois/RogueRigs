---
id: task-540-02-capture
description: "Capture: Before/after capture of the main menu at 1920 x 1080"
parent: task-540-main-menu-no-focus-outline
kind: task
status: done
---
# Main menu: no orange focus outline around New Run: capture

- goal: Before/after capture of the main menu at 1920 x 1080
- agent: capture
- depends_on: task-540-01-focus-outline-off
- inputs: Saved/Scratch/owner-ui-batch-2026-11-13/1.webp
- expected_output: see done_when
- done_when: C1 no orange rectangle around any of the three buttons at open; C2 none after one keyboard down then up; C3 buttons, title and background otherwise unchanged against the before frame.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-540
- usage: included in the executor run of task-540
- runs: 1
- milestone: M2
- started: 2026-11-13T23:06:24Z
- waiting_for: none
- result: Done: C1 seen (no rectangle at open, Saved/Captures/task540_enter2/mm_00.png); C2 seen (no rectangle after real Down then Up key presses, mm_02 and mm_04, crops nav.png); C3 seen (title, buttons, background unchanged against Saved/Captures/task540_before/mm_00.png); hover Exit plate brightens and grows (task540_after/hv_00.png). Sheet: Saved/Captures/task540_final/before_after.png. 5 -game launches total for the task (before, after, 3 keyboard runs: the first two did not reach the game window, fixed with an EnumWindows + ALT-tap foreground script in the scratchpad). Budget ALERT at subtask 01 (60 turns, 0.73 M units vs light 40 turns, 0.6 M): cause = key-injection tooling to verify keyboard focus and Enter (no rr command exists) and the extra correction for the engine focus rectangle.

## Relations
- [[task-540-main-menu-no-focus-outline]] — parent global task.
