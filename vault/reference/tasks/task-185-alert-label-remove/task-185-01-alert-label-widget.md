---
id: task-185-01-alert-label-widget
description: "Remove the ALERT text (AlertLabel) next to the alert bar in WBP_AlertObjectives and every graph use of it."
parent: task-185-alert-label-remove
kind: task
status: in-progress
---
# Alert bar: remove the ALERT label

- goal: Remove the ALERT text (AlertLabel) next to the alert bar in WBP_AlertObjectives and every graph use of it.
- agent: bp
- depends_on: none
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: AlertLabel removed from WBP_AlertObjectives with its uses: the text set in ShowAlertBar ("ALERT" / "ALERT (paused)"), the Collapsed in HideForOffer and the Visible in RestoreAfterOffer; the bar, its percent and the alert icons unchanged; no pause indicator replaces it (owner default); wbp-alert-objectives doc updated
- done_when: AlertLabel no longer in the widget tree (read back); the graphs compile with warnings as errors; saved by explicit path; doc updated; committed by path; the editor stays open
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-185-alert-label-remove]] — parent global task.
