---
id: task-342-02-shop-card-hold
description: "Bp: WBP_ShopCard keeps its face and Buy button during the purchase sequence, sold look after."
parent: task-342-shop-ore-flight
kind: task
status: todo
---
# Shop purchase: ore flies from the balance to the Buy button: WBP_ShopCard keeps its face and Buy button during the purchase sequence, sold look after

- goal: WBP_ShopCard: on OnSlotBought for its own SlotIndex it freezes its face (price row and Buy button visible, no refresh to the sold look) until the parent calls a new public function EndPurchaseFx, which shows the sold look (SetBought). Other slots keep refreshing as today.
- agent: bp
- depends_on: task-342-01-shop-bought-event
- inputs: wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global task-342 `gdd_check`
- expected_output: WBP_ShopCard compiled (warnings as errors) and saved; change summary; one commit.
- done_when: compiled and saved; graphs read back: the bought slot ignores the refresh until EndPurchaseFx; committed.
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
- [[task-342-shop-ore-flight]] — parent global task.
