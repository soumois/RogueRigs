---
id: task-484-06-capture
description: "Capture: -game capture of the death sequence, the appearance and the final screen at the four resolutions, plus Retry and Main Menu."
parent: task-484-death-screen
kind: task
status: done
---
# Death screen with Retry and Main Menu: capture

- goal: -game capture of the death sequence, the appearance and the final screen at the four resolutions, plus Retry and Main Menu.
- agent: capture
- depends_on: task-484-05-hud-loss-path
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: Contact sheet sent to the owner with: C1 a frame during the slow motion after the lethal hit; C2 frames of the appearance in order (backdrop, title, recap frame, buttons); C3 final screen at 1920 x 1080 matching the validated mockup; C4 1280 x 720, C5 1920 x 1200, C6 2560 x 1080 with nothing clipped or overlapping; C7 no GAME OVER text, no RUN LOST panel, no HUD on the screen; C8 Retry leads to the loading screen then level 1 with Duskite 0; C9 Main Menu opens the main menu; C10 a missed shuttle shows the screen with its own title.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-484
- usage: included in the executor run of task-484
- runs: 1
- milestone: M2
- started: 2026-11-07T01:44:22Z
- waiting_for: none
- result: 3 launches (plus the 04/05 self-check runs reused for 1080 frames). Owner sheets: Saved/Captures/rl06_owner/finals_4res.png and Saved/Captures/rl06_owner/sequence_and_buttons.png. C1 seen (rl06_2560_seq/s0slowmo_00: slow motion after the lethal hit, red damage vignette, HUD still up); C2 seen in order (rl04b_1080 rl_01..03 backdrop and grey level, title, recap fading in; rl04_1080/rl_07 buttons fading in, from the run before the desaturation fix so the level is still coloured there); C3 seen (rl04c_1080/rl_00, layout of the validated mockup); C4 1280x720, C5 1920x1200, C6 2560x1080 seen, nothing clipped or overlapping (on 2560x1080 the recap frame and button gap are wider, width anchors); C7 seen (no GAME OVER, no RUN LOST panel, no HUD on every final frame); C8 seen (DROPPING IN Level 1, then level 1 with Duskite 0, 100/100, Lv 1); C9 seen (L_MainMenu); C10 seen (CLAIM SECURED with its subtitle, rl05_shuttle). Unverified on screen: non-zero recap values (all runs had 0 kills and 0 Duskite; values covered by automation tests), gamepad confirm, skip by key in game (skip path covered by the end of appearance only). ALERT cause: executor run over the standard budget (MCP by hand, captures).

## Relations
- [[task-484-death-screen]] — parent global task.
