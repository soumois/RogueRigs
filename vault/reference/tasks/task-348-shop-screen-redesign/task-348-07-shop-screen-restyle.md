---
id: task-348-07-shop-screen-restyle
description: "Bp: WBP_ShopScreen to the mockup: title bars and subtitle, counter panel top left, riveted shop panel, Reroll and Continue plates."
parent: task-348-shop-screen-redesign
kind: task
status: in-progress
---
# Shop screen redesign: WBP_ShopScreen

- goal: Restyle WBP_ShopScreen after the mockup, display only: Title flanked by T_Shop_TitleBar (left, and mirrored right), subtitle from GetScreenSubtitle("shop_screen") (Q4); OreIcon and OreBalanceText moved into T_Shop_CounterPanel at the top left (Q9); ShopPanel brush = T_Shop_Panel as a box brush; RerollButton centred under the panel with T_Shop_ButtonReroll, T_Shop_IconReroll, "REROLL", a divider, ore icon and cost; ContinueButton under it with T_Shop_ButtonContinue and "CONTINUE"; hover, pressed and disabled by tint (Q3). Keep every variable, event and graph of task-342 (OreIcon is still the flight start, button lock): only placement, brushes and new children change. Backdrop stays opaque black. Font Roboto Bold (Q8).
- agent: bp
- depends_on: task-348-06-shop-card-restyle
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (9-slice margins), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look), the label index map in task-348-04 `result`, bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global task-348 `## Questions`
- expected_output: WBP_ShopScreen compiled (warnings as errors) and saved; change summary; one commit.
- done_when: compiled and saved; tree and brushes read back; task-342 flight and lock graphs still present and OreIcon still the flight start; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T16:51:43Z
- waiting_for: none
- result: open

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
