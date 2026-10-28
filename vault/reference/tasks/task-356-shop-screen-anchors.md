---
id: task-356-shop-screen-anchors
description: "Global task: shop screen rebuilt on a Root Canvas with explicit anchors, from the owner's hand-edited version, to adapt to resolution and ratio."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Shop screen: Canvas and anchors instead of fixed pixel layout

- request: owner, 2026-10-28, verbatim: "Ok avant de continuer, j'aimerais revoir un peu ta manière de travailler, car je préfère retoucher à la main. Est-ce qu'il n'y a pas moyen d'utiliser plus souvent de simple canvas avec anchor ? Car là ça ne s'adapte pas vraiment à des petites variations de taille d'écran. Essaye déjà de changer ça, retrouve un résultat plus ou moins correct. Et si ta manière de faire est correcte, je saurai corriger par moi-même. J'ai modifié le WBP, mais tu peux repartir depuis. J'ai modifié ce qu'il y avait dans Indicator Ressource, mais même là je ne suis pas convaincu du container "row", car je voudrais que ça se centre par rapport à la taille de police qui s'adapte à l'écran." This task is the first point (Canvas and anchors for the screen); the resource indicator point is [[task-357-shop-resource-indicator]]. Relayed context (main session): the owner hand-edited `Content/RogueRigs/UI/WBP_ShopScreen.uasset`, modified and NOT committed; agents start from that version, never restore or overwrite it; tasks 348 to 353 used fixed pixel sizes (SizeBox, ImageSize, offsets set for 1920 x 1080) and vertical/horizontal boxes; the owner wants to correct by hand afterwards, so the layout must be simple and readable in the Designer (few nesting levels, clear names, explicit anchors) and the doc must state the anchor rule used. Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q7 accepted (Q3 = option a); values provisional, owner 2026-10-28. Run order confirmed: 356, then 357, then 355; 354 cancelled (absorbed).
- goal: WBP_ShopScreen: every top-level block is a direct child of the Root CanvasPanel with explicit anchors, alignment and offsets; sizes come from anchors (or from content and padding) instead of fixed pixel SizeBox / ImageSize, except images shown at native size; the look at 1920 x 1080 stays about the owner's hand-edited version; the cards follow their anchored area (with the WBP_ShopCard change of subtask 02).
- acceptance: the owner's hand-edited WBP_ShopScreen committed alone before any change (message says it is the owner's manual edits); -game captures at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 (Q4): title, subtitle, resource indicator, shop panel with 5 readable cards, Reroll and Continue fully visible, no overlap (indicator bottom above the panel top, run-shop "Screen fit"), nothing cut at the screen edges; the 342 to 344 purchase effects still play (BuyFlash, sparks, ore flight from OreIcon, burn, SOLD); widget names used by graphs kept; wbp-shop-screen and wbp-shop-card state which widget is anchored where, with which anchor values; lint clean; committed.
- gdd_check: read 2026-10-28 by the Planner: [[run-shop]] (screen look and "Screen fit at 1920 x 1080", provisional), tech [[wbp-shop-screen]], [[wbp-shop-card]], [[bp-mcp-lessons]], tasks [[task-354-shop-counter-overlap]] and [[task-355-shop-card-frame-details]] (paused). No conflict: display only, rules unchanged; the 1920 x 1080 fit values of run-shop stay the reference look and must also hold at the other resolutions. No `open` GDD value needed. Scope M2. Font scaling read: `Config/*.ini` has no `UserInterfaceSettings` override, so the engine default DPI scale rule applies (read back in Project Settings > User Interface by the bp agent, not changed). Binary scan of the modified asset (names only, no editor): it holds widgets CounterOverlay, CounterRow, CounterBg and a new "IndicatorRessource" ("Indicator Ressource" in the Designer); WBP_ShopCard and every other WBP are unmodified in git (only WBP_ShopScreen is ` M`). Effect on paused tasks: see Q6. Owner "OK, tout par défaut": every `## Questions` default is DECIDED; [[run-shop]] updated by the Planner the same day (screen fit by anchors at the four resolutions); rule node [[umg-layout]] written (Q5) and added to agent-bp `reads`; [[task-354-shop-counter-overlap]] cancelled, [[task-355-shop-card-frame-details]] rewritten to run after [[task-357-shop-resource-indicator]] under umg-layout.
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q7 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-356-01-shop-screen-anchors
- started: 2026-10-28T21:22:09Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-28 (owner "OK, tout par défaut"), values provisional.
- Q1 Anchor scheme of WBP_ShopScreen. Default: Root CanvasPanel with direct children only: Backdrop anchors (0,0)-(1,1) offsets 0; title group top centre anchor (0.5,0) alignment (0.5,0); IndicatorRessource top left anchor (0,0); ShopPanel stretched between anchor fractions of the screen (min and max anchors, so it follows the ratio); Reroll and Continue bottom centre anchor (0.5,1) alignment (0.5,1); the Content VerticalBox removed; at most three nesting levels per block; the owner's 1920 x 1080 placements kept as the reference and converted to anchors; offsets small and in slate units (DPI-scaled); 9-slice plates sized by their slot, not by a forced ImageSize.
- Q2 WBP_ShopCard. Default: included as subtask 02, minimal: the card scales to the anchored slot the screen gives it (the fixed CardHost 330 x 470 no longer forces the size; the card keeps its ratio through its ScaleBox); card internals (500 x 720 face, tab, plate, sold panel) untouched, they stay in task-355.
- Q4 Resolutions of the captures: 1920 x 1080 plus 1280 x 720, 1920 x 1200 and 2560 x 1080 (windowed -game).
- Q5 Method rule for the bp agents: a new rule node `umg-layout` in `tech/conventions/` ("UMG: Root Canvas and anchors first; no fixed pixel sizes except images at native size; no Row/Column to centre on a font; few nesting levels, clear names; doc lists anchors"), added to conventions-index and to agent-bp `reads`, written by the Planner once confirmed, before 356 runs.
- Q6 Paused tasks. Default: [[task-354-shop-counter-overlap]] cancelled, absorbed (its check "indicator bottom at least 8 px above the panel" is in this acceptance); [[task-355-shop-card-frame-details]] stays valid, still paused, runs after 357 with the rule of Q5 (sold panel fitted by anchors instead of 450 x 600).
- Q7 Captures: one per task (owner rule 2026-10-16), so 356 and 357 each end with their own.
Q3 is in [[task-357-shop-resource-indicator]].

