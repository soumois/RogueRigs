---
id: task-348-02-shop-button-sprites
description: "Art: Buy, Reroll and Continue button plates and the reroll arrow icon after the ChatGPT sheet, keyed to alpha, no text."
parent: task-348-shop-screen-redesign
kind: task
status: done
---
# Shop screen redesign: button sprites

- goal: Make, after groups 7, 8 and 9 of the owner's asset sheet, one plate per button in its normal state with no text (Buy teal, Reroll dark teal with bolted ends, Continue cream rusty metal with teal ends), drawn so they can be 9-sliced horizontally, and the reroll circular arrow icon (light grey, magenta key). Hover, pressed and disabled are made by UMG tint (Q3). Decisions Q1 to Q3 of the global.
- agent: art
- depends_on: task-348-01-shop-chrome-sprites
- inputs: art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md), `Art/Source/shop-screen/reference/chatgpt-shop-asset-sheet-2026-10-28.webp` (the `--ref` image), the sprites retained in task-348-01 (`Art/UI/Shop/`, same style), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), `Tools/art/ui_icon_prepare.js`, global task-348 `## Questions`
- expected_output: prompts, candidates and logs in `Art/Source/shop-screen/`; retained RGBA PNGs in `Art/UI/Shop/`: `T_Shop_ButtonBuy.png` (about 512 x 96), `T_Shop_ButtonReroll.png` (about 640 x 112), `T_Shop_ButtonContinue.png` (about 640 x 128), each with its 9-slice margins written down, and `T_Shop_IconReroll.png` (256 x 256); the "Sprites" section of art-prompts-shop-screen-mockup extended; one commit of the retained files only.
- done_when: four keyed PNGs, clean alpha (`ui_icon_prepare.js --check`), no baked text; at most 2 variants and 1 correction run (Q1); images sent to the owner and retained by him; node updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: sonnet+opus
- tokens: input 52, cache write 142972, cache read 679169, output 18026
- usage: turns 25, context start 10k peak 57k, resumes 1 (message x1), 18 min, units 0.34 M
- runs: 2
- milestone: M2
- started: 2026-10-28T16:01:36Z
- waiting_for: none
- result: Retained Buy c2, Reroll c2, Continue c2, icon c2 in Art/UI/Shop/ (48 credits, no correction run). Plate heights are above the node targets (Buy 137, Reroll 180, Continue 163): adjust in UMG. Resume and units alerts: owner pick finalisation after image review. A6 white-plate look still accepted.

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
