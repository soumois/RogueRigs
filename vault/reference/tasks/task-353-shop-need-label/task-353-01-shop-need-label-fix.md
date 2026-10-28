---
id: task-353-01-shop-need-label-fix
description: "Bp: WBP_ShopCard layout fix for owner point 3 of the shop capture, display only."
parent: task-353-shop-need-label
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-353-shop-need-label]] — parent global task.