## Subtasks
1. task-356-01-shop-screen-anchors (vault/reference/tasks/task-356-shop-screen-anchors/task-356-01-shop-screen-anchors.md) — bp: commit the owner's WBP_ShopScreen as-is, then rebuild it on Canvas and anchors.
2. task-356-02-shop-card-fit (vault/reference/tasks/task-356-shop-screen-anchors/task-356-02-shop-card-fit.md) — bp: WBP_ShopCard scales to its anchored slot (no fixed 330 x 470).
3. task-356-03-shop-anchors-capture (vault/reference/tasks/task-356-shop-screen-anchors/task-356-03-shop-anchors-capture.md) — cpp: -game captures at four resolutions, compared with the owner's version.
4. task-356-04-shop-anchors-doc (vault/reference/tasks/task-356-shop-screen-anchors/task-356-04-shop-anchors-doc.md) — doc: wbp-shop-screen and wbp-shop-card with the anchor rule used.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-356-01-shop-screen-anchors]] — step 1, bp.
- [[task-356-02-shop-card-fit]] — step 2, bp.
- [[task-356-03-shop-anchors-capture]] — step 3, cpp.
- [[task-356-04-shop-anchors-doc]] — step 4, doc.
- [[task-357-shop-resource-indicator]] — the second owner point of the same message, runs after this one.
- [[task-354-shop-counter-overlap]] — cancelled, absorbed by this task (Q6).
- [[umg-layout]] — the UMG layout rule this task applies (Q5).
- [[task-355-shop-card-frame-details]] — kept, runs after task-357 (Q6).
