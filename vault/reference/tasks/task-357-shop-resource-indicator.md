---
id: task-357-shop-resource-indicator
description: "Global task: Shop resource indicator (IndicatorRessource): ore icon and value centred on the font line without a Row container, following the screen-scaled font."
parent: tasks-done-index
kind: task
status: done
---
# Shop screen: resource indicator centred on its font

- request: owner, 2026-10-28, verbatim message quoted in full in [[task-356-shop-screen-anchors]] `request`; this task is its point: "J'ai modifié ce qu'il y avait dans Indicator Ressource, mais même là je ne suis pas convaincu du container "row", car je voudrais que ça se centre par rapport à la taille de police qui s'adapte à l'écran." Relayed context: start from the owner's hand-edited WBP_ShopScreen (committed as-is by task-356-01); the layout must stay simple to edit by hand. Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q7 accepted (Q3 = option a); values provisional, owner 2026-10-28. Q3 = option (a). Q4 to Q7 in task-356. Run order confirmed: 356, then 357, then 355.
- goal: inside IndicatorRessource (placed top left by task-356), the ore icon and the balance value are centred on the same line defined by the text, with no HorizontalBox Row; the icon size is tied to the font size, so both scale together with the screen (DPI scale); OreIcon stays the ore-flight start.
- acceptance: -game captures at the four resolutions of task-356 Q4: icon and value vertically centred on each other and inside the counter panel; balance values 0, 2 digits and 3 digits do not move the icon off the panel; no Row container left in IndicatorRessource; the ore flight still starts at OreIcon and the balance counts down; doc states the anchors used; lint clean; committed.
- gdd_check: read 2026-10-28 by the Planner: [[run-shop]] (ore counter in a pipe-framed panel at the top left, provisional), tech [[wbp-shop-screen]] (CounterBg, OreIcon, OreBalanceText, ore_flight CenterOf(OreIcon), RefreshShop writes OreBalanceText). No conflict, display only. No `open` GDD value needed. Font scaling: no `UserInterfaceSettings` override in `Config/*.ini`, engine default DPI rule (read back by task-356-01). Binary scan of the owner's asset: IndicatorRessource holds CounterOverlay, CounterBg and CounterRow (the "row" the owner doubts). Order: after [[task-356-shop-screen-anchors]] done (same asset, one agent at a time); then [[task-355-shop-card-frame-details]]. Owner "OK, tout par défaut": Q3 option (a) DECIDED (provisional, owner 2026-10-28); [[run-shop]] updated by the Planner; rule [[umg-layout]] applies.
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q7 and the plan, relayed by the main session)
- milestone: M2
- next_step: none
- started: 2026-10-28T21:37:43Z
- waiting_for: none
- executor_tokens: input 26, cache write 78733, cache read 373456, output 8177
- executor_usage: runs 1, turns 12, context start 17k peak 54k, resumes 2, 11 min, units 0.18 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. wbp-shop-screen updated and new node wbp-shop-screen-indicator (08749242); no capture-tool node reachable, --force-res noted in wbp-shop-screen verified_anchors.

## Questions
Decided 2026-10-28 (owner "OK, tout par défaut"): option (a), values provisional.
- Q3 How icon and value are centred. Default (a): no Row; OreBalanceText anchored at the panel centre point (0.5,0.5) with alignment (0,0.5), so the text starts at the centre line and grows to the right; OreIcon anchored at the same point with alignment (1,0.5) and a small gap, so it ends left of the centre line; both on the same vertical centre; icon height set from the font size (same slate units, so they scale together); the icon does not move when the number changes. Option (b): a RichTextBlock with the icon inline in the text (it follows the font size exactly), OreIcon kept as an invisible flight anchor; more setup, harder to edit by hand. Option (c): the whole group centred in the panel (moves slightly when the digit count changes).

## Subtasks
1. task-357-01-shop-resource-indicator (vault/reference/tasks/task-357-shop-resource-indicator/task-357-01-shop-resource-indicator.md) — bp: IndicatorRessource rebuilt without a Row, icon and value centred on the font line.
2. task-357-02-shop-indicator-capture (vault/reference/tasks/task-357-shop-resource-indicator/task-357-02-shop-indicator-capture.md) — cpp: -game captures at four resolutions and three balance sizes.
3. task-357-03-shop-indicator-doc (vault/reference/tasks/task-357-shop-resource-indicator/task-357-03-shop-indicator-doc.md) — doc: update wbp-shop-screen.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-357-01-shop-resource-indicator]] — step 1, bp.
- [[task-357-02-shop-indicator-capture]] — step 2, cpp.
- [[task-357-03-shop-indicator-doc]] — step 3, doc.
- [[task-356-shop-screen-anchors]] — runs first; holds the owner's message and Q4 to Q7.
- [[umg-layout]] — the UMG layout rule this task applies.
