---
id: task-351-01-shop-title-clip-fix
description: "Bp: WBP_ShopScreen layout fix for owner point 1 of the shop capture, display only."
parent: task-351-shop-title-clip
kind: task
status: todo
---
# Shop screen: title clipped at the top: WBP_ShopScreen fix

- goal: WBP_ShopScreen: the title row (TitleBarL, Title, TitleBarR) and Subtitle fully on screen with a top margin (Q1): Content anchored to the top of the canvas with the decided margin instead of centred, title bars vertically centred on the title text. Placement only: every widget name, variable, event and task-342 graph kept; OreIcon stays the flight start. Decided values: the global `## Questions` (as answered in `request`).
- agent: bp
- depends_on: none
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (9-slice margins), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look), `Saved/ShopRedesign/srd_00.png` and `Saved/ShopRedesign/srd_23.png` (look before), global task-351-shop-title-clip `## Questions`
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
- [[task-351-shop-title-clip]] — parent global task.
