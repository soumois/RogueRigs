---
id: task-343-shop-buy-juice
description: "Global task: Clicking an affordable Buy button gives a juicy press reaction of its own, separate from the ore flight."
parent: tasks-done-index
kind: task
status: done
---
# Shop purchase: juicy Buy button click

- request: Point 4 of the owner message: "Il faut un côté juicy aussi lorsqu'on clique sur le bouton (autre que l'effet d'avant)" (owner, 2026-10-28; "l'effet d'avant" read as the ore flight of task-342; full message: "* Crée une tâche pour modifier le GDD et le code, les cristaux mauve ne sont plus reset entre les niveaux. Cette ressource ne sert que pour la run en cours. (et c'est frustrant de ne pas pouvoir cumuler les cristaux sur deux niveaux pour faire de gros achat) / Ok voilà l'écran de sélection des cartes entre deux niveaux, il va falloir faire son design et son gamefeeling. Pour cela: * Il me faut le prompt pour chatgpt qui me fera des propositions et je discuterais avec lui * Lorsque j'achète une carte, je veux mes cristaux qui partent du compte et vont vers le bouton * Il faut un côté juicy aussi lorsqu'on clique sur le bouton (autre que l'effet d'avant) * Il faut le même effet que lorsque je clique sur la carte (sauf l'effet qui bouge jusqu'à l'arme ou l'xp"). Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q11 accepted (listed in gdd_check).
- goal: Clicking an affordable Buy button gives a juicy press reaction of its own, separate from the ore flight.
- acceptance: on a successful purchase the Buy button punches (shrink then overshoot), flashes white and throws tier-coloured sparks (Q10); nothing on a greyed button; also on rr.Shop.Buy; -game frame capture sent to the owner; wbp-shop-card updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[run-shop]], [[color-code]] (tier colours), [[rarities]], tech [[wbp-shop-card]], [[sys-run-shop]]. No conflict; values not in the GDD. Depends on [[task-342-shop-ore-flight]] (OnSlotBought, the card freeze during the sequence). Questions (defaults, values provisional): Q9 points 4 and 5 are two effects: 4 = the Buy button reaction, 5 = the bought card burning (task-344); Q10 the button reaction: scale 1 to 0.88 in 0.06 s then ease-out-back to 1.12 and back to 1 by 0.3 s, a white flash over the button fading in 0.2 s, 8 small sparks in the card tier colour bursting from the button over 0.35 s; no sound (no audio assets yet). Owner answer 2026-10-28: "OK, tout par défaut": every default above is DECIDED (values provisional, owner 2026-10-28); GDD updated by the Planner the same day.
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q11 and the plan, relayed by the main session)
- milestone: M2
- next_step: none
- started: 2026-10-28T06:57:24Z
- waiting_for: none
- executor_tokens: input 38, cache write 89557, cache read 538531, output 8049
- executor_usage: runs 1, turns 18, context start 16k peak 46k, resumes 1, 27 min, units 0.21 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. wbp-shop-card updated with BuyHost/BuyFlash/Spark0..7, StartJuice/JuiceStep/SparkStep, Q10 values and capture verification (8d093514)

## Subtasks
1. task-343-01-shop-buy-juice (vault/reference/tasks/task-343-shop-buy-juice/task-343-01-shop-buy-juice.md) — bp: WBP_ShopCard Buy button punch, flash and tier sparks on purchase.
2. task-343-02-shop-buy-juice-capture (vault/reference/tasks/task-343-shop-buy-juice/task-343-02-shop-buy-juice-capture.md) — cpp: -game frame capture of the Buy button reaction.
3. task-343-03-shop-buy-juice-doc (vault/reference/tasks/task-343-shop-buy-juice/task-343-03-shop-buy-juice-doc.md) — doc: update wbp-shop-card.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-343-01-shop-buy-juice]] — step 1, bp.
- [[task-343-02-shop-buy-juice-capture]] — step 2, cpp.
- [[task-343-03-shop-buy-juice-doc]] — step 3, doc.
