---
id: task-379-03-loading-animation-widget
description: "Bp: WBP_Loading: looping image animation in place of the bar when progress is not measured"
parent: task-379-new-run-loading-screen
kind: task
status: todo
---
# Main menu New Run: show the loading screen: bp

- goal: In WBP_Loading add a new Image LoadingAnimImage (variable, Collapsed by default; the existing LoadingImage stays reserved and untouched) in the column where LoadingBarBox sits, with the image and the looping UMG widget animation decided in Q4 (default: brush /Game/RogueRigs/UI/Icons/T_Icon_Ore, 128 x 128, centred, render-transform angle 0 to 360 over 2 s, linear, looping). In OnLoadingShown, after the existing logic: if IsLoadingProgressMeasured() is false, collapse LoadingBarBox, show LoadingAnimImage and play the animation in a loop; else keep the bar as today and LoadingAnimImage collapsed. Stop the animation in Destruct. Compile with warnings as errors, save, read the tree, the animation track and the graph back. Return a change summary; do not edit doc nodes.
- agent: bp
- depends_on: task-379-01-new-run-loading-code
- inputs: wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), Change summary of task-379-01-new-run-loading-code (IsLoadingProgressMeasured), Global questions Q4 and Q5 of task-379-new-run-loading-screen for the decided image and animation
- expected_output: /Game/RogueRigs/UI/WBP_Loading.uasset saved, one commit `task-379-03-loading-animation-widget: <summary>`.
- done_when: Tree read back: LoadingAnimImage with the Q4 brush and size, Collapsed by default, LoadingImage unchanged; the animation exists with the Q4 track and length; graph read back: the branch on IsLoadingProgressMeasured in OnLoadingShown with the loop play and the bar collapse; compiles with no warning; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-379-new-run-loading-screen]] — parent global task.
