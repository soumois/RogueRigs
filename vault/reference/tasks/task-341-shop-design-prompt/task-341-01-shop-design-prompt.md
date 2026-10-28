---
id: task-341-01-shop-design-prompt
description: "Art: write the ChatGPT prompt and its vault prompt node."
parent: task-341-shop-design-prompt
kind: task
status: done
---
# ChatGPT prompt for the shop screen design and game feel: write the ChatGPT prompt and its vault prompt node

- goal: Write a ready-to-paste ChatGPT prompt (language per Q4) asking for design and game-feel proposals for the shop screen and a discussion: context from the input nodes (pitch, pillars, visual style, colour code, readability rules, shop rules and prices, current layout from the screenshot, 1920x1080, UMG widgets, no runtime AI, no audio assets yet), the purchase effects already planned (ore flight to the Buy button, Buy button juice, bought card burn), what is fixed (rules, prices) versus open to proposals (layout, hierarchy, card look, motion, sound, feedback), and the expected answer form (several numbered proposals, each with the why and a cost estimate in UMG). No image generation.
- agent: art
- depends_on: none
- inputs: run-shop (vault/reference/gdd/run/run-shop.md), intermission (vault/reference/gdd/run/intermission.md), rarities (vault/reference/gdd/weapons/rarities.md), gdd-pillars (vault/reference/gdd/gdd-pillars.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md) (pick animation section only), screenshot `Art/Source/shop-screen/reference/shop-screen-2026-10-28.png`, global task-341 `request` and `gdd_check`
- expected_output: `Art/Source/shop-screen/prompts/chatgpt-shop-design.md` (the prompt, verbatim); a prompt node `art-prompts-shop-screen` child of art-prompts (purpose, file paths, screenshot path, the prompt); art-prompts Children updated; one commit.
- done_when: prompt file and node exist; the prompt names every fixed rule with its value from run-shop and invents none; it references the screenshot; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: sonnet
- tokens: input 18, cache write 61120, cache read 343015, output 9943
- usage: turns 9, context start 10k peak 61k, resumes 0, 1 min, units 0.16 M
- runs: 1
- milestone: M2
- started: 2026-10-28T06:28:01Z
- waiting_for: none
- result: ChatGPT prompt (French, 7 sections) saved at Art/Source/shop-screen/prompts/chatgpt-shop-design.md; node art-prompts-shop-screen holds a summary (prompt over node size limit); no image generated; lint clean.

## Relations
- [[task-341-shop-design-prompt]] — parent global task.
