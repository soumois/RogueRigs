---
id: task-342-shop-ore-flight
description: "Global task: When a shop card is bought, ore crystals fly from the balance counter to that card's Buy button while the balance counts down."
parent: tasks-to-do-index
kind: task
status: todo
---
# Shop purchase: ore flies from the balance to the Buy button

- request: Point 3 of the owner message: "Lorsque j'achète une carte, je veux mes cristaux qui partent du compte et vont vers le bouton" (owner, 2026-10-28; full message: "* Crée une tâche pour modifier le GDD et le code, les cristaux mauve ne sont plus reset entre les niveaux. Cette ressource ne sert que pour la run en cours. (et c'est frustrant de ne pas pouvoir cumuler les cristaux sur deux niveaux pour faire de gros achat) / Ok voilà l'écran de sélection des cartes entre deux niveaux, il va falloir faire son design et son gamefeeling. Pour cela: * Il me faut le prompt pour chatgpt qui me fera des propositions et je discuterais avec lui * Lorsque j'achète une carte, je veux mes cristaux qui partent du compte et vont vers le bouton * Il faut un côté juicy aussi lorsqu'on clique sur le bouton (autre que l'effet d'avant) * Il faut le même effet que lorsque je clique sur la carte (sauf l'effet qui bouge jusqu'à l'arme ou l'xp"). Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q11 accepted (listed in gdd_check).
- goal: When a shop card is bought, ore crystals fly from the balance counter to that card's Buy button while the balance counts down.
- acceptance: buying a card sends ore icons from the balance (bottom left) to the Buy button of that card; the balance counts down as they land; the bought card keeps its face and button until the flight ends, then shows the sold look; Reroll and Continue are blocked during the sequence (Q8); also triggered by rr.Shop.Buy; -game frame capture of a purchase sent to the owner; docs updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[run-shop]] (provisional: shop shown "without the deal, suspense or fly animations"; the owner now asks for a purchase flight, so the line gets a purchase-effects entry once answered), [[intermission]], [[color-code]], tech [[sys-run-shop]], [[wbp-shop-screen]], [[wbp-shop-card]], [[wbp-upgrade-choice]] (FlyStep maths of task-246-02 reusable). Today BuySlot refreshes the card to "Sold" at once and the debug buy does not pass through the card click, so a C++ event OnSlotBought is added (events over polling). owner screenshot of the shop screen saved as `Art/Source/shop-screen/reference/shop-screen-2026-10-28.png` (SPARE PARTS, 4 cards greyed "Short on ore", slot 5 sold, balance 7, Reroll stock 20, Continue). Questions (defaults, all values provisional): Q6 points 3 to 5 are built now on the current screen, before the ChatGPT discussion (a redesign is a later request); Q7 one ore icon per 10 ore of price, 3 minimum, 10 maximum, T_Icon_Ore 40 px, launched 0.05 s apart, 0.5 s each, smoothstep with an upward bulge as the level-up fly; the balance drops by its share as each icon lands; Q8 sequence: click, button juice at once (task-343), ore flight, card burn on the last landing (task-344), then the sold look; every shop button (other cards, Reroll, Continue) is blocked during the sequence, about 1.2 s. Owner answer 2026-10-28: "OK, tout par défaut": every default above is DECIDED (values provisional, owner 2026-10-28); GDD updated by the Planner the same day.
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q11 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-342-01-shop-bought-event
- started: open
- waiting_for: none
- result: open

## Subtasks
1. task-342-01-shop-bought-event (vault/reference/tasks/task-342-shop-ore-flight/task-342-01-shop-bought-event.md) — cpp: URRIntermissionShopSubsystem broadcasts OnSlotBought(SlotIndex, Price, BalanceBefore).
2. task-342-02-shop-card-hold (vault/reference/tasks/task-342-shop-ore-flight/task-342-02-shop-card-hold.md) — bp: WBP_ShopCard keeps its face and Buy button during the purchase sequence, sold look after.
3. task-342-03-shop-ore-flight (vault/reference/tasks/task-342-shop-ore-flight/task-342-03-shop-ore-flight.md) — bp: WBP_ShopScreen flies ore icons from the balance to the Buy button, counts the balance down.
4. task-342-04-shop-ore-flight-capture (vault/reference/tasks/task-342-shop-ore-flight/task-342-04-shop-ore-flight-capture.md) — cpp: -game frame capture of a purchase.
5. task-342-05-shop-ore-flight-doc (vault/reference/tasks/task-342-shop-ore-flight/task-342-05-shop-ore-flight-doc.md) — doc: update sys-run-shop, wbp-shop-card, wbp-shop-screen.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-342-01-shop-bought-event]] — step 1, cpp.
- [[task-342-02-shop-card-hold]] — step 2, bp.
- [[task-342-03-shop-ore-flight]] — step 3, bp.
- [[task-342-04-shop-ore-flight-capture]] — step 4, cpp.
- [[task-342-05-shop-ore-flight-doc]] — step 5, doc.
