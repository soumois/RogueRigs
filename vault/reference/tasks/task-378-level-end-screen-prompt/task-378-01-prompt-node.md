---
id: task-378-01-prompt-node
description: "Art: write the ChatGPT prompt of the end-of-level stats screen mockup and its vault node art-prompts-level-end-screen; no generation"
parent: task-378-level-end-screen-prompt
kind: task
status: done
---
# Level-end screen prompt for ChatGPT: art

- goal: Write the prompt file Art/Source/level-end-screen/prompts/chatgpt-level-end-mockup.md and the node art-prompts-level-end-screen (child of art-prompts, linked from it), following the decisions of the global request and gdd_check (Q1 to Q6). The prompt describes the content of the screen shown when the miner reaches the extractor (stats screen) from intermission and wbp-intermission with sample values, keeps the palette and look of the Spare Parts upgrade shop screen as seen on the ChatGPT mockup Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp (context: art-prompts-shop-screen-mockup and the T_Shop sprites in Art/UI/Shop/), lists that mockup as the only attachment (Q2, owner correction), keeps English UI texts and the colour code, and asks for no new rule, stat or icon not in the GDD.
- agent: art
- depends_on: none
- inputs: intermission (vault/reference/gdd/run/intermission.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md), art-prompts-miner (vault/reference/gdd/art-audio/art-prompts-miner.md) (format model of a prompt node), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp (the only attachment of the prompt, palette and look reference), Art/UI/Shop/ (retained Spare Parts sprites)
- expected_output: Art/Source/level-end-screen/prompts/chatgpt-level-end-mockup.md; vault/reference/gdd/art-audio/art-prompts-level-end-screen.md; art-prompts Relations line; one commit task-378-01-prompt-node: <summary>.
- done_when: Both files exist and are committed; the prompt follows Q1 to Q6 as decided, lists the attachments, names the Spare Parts palette and look, and holds only content from intermission and wbp-intermission; art-prompts links the new node; no kie_image.js run; node vault/scripts/lint.js --write clean.
- editor_state: not needed
- token_ceiling: 0.15 M units
- model: sonnet
- tokens: input 14, cache write 37386, cache read 140334, output 6074
- usage: turns 7, context start 11k peak 37k, resumes 0, 1 min, units 0.09 M
- runs: 1
- milestone: M2
- started: 2026-10-30T00:59:57Z
- waiting_for: none
- result: Prompt file and node art-prompts-level-end-screen written and committed (ddb1f566); no generation. Wording of subtitle and ore labels are placeholders, not from screen_texts.json.

## Relations
- [[task-378-level-end-screen-prompt]] — parent global task.
