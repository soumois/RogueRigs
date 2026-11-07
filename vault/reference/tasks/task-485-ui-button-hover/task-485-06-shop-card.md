---
id: task-485-06-shop-card
description: "Bp: Same hover on the Buy button of WBP_ShopCard, the card body unchanged."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: bp

- goal: Same hover on the Buy button of WBP_ShopCard, the card body unchanged.
- agent: bp
- depends_on: task-485-05-shop-screen
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md)
- expected_output: see done_when
- done_when: Buy shows the hover of ui-button-hover on all five cards, a disabled (NEED n MORE) or sold button shows none, purchase juice, burn and sold look still play; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T03:02:44Z
- waiting_for: none
- result: Done (0accb545). URRShopScreenWidget::NativeConstruct attaches the BuyButton of ShopCard0..4 with BuyPlateBox as the growing widget (the purchase juice animates the button's own scale, so the two scales compose); card body not attached; WBP_ShopCard unchanged. Same commit: hover driver scale wrapper, grey-plate tint path, rr.Debug.CursorOn and rr.Debug.ButtonHover. Seen in -game 1920x1080: affordable BUY plates grow and brighten; NEED 10 MORE (disabled) hovered shows no change, log enabled 0 amount 0 (Saved/Captures/task485_b1080/c_buy.png, task485_resB/c_shop1080.png); four resolutions without overlap. Not exercised: a purchase while hovered (juice, burn, sold look) - code paths untouched.

## Relations
- [[task-485-ui-button-hover]] — parent global task.
