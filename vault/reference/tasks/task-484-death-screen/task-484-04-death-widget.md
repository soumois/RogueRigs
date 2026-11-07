---
id: task-484-04-death-widget
description: "Bp: New death-screen widget from the validated mockup, with the appearance animation and the two buttons wired to C++."
parent: task-484-death-screen
kind: task
status: done
---
# Death screen with Retry and Main Menu: bp

- goal: New death-screen widget from the validated mockup, with the appearance animation and the two buttons wired to C++.
- agent: bp
- depends_on: task-484-03-death-flow
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md)
- expected_output: see done_when
- done_when: The widget has a Canvas root with anchors (umg-layout), shows the texts and recap from C++, plays the appearance of death-screen with skip on input, focus on Retry, cursor shown, Retry and Main Menu call the C++ actions; compiled with warnings as errors, saved, PIE death with no script error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-484
- usage: included in the executor run of task-484
- runs: 1
- milestone: M2
- started: 2026-11-07T01:24:52Z
- waiting_for: none
- result: Commit 0ec081cd. WBP_RunLost (/Game/RogueRigs/UI/WBP_RunLost): duplicated from WBP_PauseMenu, reparented to URRRunLostWidget, Canvas root with anchors from the validated mockup (title point (0.5,0.202), subtitle (0.5,0.278), RecapBlock (0.25,0.343)-(0.749,0.667) holding Frame, two row lines and three label/value rows, RetryButton (0.367,0.765) and MainMenuButton (0.634,0.765) 440x108, SkipButton full screen). Title rails and subtitle lines placed from the text desired width. ApplyAppearance(T): backdrop 0 to 75 % black and SetLevelDesaturation 0 to 1 over 0.6 s, title fade and scale 1.4 to 1 over 0.25 s with shake, recap fade and 40 px slide over 0.3 s, buttons over 0.2 s, end at 1.4 s; SkipButton click or any key skips, then FinishAppearance focuses Retry. Construct sets bShouldPerformFullTickWhenPaused on the owning controller so the paused camera keeps updating (desaturation was invisible in the first capture; one correction, plus button label font 36 to 30 for Main Menu overflow). BP_RRPlayerController RunLostWidgetClass = WBP_RunLost_C. Compiled with warnings as errors, saved; check by -game death (rr.Run.Lost.Die) instead of PIE (no console command over MCP): no script error, run-lost screen shown, appearance finished with focus on Retry, frames Saved/Captures/rl04b_1080 and rl04c_1080 match the mockup. ALERT cause: executor run over the standard budget (179 turns, 5.32 M units at this closing): MCP was driven by hand through a Node JSON-RPC client (tool discovery, DSL graph writing) plus three -game captures for the self-check.

## Relations
- [[task-484-death-screen]] — parent global task.
