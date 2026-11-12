---
id: task-510-10-options-capture
description: "Capture: -game captures of the options screen against the mockup at four resolutions, other tabs, pause opening, settings read back after relaunch."
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: capture

- goal: -game captures of the options screen against the validated mockup: each tab and the opening from the pause menu at the four resolutions
- agent: capture
- depends_on: task-510-09-pause-menu-options-button
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: At 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080, nothing cut or overlapping and close to the validated mockup: C1 Sound tab opened from the main menu, which shows Options active; C2 Graphics tab with every control of options-screen; C3 Gameplay tab with the four toggles; C4 the screen opened from the pause menu over the paused arena, the pause menu showing Resume, Options, Exit. C5 at 1920 x 1080: the keep-or-revert dialog with its countdown. C6 a value set by debug command is in the saved settings file and applied after a relaunch (file read, no extra capture). Sheet sent.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:53:10Z
- waiting_for: none
- result: 5 launches (capture.js), sheets Saved/Captures/t500_sheets/s1080.png, s720.png, s1200.png, s2560.png; frames Saved/Captures/t500_c_<res>/. At 1920x1080, 1280x720, 1920x1200, 2560x1080: C1 seen (main menu with Options active, mm; Sound tab over the menu, sa); C2 seen (Graphics: window mode, resolution, VSync, frame limit, resolution scale, preset and five sub-settings, gr); C3 seen (four toggles, gp); C4 seen (pause menu Resume, Options, Exit, pm; options over the paused arena, po). Nothing cut or overlapping; close to the validated mockup (two columns, plates, frame, buttons). Minor: at 1920x1200 the Resolution scale value sits close to the divider; at 2560x1080 the frame and rows stretch wide (fraction anchors). C5 seen at 1080 (dl: 'Keep these display settings?', 'Reverting in 14 s', Keep, Revert); after Revert and close, the pause menu is back (bk). C6 seen: rr.Options.Set screenshake 0 and master 40 wrote bScreenShake=False and MasterVolume=0.4 to Saved/Config/WindowsEditor/GameUserSettings.ini; the next launch logged 'loaded settings: Master 40% ... ScreenShake 0'; rr.Options.Set reset sound/gameplay then restored 100 % and On (file read back). Not seen on a frame: live effect of the toggles in a fight, real mouse clicks, the countdown reaching 0, a real window mode change.

## Relations
- [[task-510-options-screen]] — parent global task.
