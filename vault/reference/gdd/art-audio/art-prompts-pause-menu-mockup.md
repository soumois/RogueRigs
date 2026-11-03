---
id: art-prompts-pause-menu-mockup
description: "Pause menu mockup from Kie.ai image-to-image off the level-end mockup: prompt files, images, logs, credits per round, review, owner words."
parent: art-prompts
kind: note
---
# Pause menu mockup (Kie.ai, task-424-01)

Content of [[pause-menu]] (task-424 Q3 defaults), style of [[art-prompts-level-end-screen-mockup]] (Image 1, attached with `--ref`).

## Round 1
- Prompt: `Art/Source/pause-menu/prompts/01-mockup.txt` (verbatim chain: one prompt, no correction).
- Command: `node Tools/art/kie_image.js --asset pause-menu --prompt-file Art/Source/pause-menu/prompts/01-mockup.txt --ref Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp --aspect 16:9 --variants 1`.
- Image: `Art/Source/pause-menu/20261102-222942-pause-menu-1.png` (1K, 16:9). Log: `Art/Source/pause-menu/20261102-222942-pause-menu.json`, model gpt-image-2-image-to-image, ref attached.
- Credits: 6 (round 1). Total 6.
- Delivery format: PNG, 16:9, 1K; mockup only, nothing extracted yet.

## Round 2 (owner change: four weapons)
- Owner words: "it proposes the stat only for 2 weapons. We can have 4 weapons."
- Prompt: `Art/Source/pause-menu/prompts/02-four-weapons.txt` (correction: "Keep this exact image (Image 1)", changes one thing, the weapon area: Miner column left, 2x2 grid of four weapon blocks right).
- Command: `node Tools/art/kie_image.js --asset pause-menu --prompt-file Art/Source/pause-menu/prompts/02-four-weapons.txt --ref Art/Source/pause-menu/20261102-222942-pause-menu-1.png --ref Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp --aspect 16:9 --variants 1`.
- Image: `Art/Source/pause-menu/20261102-223354-pause-menu-1.png`. Log: `Art/Source/pause-menu/20261102-223354-pause-menu.json`, model gpt-image-2-image-to-image, 2 refs.
- Credits: 6 (round 2). Running total 12.
- Review: pass. Four blocks of 7 rows, same font size, aligned, no empty space, frame, title, buttons and Miner column kept; no yellow, green or blue. Gaps: weapon block fonts are slightly smaller than the Miner rows; only three weapons exist in the GDD, so the fourth block is the placeholder "Weapon 4"; numbers are placeholders.

## Review (round 1)
- Content matches Q3: title PAUSED, Miner column (6 stats, then Weapon bonuses with 5 rows), two weapon columns (7 rows each), Resume and Exit buttons, English text.
- Style matches the reference (riveted frame, side bars, cream rusty buttons, dimmed arena, violet crystals). No yellow, green or blue.
- Gaps: the numbers are placeholders chosen for the mockup, not game values; the Miner column is denser than the weapon columns; the weapon bonus rows use a smaller font than the stats rows.

## Owner words
- Round 1: change request, "it proposes the stat only for 2 weapons. We can have 4 weapons."
- Round 2: pending ("je valide" or change request).

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[pause-menu]] — the screen this mockup shows.
- [[art-prompts-level-end-screen-mockup]] — the style reference image.
- [[task-424-01-pause-menu-kie-mockup]] — the subtask that made it.
