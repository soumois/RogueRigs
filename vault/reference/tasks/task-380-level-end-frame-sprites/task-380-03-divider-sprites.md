---
id: task-380-03-divider-sprites
description: "Art: horizontal and vertical divider sprites with bolted ends, 3-slice, keyed, retained by the owner"
parent: task-380-level-end-frame-sprites
kind: task
status: done
---
# Level-end screen frame sprites in 3x3 (9-slice): art

- goal: Make after the sheet (divider components) and the mockup the two dividers that split the frame: a horizontal bar (Total Ore band from the columns) and a vertical bar (between the two columns), gunmetal with teal and bolted ends matching T_LevelEnd_Frame, no text, drawn for 3-slice (Q3): details only at the two ends, uniform middle. 2 variants each (Q5), key, send to the owner, copy the retained ones to `Art/UI/LevelEnd/T_LevelEnd_DividerH.png` and `T_LevelEnd_DividerV.png`, measure the end margins in px and fraction; write them in art-prompts-level-end-screen-mockup, section "Sprites". The retained frame T_LevelEnd_Frame.png may be passed as an extra ref for consistency (say which is Image 1, 2, 3).
- agent: art
- depends_on: task-380-02-main-frame-sprite
- inputs: art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md, written by task-380-01), `Art/Source/level-end-screen/reference/chatgpt-level-end-asset-sheet-2026-10-30.webp` (Image 1, --ref) and `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp` (Image 2, --ref), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (same method, 9-slice record of T_Shop_Panel and its mid-edge tab gap), art-prompts-card-back (vault/reference/gdd/art-audio/art-prompts-card-back.md) (magenta key and delivery method), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (how UMG uses the margins: Draw As Box, Margin as a fraction of the texture), `Tools/art/ui_icon_prepare.js` (keying, --keep-aspect, --check), global task-380-level-end-frame-sprites `## Questions` (decisions Q1 to Q6), `Art/UI/LevelEnd/T_LevelEnd_Frame.png` (retained frame, for matching style)
- expected_output: prompts `Art/Source/level-end-screen/prompts/02-divider-h.txt`, `03-divider-v.txt`; candidates and run logs in `Art/Source/level-end-screen/`; retained `Art/UI/LevelEnd/T_LevelEnd_DividerH.png` and `T_LevelEnd_DividerV.png` (RGBA); "Sprites" section updated; one commit of the retained files, prompts and run logs only.
- done_when: Both PNGs exist, `ui_icon_prepare.js --check` clean, no baked text; details only at the ends, uniform middle; end margins written in px and fraction; at most 2 variants per sprite and 1 correction run; images sent to the owner and retained by him; node updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: sonnet+opus
- tokens: input 56, cache write 83215, cache read 512927, output 20209
- usage: turns 27, context start 11k peak 41k, resumes 1 (message x1), 8 min, units 0.26 M
- runs: 3
- milestone: M2
- started: 2026-10-30T01:48:54Z
- waiting_for: none
- result: Owner retained H1 + V1 as is (24 credits, runs 20261029-174247 and 174426); T_LevelEnd_DividerH 984x62 margins L 68 / R 65 px (0.069/0.066), T_LevelEnd_DividerV 58x984 margins T 56 / B 57 px (0.057/0.058); Sprites section updated; commit 20ddfe24. Style-guide A6 (cleaner than frame) accepted by owner. Runs 3 = generate, review, close-out.

## Relations
- [[task-380-level-end-frame-sprites]] — parent global task.
