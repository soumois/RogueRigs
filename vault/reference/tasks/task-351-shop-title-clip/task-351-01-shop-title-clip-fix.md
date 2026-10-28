---
id: task-351-01-shop-title-clip-fix
description: "Bp: WBP_ShopScreen layout fix for owner point 1 of the shop capture, display only."
parent: task-351-shop-title-clip
kind: task
status: done
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
- model: sonnet
- tokens: input 28, cache write 51210, cache read 454999, output 6596
- usage: turns 14, context start 11k peak 51k, resumes 0, 1 min, units 0.14 M
- runs: 1
- milestone: M2
- started: 2026-10-28T17:35:42Z
- waiting_for: none
- result: Content anchored (0.5,0) alignment (0.5,0) top offset 16; title row slots VerticalAlignment Center; compiled, saved, commit fcc11788; unseen until capture

## Relations
- [[task-351-shop-title-clip]] — parent global task.
