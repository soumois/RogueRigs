---
id: task-484-06-capture
description: "Capture: -game capture of the death sequence, the appearance and the final screen at the four resolutions, plus Retry and Main Menu."
parent: task-484-death-screen
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T01:44:22Z
- waiting_for: none
- result: open

## Relations
- [[task-484-death-screen]] — parent global task.
