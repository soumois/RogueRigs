---
id: task-355-01-shop-card-frame-details-fix
description: "Bp: WBP_ShopCard layout fix for owner point 5 of the shop capture, display only."
parent: task-355-shop-card-frame-details
kind: task
status: todo
---
# Shop card: second frame line, cramped tier tab, sold panel fit: WBP_ShopCard fix

- goal: Start from the assets as left by tasks 356 and 357 (anchored layout; read the trees first). Find which widget draws the second frame line beside slot 0 (WBP_ShopScreen panel or card slot, or WBP_ShopCard frame) and remove it; TierTabText at the decided size so LEGENDARY fits the tab with margins; SoldPanel fitted to the frame opening by anchors and padding (no fixed 450 x 600, rule umg-layout) so the grey frame shows around the burnt panel as in the mockup (Q5). Every widget name, variable, function and task-342/343/344 graph and animation kept. Decided values: the global `## Questions` (as answered in `request`).
- agent: bp
- depends_on: none
- inputs: wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (9-slice margins), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look), `Saved/ShopFix/task-357/` frames (look before), global task-355-shop-card-frame-details `## Questions`, umg-layout (vault/reference/tech/conventions/umg-layout.md) (the rule to apply), change summaries of task-356-01, 356-02 and 357-01
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
- [[task-355-shop-card-frame-details]] — parent global task.
