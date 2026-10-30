---
id: task-380-01-archive-mockup
description: "Art: save the owner mockup and asset sheet in the repo and write the node art-prompts-level-end-screen-mockup; no generation"
parent: task-380-level-end-frame-sprites
kind: task
status: in-progress
---
# Level-end screen frame sprites in 3x3 (9-slice): art

- goal: Copy the two owner images as received (no conversion) from `C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\3b60b544-be82-4f29-b4f8-b34a9666b878\images\1.webp` (mockup) and `...\images\2.webp` (asset sheet, same folder) to `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp` and `Art/Source/level-end-screen/reference/chatgpt-level-end-asset-sheet-2026-10-30.webp` (Q6). Write the node art-prompts-level-end-screen-mockup (child of art-prompts, model: art-prompts-shop-screen-mockup): owner words, files with pixel sizes, contents of each image, elements taken and reused and not taken (Q2), the 3x3 method (Q3, Q4), budget (Q5); add a Relations line in art-prompts and in art-prompts-level-end-screen. If the temp files are gone, stop and report: the owner must send them again.
- agent: art
- depends_on: none
- inputs: art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (format model), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-level-end-screen (vault/reference/gdd/art-audio/art-prompts-level-end-screen.md), global task-380-level-end-frame-sprites `## Questions`
- expected_output: two webp files in Art/Source/level-end-screen/reference/; vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md; Relations lines in art-prompts and art-prompts-level-end-screen; one commit task-380-01-archive-mockup: <summary>.
- done_when: Both files exist, byte-identical to the sources; the node exists with files, contents, decisions Q2 to Q6, and is linked both ways; no kie_image.js run; node vault/scripts/lint.js --write clean; committed by path.
- editor_state: not needed
- token_ceiling: 0.1 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T01:34:40Z
- waiting_for: none
- result: open

## Relations
- [[task-380-level-end-frame-sprites]] — parent global task.
