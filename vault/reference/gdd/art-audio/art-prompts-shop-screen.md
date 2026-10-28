---
id: art-prompts-shop-screen
description: "ChatGPT text prompt (French) to get design and game-feel proposals for the shop screen, with the owner's screenshot. Open to reuse or update it."
parent: art-prompts
kind: note
---
# Shop screen — ChatGPT design prompt

Not an image prompt: a text prompt the owner pastes into ChatGPT, with the screenshot attached, to discuss design and game feel of the cards shop ([[run-shop]], [[intermission]]). Made by [[task-341-01-shop-design-prompt]] (owner request 2026-10-28, "OK, tout par défaut"). No image generated, no Kie.ai run, no credits.

## Files
- Prompt, verbatim, French (about 1500 words, too long to copy into a node): `Art/Source/shop-screen/prompts/chatgpt-shop-design.md`.
- Screenshot to attach (owner, 2026-10-28; SPARE PARTS, 4 cards greyed "Short on ore", slot 5 sold, balance 7, "Reroll stock 20", Continue): `Art/Source/shop-screen/reference/shop-screen-2026-10-28.png`.

## What the prompt holds
1. Pitch, pillars P1 to P3, no AI at runtime ([[gdd-pillars]]).
2. Visual style, colour code, readability rules, tier colours, English UI, no audio assets yet ([[visual-style]], [[color-code]], [[readability-rules]]).
3. Technical limits: 1920 x 1080, UMG, current tree of [[wbp-shop-screen]] and [[wbp-shop-card]], the level-up pick animation of [[wbp-upgrade-choice]] for comparison.
4. Fixed rules with values from [[run-shop]]: 5 stat cards, prices grey 25, green 40, blue 60, purple 80, yellow 100, reroll 15 then +5 per reroll (reset each intermission), several buys, greyed if unaffordable, balance, Continue, fade 0.3 s out and 0.5 s in ([[intermission]]). Card stat values stay open ([[rarities]]).
5. The three planned purchase effects ([[task-342-shop-ore-flight]], [[task-343-shop-buy-juice]], [[task-344-shop-card-burn]]).
6. What is open to proposals: layout, hierarchy, card look, buttons, balance, greyed and sold states, motion, sound events, text.
7. Expected answer: numbered proposals (5 or more), each with the principle, why (pillar P1, colour code), UMG cost (low, medium, high), risks; then a recommendation and questions.

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[run-shop]] — the rules the prompt states as fixed.
- [[intermission]] — the screen that hosts the shop.
- [[gdd-pillars]] — pitch and pillars given to ChatGPT.
- [[visual-style]] — style given to ChatGPT.
- [[color-code]] — colours given to ChatGPT.
- [[readability-rules]] — readability and rarity colour rules.
- [[rarities]] — tiers, stat values open.
- [[wbp-shop-screen]] — current screen tree.
- [[wbp-shop-card]] — current card tree.
- [[wbp-upgrade-choice]] — level-up screen used as comparison.
- [[task-341-01-shop-design-prompt]] — the subtask that made it.
- [[task-342-shop-ore-flight]] — planned ore flight to the Buy button.
- [[task-343-shop-buy-juice]] — planned Buy button juice.
- [[task-344-shop-card-burn]] — planned bought card burn.
- [[art-prompts-shop-screen-mockup]] — ChatGPT's answer: mockup and asset sheet.
