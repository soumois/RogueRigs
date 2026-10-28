---
id: task-344-01-shop-card-burn
description: "Bp: WBP_ShopCard burns the bought card with MI_CardBurn, then the sold look."
parent: task-344-shop-card-burn
kind: task
status: in-progress
---
# Shop purchase: the bought card burns like a level-up pick: WBP_ShopCard burns the bought card with MI_CardBurn, then the sold look

- goal: WBP_ShopCard: EndPurchaseFx starts the burn of Q11 (a burn overlay on the card face with a dynamic MI_CardBurn, fitted to the scaled frame, tier tint, face fading), then SetBought at the end. Reuse the level-up pick lessons (Index -1 in MakeMaterialParameterInfo, GetDynamicMaterial on the Image itself, overlay fit). Display only.
- agent: bp
- depends_on: none
- inputs: wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md) (pick animation, pick fixes, burn real cause sections), m-card-burn (vault/reference/tech/blueprints/ui/m-card-burn.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global task-344 `gdd_check`
- expected_output: WBP_ShopCard compiled (warnings as errors) and saved; change summary; one commit.
- done_when: compiled and saved; graphs read back (parameter info index -1, overlay fit, SetBought at the end); committed.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T07:24:20Z
- waiting_for: none
- result: open

## Relations
- [[task-344-shop-card-burn]] — parent global task.
