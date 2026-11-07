---
id: task-487-01-menu-plates
description: "Bp: WBP_MainMenu New Run, Options and Exit become plate buttons as main-menu Layout says, with the plate hover and the focus outline."
parent: task-487-main-menu-buttons-plate
kind: task
status: done
---
# Main menu buttons that fit the screen (plate buttons): bp

- goal: WBP_MainMenu New Run, Options and Exit become plate buttons as main-menu Layout says, with the plate hover and the focus outline.
- agent: bp
- depends_on: none
- inputs: main-menu (vault/reference/gdd/run/main-menu.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), wbp-run-lost (vault/reference/tech/blueprints/ui/wbp-run-lost.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Captures/task485_b1080/c_mm.png
- expected_output: see done_when
- done_when: The three buttons draw as cream plates with dark brown labels, Options greyed and without hover, New Run and Exit with the plate hover, focus shown as an orange outline; clicks, focus on New Run and Enter unchanged; title, background and gradient unchanged; compiled with warnings as errors and saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-487
- usage: included in the executor run of task-487
- runs: 1
- milestone: M2
- started: 2026-11-07T04:41:16Z
- waiting_for: none
- result: Done (663c7c6a). WBP_MainMenu: buttons moved into ButtonColumn (VerticalBox, HAlign Left in MenuColumn; slots HAlign Fill, padding 8) so the three plates share the New Run width; style T_Shop_ButtonContinue Box (as WBP_PauseMenu, paddings 0, Disabled tint 0.5 alpha 0.85); each label in <Name>Content Overlay, Roboto Bold 36 dark brown (0.05,0.04,0.03), padding 125/30; NewRunFocus and ExitFocus Images (RoundedBox outline orange (1,0.35,0.1) width 3, radius 10, fill alpha 0, inset 20/13) toggled Hidden/HitTestInvisible by RefreshColours on HasKeyboardFocus. C++ RRMainMenuWidget attaches NewRun/Exit with bBrightenPlate true (one-line change, build clean). Compiled, saved. Self-check -game 1920x1080 Saved/Captures/task487_try2/crops.png: plates, outline on New Run at rest, New Run and Exit hover brighten and grow, Options no hover. Enter and clicks: graph events unchanged, not exercised on screen.

## Relations
- [[task-487-main-menu-buttons-plate]] — parent global task.
