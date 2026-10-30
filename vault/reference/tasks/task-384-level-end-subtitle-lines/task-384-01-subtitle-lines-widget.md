---
id: task-384-01-subtitle-lines-widget
description: "Bp: WBP_Intermission places the two subtitle lines outside the subtitle text whatever its length."
parent: task-384-level-end-subtitle-lines
kind: task
status: cancelled
---
# Stats screen subtitle lines: WBP_Intermission

- goal: In /Game/RogueRigs/UI/WBP_Intermission (after task-383; start from the asset as it is; uncommitted owner edits are committed alone first): per global Q5 (default: at the end of OnRecapShown, once Subtitle is set, ForceLayoutPrepass then GetDesiredSize of Subtitle; SubtitleLineL slot position x = -(width / 2 + 20), SubtitleLineR x = +(width / 2 + 20), anchors, alignment, size 140 x 2 and colour unchanged), for both the bRunWon and the normal branch. Nothing else changes.
- agent: bp
- depends_on: task-383-04-doc-total-ore
- inputs: agent-bp (vault/reference/agents/agent-bp.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md) (anchor_rule, graph), Global request, acceptance and Questions of task-384-level-end-subtitle-lines for the decided values
- expected_output: WBP_Intermission compiled and saved; change summary (graph nodes added, values); one commit `task-384-01-subtitle-lines-widget: <summary>` with only WBP_Intermission.uasset.
- done_when: CompileWidgetBlueprint true and compile_blueprint with warnings as errors gives no error; graph read back: both branches reach the line placement after the Subtitle SetText; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: Owner 2026-10-30: "Non, annule"; owner validated the screen as is, subtitle lines not fixed.

## Relations
- [[task-384-level-end-subtitle-lines]] — parent global task.
