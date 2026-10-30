---
id: task-379-03-loading-animation-widget
description: "Bp: WBP_Loading: looping image animation in place of the bar when progress is not measured"
parent: task-379-new-run-loading-screen
kind: task
status: done
---
# Main menu New Run: show the loading screen: bp

- goal: In WBP_Loading add a new Image LoadingAnimImage (variable, Collapsed by default; the existing LoadingImage stays reserved and untouched) in the column where LoadingBarBox sits, with the image and the looping UMG widget animation decided in Q4 (default: brush /Game/RogueRigs/UI/Icons/T_Icon_Ore, 128 x 128, centred, render-transform angle 0 to 360 over 2 s, linear, looping). In OnLoadingShown, after the existing logic: if IsLoadingProgressMeasured() is false, collapse LoadingBarBox, show LoadingAnimImage and play the animation in a loop; else keep the bar as today and LoadingAnimImage collapsed. Stop the animation in Destruct. Compile with warnings as errors, save, read the tree, the animation track and the graph back. Return a change summary; do not edit doc nodes.
- agent: bp
- depends_on: task-379-01-new-run-loading-code
- inputs: wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), Change summary of task-379-01-new-run-loading-code (IsLoadingProgressMeasured), Global questions Q4 and Q5 of task-379-new-run-loading-screen for the decided image and animation
- expected_output: /Game/RogueRigs/UI/WBP_Loading.uasset saved, one commit `task-379-03-loading-animation-widget: <summary>`.
- done_when: Tree read back: LoadingAnimImage with the Q4 brush and size, Collapsed by default, LoadingImage unchanged; the rotation is tick-driven (owner accepted 2026-10-30 instead of a WidgetAnimation asset): EventTick SpinAngle += dt*180 mod 360, SetRenderTransformAngle on LoadingAnimImage, one turn per 2 s looping, stopped on EventDestruct; graph read back: the branch on IsLoadingProgressMeasured in OnLoadingShown with the loop play and the bar collapse; compiles with no warning; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 148, cache write 163743, cache read 5807579, output 42703
- usage: turns 73, context start 58k peak 134k, resumes 0, 10 min, units 1.00 M
- runs: 1
- milestone: M2
- started: 2026-10-30T04:16:27Z
- waiting_for: none
- result: WBP_Loading redone (bdc75c51, d3b6e4ac): LoadingAnimImage T_Icon_Ore 128x128 Collapsed; tick-driven spin (SpinAngle += dt*180 mod 360, SetRenderTransformAngle, bSpinning off in Destruct), owner accepted instead of WidgetAnimation; OnLoadingShown branch on IsLoadingProgressMeasured; compiled, saved, read back. Unverified: runtime/render.

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
