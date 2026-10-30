---
id: task-381-02-intermission-widget-rebuild
description: "Bp: rebuild WBP_Intermission to the ChatGPT level-end mockup with the LevelEnd and Spare Parts sprites, Root Canvas and anchors."
parent: task-381-level-end-screen-rebuild
kind: task
status: todo
---
# Level-end screen rebuild: WBP_Intermission

- goal: Rebuild /Game/RogueRigs/UI/WBP_Intermission after the mockup `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp` and the decided values of the global `request`, `acceptance` and `## Questions` (Q1 to Q5): Root Canvas direct children per umg-layout (Backdrop kept black alpha 0.85; TitleBarL, Title, TitleBarR mirrored, Subtitle with two thin plain lines; Frame Border T_LevelEnd_Frame Draw As Box margins L/R 0.0955 T/B 0.1878 anchored (0.22,0.242)-(0.78,0.799); inside it the ore band with T_Icon_Ore and OreLevelText + OreRunText stacked (Q1), DividerH Box L 0.069 R 0.066 at about one third of the frame height, two columns each with label, a thin line, and names left / values right (Q2), DividerV Box T 0.057 B 0.058 in the middle under the band; ContinueButton T_Shop_ButtonContinue Box 0.155, tints 1.0 / 1.3 hover / 0.7 pressed / 0.35 disabled, size 416 x 106, centre at 0.861 of the height, ContinueText upper case by the TextBlock transform (Q3)). Start from the asset as it is (clean at planning, 114c7539); if it shows uncommitted owner edits, commit them alone first. Keep the names the graphs use or update the graphs; keep OnRecapShown, the bRunWon branch (Q5) and the NextLevel call. Fonts Roboto Bold as today. No C++, no screen_texts.json change, no other asset.
- agent: bp
- depends_on: task-381-01-level-end-sprites-import
- inputs: agent-bp (vault/reference/agents/agent-bp.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md) (current tree and graphs), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (TitleBar and Continue brush settings, anchor_rule example), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md) (section Sprites: margins), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md) (recap struct, NextLevel), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md) (GetScreenText), `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp` (layout reference), Global request, acceptance and Questions of task-381-level-end-screen-rebuild for the decided values
- expected_output: WBP_Intermission compiled and saved; change summary (tree with anchors, alignment, offsets and size mode per top-level widget, brushes and margins, graph changes); one commit `task-381-02-intermission-widget-rebuild: <summary>` with only WBP_Intermission.uasset.
- done_when: CompileWidgetBlueprint true and compile_blueprint with warnings as errors gives no error; tree read back with the widgets of the acceptance and their brushes and margins; OnRecapShown fills OreLevelText, OreRunText, both columns (names and values), Title, Subtitle and ContinueText for both branches (graph pins read back); ContinueButton still calls NextLevel; saved; committed by path.
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
- [[task-381-level-end-screen-rebuild]] — parent global task.
