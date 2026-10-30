---
id: art-prompts-level-end-screen
description: "ChatGPT mockup prompt of the end-of-level stats screen (levels 1 to 3) in the Spare Parts look: file, attachment, decisions. Open for that screen's art."
parent: art-prompts
kind: note
---
# Level-end screen — ChatGPT mockup prompt

Written for the owner (task-378, 2026-10-30): the screen shown when the miner reaches the extractor, the stats screen of [[intermission]] drawn by [[wbp-intermission]]. The owner pastes the prompt into ChatGPT himself; no Kie.ai run, no credit, no image yet.

## Files
- Prompt (verbatim, English): `Art/Source/level-end-screen/prompts/chatgpt-level-end-mockup.md`.
- Single attachment: `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp`, the Spare Parts mockup, palette and look reference ([[art-prompts-shop-screen-mockup]]).
- Delivery: one 16:9 full-screen mockup; the answer is saved by the owner, nothing in `vault/assets/` until he validates.

## Decisions (owner 2026-10-30, provisional)
- Q1: the screen is the stats screen at the extractor; "l'écran des cartes" = the Spare Parts shop. No map screen.
- Q2: only the ChatGPT Spare Parts mockup attached, no owner screenshots.
- Q3: English prompt and UI texts.
- Q4: background = the paused arena, dimmed almost black, as the GDD says.
- Q5: levels 1 to 3 only (LEVEL N CLEARED, Next level); the level 4 run-won variant is not in the prompt.
- Q6: prompt only, no generation.

## Content and limits
- Palette and look named in the prompt: gunmetal and teal riveted panels, rust, cream Continue-style button, violet ore, title bar with side bars. Colour rules from [[color-code]].
- Content from the GDD only: title, subtitle, ore this level and run total, robots destroyed per type as text (no icons, `open` in the GDD), damage per weapon, Next level button. No new rule, stat or icon.
- Sample values (Chaser: 8, Burst rifle: 125 come from the verified widget; other names and numbers, the subtitle and the ore labels are placeholders, the real wording is in screen_texts.json).

## Relations
- [[art-prompts]] — parent: method and prompt nodes.
- [[art-prompts-shop-screen-mockup]] — the reference mockup and retained sprites.
- [[intermission]] — the stats screen spec.
- [[wbp-intermission]] — the current widget.
- [[color-code]] — colour rules the prompt follows.
- [[task-378-level-end-screen-prompt]] — the task.
