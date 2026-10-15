---
id: task-177-02-wider-cards-layout
description: "Use the re-baked 500 x 720 frames in WBP_UpgradeChoice and widen the cards and their content."
parent: task-177-wider-upgrade-cards
kind: task
status: in-progress
---
# Upgrade cards: wider layout

- goal: Use the re-baked 500 x 720 frames in WBP_UpgradeChoice and widen the cards and their content.
- agent: bp
- depends_on: task-177-01-wider-upgrade-cards
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md); report of task-177-01-wider-upgrade-cards
- expected_output: Re-imported baked frames (reimport or delete and import, same names); CardSize for a 500 x 720 stack, 40 px gap kept; CardPanel, CardBox, WeaponBody paddings from the new insets; ImageArea widened with the card; doc updated.
- done_when: Compiles, saved, sizes read back, doc updated, committed. Owner checks on screen.
- editor_state: open
- token_ceiling: 40 turns
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-177-wider-upgrade-cards]] — parent global task.
