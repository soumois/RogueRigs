---
id: task-177-01-wider-upgrade-cards
description: "Make the upgrade cards wider: 500 x 720 instead of 440 x 720, frames re-baked at the new size, same frame thickness."
parent: task-177-wider-upgrade-cards
kind: task
status: done
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
- model: sonnet
- tokens: input 10, cache write 20757, cache read 63798, output 934
- runs: 1
- milestone: M2
- result: Frames re-baked at 1000 x 1440 (cards 500 x 720), corner scale 0.5, band unchanged (about 24 px at 1x). Opening insets at 1x: stat 28/48/28/50; weapon 28/45/28.5 and real bottom bar 33.5 (the script still stops at a faint line). Script default width 1000. Control sheet updated. 0 credits. Commit 2cad6a7.

## Relations
- [[task-177-wider-upgrade-cards]] — parent global task.
