---
id: task-383-02-total-ore-widget
description: "Bp: WBP_Intermission shows one Total Ore line in a thinner band and moves the columns up."
parent: task-383-level-end-total-ore
kind: task
status: todo
---
# Stats screen Total Ore: WBP_Intermission

- goal: In /Game/RogueRigs/UI/WBP_Intermission, from the task-381 tree (start from the asset as it is; uncommitted owner edits are committed alone first): replace OreTexts (OreLevelText, OreRunText) by the Total Ore line of Q2 (default: OreLabel "Total Ore:" 32 from Lines[0] and OreValue 48 = RunSecuredOre of the recap, Roboto Bold, outline 2, anchored on the same centre line as OreIcon, no Row, rule umg-layout); band thinner per Q3: DividerH moved up about 0.057 of the screen height (centre 0.4166 to about 0.36), KillsLabel, DamageLabel, KillsLine, DamageLine, KillsRows and DamageRows tops and the DividerV top moved up by the same amount, the OreIcon and the ore line centred in the new band, list bottoms unchanged. Update OnRecapShown (no reference left to the removed texts, Lines[1] no longer read). Nothing else in the widget changes (title, subtitle, lines, button, bRunWon branch, NextLevel).
- agent: bp
- depends_on: task-383-01-total-ore-text
- inputs: agent-bp (vault/reference/agents/agent-bp.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md) (task-381 tree, anchor_rule, graph), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md) (FRRIntermissionRecap, RunSecuredOre), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md) (GetScreenText), `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp` (band reference), Global request, acceptance and Questions of task-383-level-end-total-ore for the decided values
- expected_output: WBP_Intermission compiled and saved; change summary (removed and added widgets, new anchors of every moved widget, graph changes); one commit `task-383-02-total-ore-widget: <summary>` with only WBP_Intermission.uasset.
- done_when: CompileWidgetBlueprint true and compile_blueprint with warnings as errors gives no error; tree read back: OreLevelText and OreRunText gone, the Q2 ore texts present, the moved widgets at their new anchors; graph pins read back: the ore value comes from RunSecuredOre, the label from Lines[0], no Lines[1] read; saved; committed by path.
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
- [[task-383-level-end-total-ore]] — parent global task.
