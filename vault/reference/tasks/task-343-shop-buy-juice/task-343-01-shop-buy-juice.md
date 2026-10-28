---
id: task-343-01-shop-buy-juice
description: "Bp: WBP_ShopCard Buy button punch, flash and tier sparks on purchase."
parent: task-343-shop-buy-juice
kind: task
status: todo
---
# Shop purchase: juicy Buy button click: WBP_ShopCard Buy button punch, flash and tier sparks on purchase

- goal: WBP_ShopCard: on OnSlotBought for its own slot, play the Buy button reaction of Q10 (punch, white flash, tier-coloured sparks), widget tick on real time, display only.
- agent: bp
- depends_on: none
- inputs: wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), color-code (vault/reference/gdd/art-audio/color-code.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md) (tick on real time, tier colours), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global task-343 `gdd_check`
- expected_output: WBP_ShopCard compiled (warnings as errors) and saved; change summary; one commit.
- done_when: compiled and saved; graphs read back; no reaction on a refused buy; committed.
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
- [[task-343-shop-buy-juice]] — parent global task.
