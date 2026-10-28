---
id: task-353-01-shop-need-label-fix
description: "Bp: WBP_ShopCard layout fix for owner point 3 of the shop capture, display only."
parent: task-353-shop-need-label
kind: task
status: done
---
# Shop card: NEED N MORE overflows the Buy plate: WBP_ShopCard fix

- goal: WBP_ShopCard: when the card is not affordable, BuyText shows ShortLabel at the decided smaller size on the Buy plate greyed as decided (Q3), inside the plate with side margins for "NEED 100 MORE"; BUY keeps its size; the plate drawn inside its target (ImageSize adjusted in UMG if it is taller than BuyHost). Every widget name, variable, function and task-343/344 graph kept. Decided values: the global `## Questions` (as answered in `request`).
- agent: bp
- depends_on: none
- inputs: wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (9-slice margins), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look), `Saved/ShopRedesign/srd_00.png` and `Saved/ShopRedesign/srd_23.png` (look before), global task-353-shop-need-label `## Questions`
- expected_output: WBP_ShopCard compiled (warnings as errors) and saved; change summary (widgets, slots, sizes, brushes changed); one commit.
- done_when: compiled and saved; changed slots and brushes read back; widget names and task-342/343/344 graphs still present; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 52, cache write 61963, cache read 1196338, output 14097
- usage: turns 26, context start 11k peak 67k, resumes 0, 3 min, units 0.27 M
- runs: 1
- milestone: M2
- started: 2026-10-28T17:47:19Z
- waiting_for: none
- result: WBP_ShopCard: SetFontSize 28 (affordable) / 20 (NEED n MORE) after SetText in ApplyShopLook; BuyFlash ImageSize 256x68 to keep plate height; commit 68d93584; unrendered, fit of NEED 100 MORE estimated ~150px in 230 min width

## Relations
- [[task-353-shop-need-label]] — parent global task.
