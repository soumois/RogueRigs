---
id: art-prompts-death-screen-mockup
description: "Death screen mockup from Kie.ai image-to-image off the level-end and pause mockups: prompt file, image, log, credits per round, review, owner words."
parent: art-prompts
kind: note
---
# Death screen mockup (Kie.ai, task-484-01)

Content of [[death-screen]] (task-484 Q1 to Q10 defaults), style of [[art-prompts-level-end-screen-mockup]] (Image 1) and [[art-prompts-pause-menu-mockup]] (Image 2, validated pause mockup), both attached with `--ref`.

## Round 1
- Prompt: `Art/Source/death-screen/prompts/01-mockup.txt` (one prompt, no correction).
- Command: `node Tools/art/kie_image.js --asset death-screen --prompt-file Art/Source/death-screen/prompts/01-mockup.txt --ref Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp --ref Art/Source/pause-menu/20261102-223354-pause-menu-1.png --aspect 16:9 --variants 1`.
- Image: `Art/Source/death-screen/20261106-165747-death-screen-1.png` (1K, 16:9). Log: `Art/Source/death-screen/20261106-165747-death-screen.json`, model gpt-image-2-image-to-image, 2 refs.
- Credits: 6. Total 6.
- Content: title "CONTRACT TERMINATED" in rust red with riveted side bars; subtitle "Unauthorized personnel neutralized. Claim returned to Kessler-Voss." between thin lines; a smaller riveted teal and gunmetal frame with three rows (Level reached, Level 2 of 4; Robots destroyed, 47; Duskite lost, 61; placeholder numbers); Retry and Main Menu cream rusty buttons; level dimmed and desaturated behind (grey crystals).

## Review (round 1)
- Style guide: pass, image and both text lines, no fail (`Art/Source/death-screen/20261106-165747.review.json`). Notes for the owner: the whole title is red where the reference titles are white (a white title with red accents is the alternative); no Duskite icon on the "Duskite lost" row.
- Self-check: every death-screen element present, no yellow, green or blue; Retry is barely brighter than Main Menu.

## Owner words
- Round 1: "Je valide" (owner 2026-11-07, relayed by the main session). Round 1 is the validated mockup; status keep.

## Elements (task-484-02)
Every element of the validated round 1 maps to an existing sprite: no new PNG, no generation, 0 credits (cap 24, unused). The mockup is the layout reference only; sizes and margins as [[art-prompts-pause-menu-mockup]] Elements.

| Element | Reused asset | Margins (px, UMG fraction) | Credits |
|---|---|---|---|
| Recap frame | `Art/UI/LevelEnd/T_LevelEnd_Frame.png` 942 x 458, Draw As Box | L, R 90 = 0.0955; T, B 86 = 0.1878 | 0 |
| Title side rails | `Art/UI/Shop/T_Shop_TitleBar.png` 512 x 69, left bar, UMG mirrors it for the right | none (plain image) | 0 |
| Retry, Main Menu | `Art/UI/Shop/T_Shop_ButtonContinue.png` 640 x 163, hover and pressed by UMG tint | L, R about 95 to 105 (estimate from task-424-02), T, B 0 | 0 |
| Title (red), subtitle, recap labels and values | UMG text | none | 0 |
| Lines beside the subtitle, lines between recap rows | plain UMG lines (as level-end and pause) | none | 0 |
| Dimmed, desaturated level behind | UMG dark overlay plus a desaturation effect (no sprite) | none | 0 |
| Duskite icon | not in the mockup, not used | none | 0 |

Gaps: the mockup title rails carry a round bolt at the inner end, `T_Shop_TitleBar` is the same family (accepted on level-end and pause). Title colour: rust red of the mockup ([[color-code]] red accent).

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[death-screen]] — the screen this mockup shows.
- [[art-prompts-level-end-screen-mockup]] — style reference Image 1.
- [[art-prompts-pause-menu-mockup]] — style reference Image 2.
- [[task-484-01-mockup]] — the subtask that made it.
- [[task-484-02-elements]] — maps the mockup elements to reused sprites.
