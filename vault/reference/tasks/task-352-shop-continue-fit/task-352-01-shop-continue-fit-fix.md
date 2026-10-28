---
id: task-352-01-shop-continue-fit-fix
description: "Bp: WBP_ShopScreen layout fix for owner point 2 of the shop capture, display only."
parent: task-352-shop-continue-fit
kind: task
status: done
---
# Shop screen: Continue plate cut at the bottom: WBP_ShopScreen fix

- goal: WBP_ShopScreen: the Reroll and Continue plates drawn inside their target boxes (brush ImageSize and box sizes adjusted in UMG, the plates are taller than their targets) and the vertical gaps under the panel reduced so the Continue plate ends above the screen bottom with the decided margin (Q2); card size unchanged; task-351 title margin kept. Every widget name, variable, event and task-342 graph kept. Decided values: the global `## Questions` (as answered in `request`).
- agent: bp
- depends_on: none
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md) (9-slice margins), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (target look), `Saved/ShopRedesign/srd_00.png` and `Saved/ShopRedesign/srd_23.png` (look before), global task-352-shop-continue-fit `## Questions`
- expected_output: WBP_ShopScreen compiled (warnings as errors) and saved; change summary (widgets, slots, sizes, brushes changed); one commit.
- done_when: compiled and saved; changed slots and brushes read back; widget names and task-342/343/344 graphs still present; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 28, cache write 61244, cache read 578056, output 9206
- usage: turns 14, context start 11k peak 66k, resumes 0, 1 min, units 0.18 M
- runs: 1
- milestone: M2
- started: 2026-10-28T17:41:33Z
- waiting_for: none
- result: WBP_ShopScreen: Reroll box 440x110, Continue box 416x106, brushes at box size, panel/subtitle/reroll gaps reduced (commit eb75ca95); fit estimated only, checked by 02 capture

## Relations
- [[task-352-shop-continue-fit]] — parent global task.
