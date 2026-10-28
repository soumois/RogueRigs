---
id: task-348-04-shop-screen-texts
description: "Text: shop screen labels, subtitle and tier names after the mockup in screen_texts.json, upper case, English."
parent: task-348-shop-screen-redesign
kind: task
status: done
---
# Shop screen redesign: texts

- goal: In `Content/RogueRigs/Data/Text/screen_texts.json`: item `shop_screen` gets the subtitle "RIG SERVICE TERMINAL"; the shop labels read by the cards and the screen become "BUY", "NEED {n} MORE" ({n} filled by the card), "SOLD", "NO REFUNDS.", "REROLL", "CONTINUE", and the five tier names COMMON, UNCOMMON, RARE, EPIC, LEGENDARY are added. Keep the existing label indices the widgets read (Labels[1] to [5] of `intermission_recap`) and append new ones; write down every index used. Exact words: Q4 of the global, no rewording.
- agent: text
- depends_on: task-348-03-shop-card-sprites
- inputs: sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), text-content-schema (vault/reference/tech/pipelines/text-content-schema.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md) (labels read), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (labels read), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), global task-348 `## Questions`
- expected_output: screen_texts.json updated with a valid `run_id`; retrieval log in `Tools/content/logs/`; the label index map in `result`; one commit.
- done_when: the file loads (`rr.Debug.ScreenText shop_screen` and `intermission_recap` log the new strings, or the text tests pass); strings exactly as Q4; index map written; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet+opus
- tokens: input 32, cache write 95154, cache read 491485, output 9869
- usage: turns 16, context start 10k peak 63k, resumes 0, 2 min, units 0.22 M
- runs: 1
- milestone: M2
- started: 2026-10-28T16:40:06Z
- waiting_for: none
- result: screen_texts.json (commit cdaa459c): shop_screen subtitle RIG SERVICE TERMINAL. intermission_recap Labels: 0 Salvage Stall (unchanged), 1 CONTINUE, 2 BUY, 3 SOLD, 4 NEED {n} MORE (card fills n = price - balance), 5 REROLL, 6-8 unchanged, 9 NO REFUNDS., 10 COMMON, 11 UNCOMMON, 12 RARE, 13 EPIC, 14 LEGENDARY. Indices 1-5 rewritten in place (validator rejects duplicates ignoring case). Validator exit 0; rr.Debug.ScreenText not run (no editor). style-guide: all pass; note Salvage Stall vs RIG SERVICE TERMINAL naming.

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
