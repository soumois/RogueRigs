---
id: task-354-01-shop-counter-overlap-fix
description: "Bp: WBP_ShopScreen layout fix for owner point 4 of the shop capture, display only."
parent: task-354-shop-counter-overlap
kind: task
status: todo
---
# Shop screen: crystal counter covers the shop panel corner: WBP_ShopScreen fix

- goal: WBP_ShopScreen: CounterBox / CounterBg placed and sized as decided (Q4) so its bottom edge stays above the shop panel top edge with the decided gap; the panel corner and the first card rivet visible; task-351 and 352 fits kept. OreIcon stays a variable and the ore flight start; every task-342 graph kept. Decided values: the global `## Questions` (as answered in `request`).
- agent: bp
- depends_on: none
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (9-slice margins), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look), `Saved/ShopRedesign/srd_00.png` and `Saved/ShopRedesign/srd_23.png` (look before), global task-354-shop-counter-overlap `## Questions`
- expected_output: WBP_ShopScreen compiled (warnings as errors) and saved; change summary (widgets, slots, sizes, brushes changed); one commit.
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
- [[task-354-shop-counter-overlap]] — parent global task.
