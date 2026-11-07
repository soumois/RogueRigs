---
id: task-485-10-capture
description: "Capture: -game capture of every screen with its buttons at rest and hovered, at the four resolutions."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: capture

- goal: -game capture of every screen with its buttons at rest and hovered, at the four resolutions.
- agent: capture
- depends_on: task-485-09-main-menu
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: Contact sheet sent to the owner, each point a pair of frames at rest and hovered: C1 main menu New Run and Exit; C2 pause Resume and Exit; C3 run-lost Retry and Main Menu; C4 stats Continue; C5 shop Reroll, Continue and one Buy; C6 a disabled Buy or Reroll hovered shows no change; C7 level-up Reroll, and a card hover unchanged; C8 run-won end screen button; C9 at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 nothing moved, overlapped or clipped on any screen. The owner's "je valide" on screen replaces this capture.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T03:03:44Z
- waiting_for: none
- result: Done. 5 -game launches (task485_resA, task485_resB with r.SetRes 1080/720/2560 in one run; task485_resA1200, task485_resB1200, task485_rl1200 at 1920x1200 with -ForceRes, r.SetRes cannot exceed the 1080-high desktop), plus the 1080 checks of 03-09 (task485_a1080, task485_b1080). Hover by the real cursor (rr.Debug.CursorOn). Verdicts: C1 seen (main menu New Run, Exit orange and grown; Options hovered no change; b1080/c_mm.png). C2 seen (pause Resume, Exit; task485_slice/crops.png). C3 seen (run-lost Retry, Main Menu; a1080/rl_crop.png). C4 seen (stats Continue and level-4 CASH OUT; b1080/c_st.png). C5 seen (shop Reroll, Continue, Buy; b1080/c_shopbtn.png, resB/c_shop1080.png; brightening of the dark teal Reroll plate is subtle, growth clear). C6 seen (disabled NEED 10 MORE Buy hovered: no change, log enabled 0 amount 0; b1080/c_buy.png). C7 seen (level-up Reroll lighter and grown after the tint correction, card hover unchanged; resA/c_lu.png, a1080/lu.png). C8 seen (run-won Main Menu lighter and grown, label readable; resB/c_won.png). C9 seen: at 1920x1080, 1280x720, 1920x1200, 2560x1080 nothing moved, overlapped or was clipped at rest or hovered (resA/sheet_*.png, resB/sheet_*.png, Saved/Captures/task485_res1200_sheet.png, task485_rl1200/contact_sheet.png). Not captured: mid-ease frame (0.1 s from code), greyed shop Reroll hovered, purchase while hovered. Budget ALERT (cause in 05 result).

## Relations
- [[task-485-ui-button-hover]] — parent global task.
