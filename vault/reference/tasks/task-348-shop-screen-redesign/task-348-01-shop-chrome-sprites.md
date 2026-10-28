---
id: task-348-01-shop-chrome-sprites
description: "Art: title side bars, ore counter panel with pipes and riveted shop panel sprites after the ChatGPT sheet, keyed to alpha."
parent: task-348-shop-screen-redesign
kind: task
status: in-progress
---
# Shop screen redesign: screen chrome sprites

- goal: Make, after groups 1, 2 and 3 of the owner's asset sheet, three keyed sprites with no text: the title side bar (left; the right one is its mirror in UMG), the ore counter panel with its pipes (empty: icon and number are widgets), the riveted teal and rust shop panel (empty inside, drawn so it can be 9-sliced). Decisions Q1, Q2 of the global.
- agent: art
- depends_on: none
- inputs: art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md), `Art/Source/shop-screen/reference/chatgpt-shop-asset-sheet-2026-10-28.webp` and `chatgpt-shop-mockup-2026-10-28.webp` (the `--ref` images), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-prompts-card-back (vault/reference/gdd/art-audio/art-prompts-card-back.md) (magenta key and delivery method), `Tools/art/ui_icon_prepare.js` (keying), global task-348 `## Questions`
- expected_output: prompts in `Art/Source/shop-screen/prompts/`, candidates and run logs in `Art/Source/shop-screen/`; retained sprites as RGBA PNG in `Art/UI/Shop/`: `T_Shop_TitleBar.png` (about 512 x 64), `T_Shop_CounterPanel.png` (about 512 x 192), `T_Shop_Panel.png` (about 1024 x 384) with its 9-slice margins written down; a section "Sprites" in art-prompts-shop-screen-mockup (prompts verbatim or their files, credits, margins); one commit of the retained files only.
- done_when: three keyed PNGs exist with clean alpha (no magenta fringe, checked with `ui_icon_prepare.js --check`), no baked text; at most 2 variants and 1 correction run (Q1); images sent to the owner and retained by him; node updated; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T15:40:51Z
- waiting_for: none
- result: open

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
