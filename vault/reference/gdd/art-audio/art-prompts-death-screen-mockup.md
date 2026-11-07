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

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[death-screen]] — the screen this mockup shows.
- [[art-prompts-level-end-screen-mockup]] — style reference Image 1.
- [[art-prompts-pause-menu-mockup]] — style reference Image 2.
- [[task-484-01-mockup]] — the subtask that made it.
