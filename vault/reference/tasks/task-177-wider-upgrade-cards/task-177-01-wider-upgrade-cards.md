---
id: task-177-01-wider-upgrade-cards
description: "Make the upgrade cards wider: 500 x 720 instead of 440 x 720, frames re-baked at the new size, same frame thickness."
parent: task-177-wider-upgrade-cards
kind: task
status: in-progress
---
# Upgrade cards: wider cards

- goal: Make the upgrade cards wider: 500 x 720 instead of 440 x 720, frames re-baked at the new size, same frame thickness.
- agent: art
- depends_on: none
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: Tools/art/card_frame_bake.js run at 1000 x 1440 (500 x 720 at 2x), same corner scale 0.5: the four *_Baked PNGs in Art/UI/Cards/ regenerated (same names), opening insets measured and reported, control sheet updated.
- done_when: Four PNGs 1000 x 1440, band thickness unchanged, insets reported, committed. No Kie credits.
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-177-wider-upgrade-cards]] — parent global task.
