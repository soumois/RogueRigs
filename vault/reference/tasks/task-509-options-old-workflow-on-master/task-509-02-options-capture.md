---
id: task-509-02-options-capture
description: "Capture: capture the merged Options screen from main menu and pause at the four resolutions"
parent: task-509-options-old-workflow-on-master
kind: task
status: done
---
# Écran Options : version de l'ancien workflow sur master, puis push: capture

- goal: capture the merged Options screen from main menu and pause at the four resolutions
- agent: capture
- depends_on: task-509-01-merge-old-options
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), wbp-options (vault/reference/tech/blueprints/ui/wbp-options.md), C:/Workspace/RogueRigs-old (task-500-10 captures, for comparison)
- expected_output: see done_when
- done_when: C1 Options opens from the main menu and shows the old-version layout and images; C2 Options opens from the pause menu and returning resumes the pause menu; C3 the four resolutions show no clipped or overlapping element; C4 the frames match the old-version captures; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-509
- usage: included in the executor run of task-509
- runs: 1
- milestone: M2
- started: 2026-11-12T12:33:00Z
- waiting_for: none
- result: 5 launches with capture.js from L_MainMenu (rr.Options.Open, rr.Menu.NewRun, rr.Debug.PauseMenu), sheets Saved/Captures/t509_sheets/s_1080.png, s_1280x720.png, s_1920x1200.png, s_2560x1080.png, compare_old_new.png; frames Saved/Captures/t509_c_<res>/. C1 seen at the four resolutions: main menu with Options active (mm), Sound, Graphics, Gameplay tabs over the menu (sa, gr, gp) in the old-version layout and sprites. C2 seen: pause menu Resume/Options/Exit (pm), Options over the paused arena (po), after Back the pause menu is back (bk); at 1920x1080 also the keep-or-revert dialog 'Keep these display settings?' 'Reverting in 14 s' (dl), then Revert. C3 seen: nothing clipped or overlapping at 1920x1080, 1280x720, 1920x1200, 2560x1080. C4 seen: SSIM against the task-510-10 frames of the worktree = 1.000 for mm, sa, gp at all four resolutions, 0.997-0.998 for gr (only the Window mode value differs: Windowed here, Windowed fullscreen in the old run, from the saved user settings, not the layout); pm/po differ only by the arena behind the backdrop; dl and po compared side by side on compare_old_new.png, same layout. One correction: the first 2560x1080 run lost the miner at 17 s before rr.Cheat.Invulnerable (map loaded sooner), the relaunch repeats the cheat from 16 s. The only log error line is the pre-existing 'InputMode:UIOnly ... Non-Focusable widget' at the loading screen (also in the old-version logs). No crash.

## Relations
- [[task-509-options-old-workflow-on-master]] — parent global task.
