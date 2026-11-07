---
id: task-486-03-capture
description: "Capture: level-up and run-won screens, the two buttons at rest, hovered and disabled, four resolutions, compared with task-485."
parent: task-486-grey-buttons-plate
kind: task
status: done
---
# Cream plate look for the two grey buttons (level-up Reroll, run-won Main Menu): capture

- goal: -game capture of the level-up and run-won screens with the two buttons at rest, hovered and (Reroll) disabled, at the four resolutions, compared with the task-485 captures.
- agent: capture
- depends_on: task-486-02-end-screen-plate
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Captures/task485_resA/c_lu.png, Saved/Captures/task485_resB/c_won.png
- expected_output: see done_when
- done_when: Checklist, each seen on a frame at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 (or closed on the owner "je valide"): C1 level-up Reroll is a cream plate, label, ore icon and cost readable, no grey engine look; C2 Reroll hovered is brighter and slightly larger than at rest; C3 Reroll disabled is greyed and unchanged under the mouse; C4 run-won Main Menu is a cream plate like the run-lost Main Menu, label readable; C5 Main Menu hovered brighter and slightly larger than at rest; C6 cards, title, subtitle and Duskite secured row unchanged from the task-485 captures, nothing overlaps or is clipped. Sheet sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-486
- usage: included in the executor run of task-486
- runs: 1
- milestone: M2
- started: 2026-11-07T03:37:15Z
- waiting_for: none
- result: Done, 2 launches (Saved/Captures/task486_capA: 1080, 720, 2560x1080; task486_capB: 1920x1200 --force-res), sheets in Saved/Captures/task486_sheets (lu_full, lu_btn, won_full, won_btn). C1 seen at 4 res (cream plate, Reroll, ore icon, cost readable, no grey engine look). C2 seen at 4 res (hovered brighter and larger, log amount 1.00 plate material). C3 seen at 4 res (0 Duskite: greyed plate, faded label, unchanged under the mouse, log enabled 0 amount 0). C4 seen at 4 res (run-won Main Menu cream plate like run-lost, label readable). C5 seen at 4 res. C6 seen: cards, title, subtitle, Duskite row at the task-485 positions (lu_full, won_full before1080), nothing overlaps or is clipped; the Reroll plate sits over the centre of the weapons HUD bar as the grey button did. rr.Cheat.Reroll updates the cost (lurerollA 20), rr.Run.MainMenu reaches the main menu (menuA).

## Relations
- [[task-486-grey-buttons-plate]] — parent global task.
