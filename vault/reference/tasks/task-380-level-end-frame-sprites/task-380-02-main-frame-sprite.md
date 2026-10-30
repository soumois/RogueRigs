---
id: task-380-02-main-frame-sprite
description: "Art: main frame sprite after the sheet, 9-slice (corners only detailed, uniform edges, flat centre), keyed, retained by the owner"
parent: task-380-level-end-frame-sprites
kind: task
status: wait-answers
---
# Level-end screen frame sprites in 3x3 (9-slice): art

- goal: Make the main frame of the level-end screen after the asset sheet (main panel and corner details) and the mockup: teal corner brackets with bolts, riveted gunmetal and rust edges, no text, drawn for 9-slice (Q3): every detail inside the four corner squares, edges uniform strips with no bolt, tab or plate between the corners, centre a flat dark plate. Generate 2 variants (Q5), key, send them to the owner, copy the retained one to `Art/UI/LevelEnd/T_LevelEnd_Frame.png` (about 1024 wide), measure its four margins in px and give the UMG Margin fractions; write them in art-prompts-level-end-screen-mockup, section "Sprites".
- agent: art
- depends_on: task-380-01-archive-mockup
- inputs: art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md, written by task-380-01), `Art/Source/level-end-screen/reference/chatgpt-level-end-asset-sheet-2026-10-30.webp` (Image 1, --ref) and `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp` (Image 2, --ref), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (same method, 9-slice record of T_Shop_Panel and its mid-edge tab gap), art-prompts-card-back (vault/reference/gdd/art-audio/art-prompts-card-back.md) (magenta key and delivery method), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (how UMG uses the margins: Draw As Box, Margin as a fraction of the texture), `Tools/art/ui_icon_prepare.js` (keying, --keep-aspect, --check), global task-380-level-end-frame-sprites `## Questions` (decisions Q1 to Q6)
- expected_output: prompt `Art/Source/level-end-screen/prompts/01-main-frame.txt`; candidates and run log in `Art/Source/level-end-screen/`; retained `Art/UI/LevelEnd/T_LevelEnd_Frame.png` (RGBA); section "Sprites" in art-prompts-level-end-screen-mockup (prompt file, run id, credits, retained candidate, size, margins px and fraction, gaps); one commit of the retained file, its prompt and run log only.
- done_when: T_LevelEnd_Frame.png exists, `ui_icon_prepare.js --check` clean (no magenta, no pink spill), no baked text; no detail between the corners on any edge and a flat centre, as stated by the agent and seen by the owner; margins written in px and fraction; at most 2 variants and 1 correction run; images sent to the owner and retained by him; node updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T01:35:43Z
- waiting_for: owner pick of candidate 1 or 2 of run 20261029-173616-level-end-screen
- result: open

## Relations
- [[task-380-level-end-frame-sprites]] — parent global task.
