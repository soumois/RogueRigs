---
id: task-177-02-wider-cards-layout
description: "Use the re-baked 500 x 720 frames in WBP_UpgradeChoice and widen the cards and their content."
parent: task-177-wider-upgrade-cards
kind: task
status: done
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
- model: sonnet
- tokens: input 50, cache write 55705, cache read 993708, output 695
- runs: 1
- milestone: M2
- result: Stat, accents and weapon baked textures re-imported at 1000 x 1440 (brushes re-assigned); frame images 500 x 720; CardSize 512 x 732 (40 px gap kept, row 1616 px of 1920); CardPanel 14/24/14/25 (shared by both card kinds), CardBox 38/58/38/60, WeaponBody 38/56/38/44, ImageArea 408 x 320, WeaponImgBox 408 x 290. Glow texture left at 880 x 1440 until 178-02. Compiled with warnings as errors, saved, read back, doc updated. Not seen on screen. Commit b2e7062.

## Relations
- [[task-177-wider-upgrade-cards]] — parent global task.
