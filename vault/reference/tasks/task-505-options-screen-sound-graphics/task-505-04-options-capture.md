---
id: task-505-04-options-capture
description: "Capture: Capture the options screen from the main menu."
parent: task-505-options-screen-sound-graphics
kind: task
status: done
---
# Options screen with Sound and Graphics, opened from the main menu: capture

- goal: Capture the options screen from the main menu.
- agent: capture
- depends_on: task-505-03-main-menu-options
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: Frames at the four resolutions show: C1 main menu Options plate active, not greyed; C2 Sound tab with three sliders and values; C3 Graphics tab with every option, nothing clipped or overlapping; C4 the look of the validated mockup, side by side; C5 a changed value (preset Low) still shown after a restart; sheet sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-505
- usage: included in the executor run of task-505
- runs: 1
- milestone: M2
- started: 2026-11-12T04:21:31Z
- waiting_for: none
- result: Done, 5 launches. C1 seen: Options plate active at 1280x720, 1920x1080, 1920x1200, 2560x1080 (Saved/Captures/task505_04_menu_sound_sheet.png). C2 seen: Sound tab, Master/Music/Effects sliders at 100 % at the four resolutions (same sheet). C3 seen: Graphics tab, 10 rows, nothing clipped at the four resolutions (Saved/Captures/task505_04_graphics_sheet.png). C5 seen: preset Low set at 1920x1080, still Low after restart at 1280x720, 1920x1200, 2560x1080. A3 countdown run-out seen: Resolution change shows Keep these display settings? Reverting in 14 s, then reverted to 1920x1200 by itself. C4: frame, tabs, rows and buttons match the mockup; tab and Reset labels ran over their plate caps, corrected (caeb7d8f, labels 30 -> 25, Saved/Captures/task505_04_fix_sheet.png). Ambiguous, for the owner: rows, arrows and labels smaller than the mockup; selected tab drawn on the narrow Shop plate (beige centre) where the mockup has a full beige tab. Settings reset to defaults after the capture (Windowed fullscreen 2560x1440, Epic). Owner decision on the two ambiguous points, 2026-11-12, verbatim: "Arrête ici" then "ah non, je voulais dire arrete de modifier l'écran, je le valide": screen validated as is, both differences kept on purpose, no further change.

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
