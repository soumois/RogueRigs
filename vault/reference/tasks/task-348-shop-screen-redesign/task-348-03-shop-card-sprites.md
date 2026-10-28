---
id: task-348-03-shop-card-sprites
description: "Art: card tier tab, sold burnt card panel and wrench icon after the ChatGPT sheet, keyed to alpha, no text."
parent: task-348-shop-screen-redesign
kind: task
status: wait-answers
---
# Shop screen redesign: card sprites

- goal: Make, after groups 4 and 6 of the owner's asset sheet: the tier tab (the small trapezoid tab at the top of each card, neutral light grey so UMG tints it per tier, Q5), the sold panel (dark card panel with burnt glowing edges, sized to sit inside the existing stat frame opening, Q10) and the wrench-and-screwdriver sold icon (light grey). No tier frame, no stat icon, no text (Q2). Decisions Q1, Q2, Q5, Q10 of the global.
- agent: art
- depends_on: task-348-02-shop-button-sprites
- inputs: art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md), `Art/Source/shop-screen/reference/chatgpt-shop-asset-sheet-2026-10-28.webp` (the `--ref` image), `Art/Source/card-frames/20261014-141713-card-frames-1.png` (the stat frame the panel sits in), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md) (card stack sizes: CardFace 500 x 720, CardPanel padding), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), `Tools/art/ui_icon_prepare.js`, global task-348 `## Questions`
- expected_output: prompts, candidates and logs in `Art/Source/shop-screen/`; retained RGBA PNGs in `Art/UI/Shop/`: `T_Shop_TierTab.png` (about 256 x 64), `T_Shop_SoldPanel.png` (sized to the frame opening of the 500 x 720 card), `T_Shop_IconSold.png` (256 x 256); the "Sprites" section of art-prompts-shop-screen-mockup extended; one commit of the retained files only.
- done_when: three keyed PNGs, clean alpha (`ui_icon_prepare.js --check`), no baked text; at most 2 variants and 1 correction run (Q1); images sent to the owner and retained by him; node updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T16:19:41Z
- waiting_for: owner choice of tab/sold panel/icon candidates c1/c2
- result: open

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
