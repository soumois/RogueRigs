---
id: task-344-shop-card-burn
description: "Global task: A bought shop card plays the level-up pick burn, without the flight to the weapon or XP bar, then shows the sold look."
parent: tasks-done-index
kind: task
status: done
---
# Shop purchase: the bought card burns like a level-up pick

- request: Point 5 of the owner message: "Il faut le même effet que lorsque je clique sur la carte (sauf l'effet qui bouge jusqu'à l'arme ou l'xp" (owner, 2026-10-28; "la carte" read as the level-up card pick of task-246-02; full message: "* Crée une tâche pour modifier le GDD et le code, les cristaux mauve ne sont plus reset entre les niveaux. Cette ressource ne sert que pour la run en cours. (et c'est frustrant de ne pas pouvoir cumuler les cristaux sur deux niveaux pour faire de gros achat) / Ok voilà l'écran de sélection des cartes entre deux niveaux, il va falloir faire son design et son gamefeeling. Pour cela: * Il me faut le prompt pour chatgpt qui me fera des propositions et je discuterais avec lui * Lorsque j'achète une carte, je veux mes cristaux qui partent du compte et vont vers le bouton * Il faut un côté juicy aussi lorsqu'on clique sur le bouton (autre que l'effet d'avant) * Il faut le même effet que lorsque je clique sur la carte (sauf l'effet qui bouge jusqu'à l'arme ou l'xp"). Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q11 accepted (listed in gdd_check).
- goal: A bought shop card plays the level-up card pick effect, without the flight to the weapon or XP bar, before showing the sold look.
- acceptance: after the ore flight lands, the bought card burns away with MI_CardBurn tinted by its tier and its frame fades (Q11), then shows "Sold. No refunds."; no icon flies to a weapon slot or the XP bar; the other cards are untouched; -game frame capture sent to the owner; docs updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[run-shop]], [[xp-level-up]], tech [[wbp-upgrade-choice]] (pick animation: non-picked cards burn with MI_CardBurn over 0.5 s, CardBorder fades, icons fly to the weapon slot or XP bar; burn fixes: Index -1 parameter info, FitOverlay), [[m-card-burn]], [[wbp-shop-card]]. In the level-up screen the burn hits the cards NOT picked; in the shop the other cards stay on sale, so the effect is moved to the bought card. Depends on [[task-342-shop-ore-flight]] (EndPurchaseFx). Questions (defaults, values provisional): Q11 the bought card burns away (Burn 0 to 1 over 0.5 s, Tint = tier colour, Tex = T_CardFrame_Stat_Baked, frame and content fading as CardBorder does), starting when the last ore icon lands, then the sold look; the other cards do nothing. Owner answer 2026-10-28: "OK, tout par défaut": every default above is DECIDED (values provisional, owner 2026-10-28); GDD updated by the Planner the same day.
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q11 and the plan, relayed by the main session)
- milestone: M2
- next_step: none
- started: 2026-10-28T07:24:20Z
- waiting_for: none
- executor_tokens: input 50, cache write 66199, cache read 673501, output 7939
- executor_usage: runs 1, turns 22, context start 16k peak 48k, resumes 3, 497 min, units 0.19 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. wbp-shop-card (BurnOverlay, vars, StartBurn/BurnStep/EndBurn, Tick branch, EndPurchaseFx contract, verified_burn) and m-card-burn (shop card as second user) updated; commit 6d56b0f1; lint OK; burn easing and fade shape left open (binary asset)

## Subtasks
1. task-344-01-shop-card-burn (vault/reference/tasks/task-344-shop-card-burn/task-344-01-shop-card-burn.md) — bp: WBP_ShopCard burns the bought card with MI_CardBurn, then the sold look.
2. task-344-02-shop-card-burn-capture (vault/reference/tasks/task-344-shop-card-burn/task-344-02-shop-card-burn-capture.md) — cpp: -game frame capture of the bought card burning.
3. task-344-03-shop-card-burn-doc (vault/reference/tasks/task-344-shop-card-burn/task-344-03-shop-card-burn-doc.md) — doc: update wbp-shop-card and m-card-burn.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-344-01-shop-card-burn]] — step 1, bp.
- [[task-344-02-shop-card-burn-capture]] — step 2, cpp.
- [[task-344-03-shop-card-burn-doc]] — step 3, doc.
