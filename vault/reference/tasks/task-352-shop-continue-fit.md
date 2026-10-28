---
id: task-352-shop-continue-fit
description: "Global task: Shop screen Continue plate fully visible above the bottom of the 1920 x 1080 screen, with Reroll above it."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Shop screen: Continue plate cut at the bottom

- request: owner point 2 of "Oui, planifier" (owner, 2026-10-28, relayed by the main session: yes to planning the fixes of the five layout defects seen in the task-348 capture, listed as open points in the task-348 `result` and in wbp-shop-card / wbp-shop-screen). Relayed points: 1 title clipped at the top (side bars at y = 0); 2 Continue plate cut at the screen bottom (y 995 to 1080); 3 "NEED 15 MORE" overflows the Buy plate, same large size as BUY, the mockup puts smaller text on a grey plate; 4 the crystal counter covers the top-left corner of the shop panel and the rivet of the first card; 5 minor: second frame line beside slot 0, tier tab text cramped. Unverified states to include in the capture: uncommon/epic/legendary tabs, hover/pressed tints, Reroll greyed when crystals are short, burn. Constraints: widget names and 342 to 344 animations kept; one agent at a time; button plates taller than their target boxes, to adjust in UMG; sold panel 450 x 600 to fit to the frame opening. The yellow flames of MI_CardBurn are a separate (material) subject, not planned. Owner answer 2026-10-28 (relayed by the main session): "OK, tout par défaut" = every default of Q1 to Q9 accepted; no task for Q8 (MI_CardBurn yellow flames) nor Q9 (square BuyFlash).
- goal: Shop screen Continue plate fully visible above the bottom of the 1920 x 1080 screen, with Reroll above it.
- acceptance: -game capture at 1920 x 1080: Continue and Reroll plates fully visible, Continue bottom at least the decided margin above y = 1080; title still fully visible; the 342 to 344 purchase effects still play; widget names kept; doc updated; lint clean; committed.
- gdd_check: read 2026-10-28 by the Planner: [[run-shop]] (screen look, provisional, owner 2026-10-28), tech [[wbp-shop-screen]], [[wbp-shop-card]], [[task-348-shop-screen-redesign]] (Q1 to Q11 stay decided) and its subtasks 07 to 09; capture frames srd_00, srd_23, crops_03_11, comparison_sheet and the mockup seen by the Planner: the five defects are confirmed. No conflict with the GDD; rules unchanged (display only). No `open` GDD value is needed; the sizes below are layout values the GDD does not hold, proposed as provisional defaults in `## Questions`. Scope: M2, same as task-348. Shared cause seen: the Content column (title, panel, Reroll, Continue) is taller than 1080 and centred, so it overflows at both ends; tasks run 351 to 355 in order, each keeping the previous fixes. Order: starts after [[task-351-shop-title-clip]] done (one agent at a time). Owner answer 2026-10-28: "OK, tout par défaut": every default of `## Questions` is DECIDED (values provisional, owner 2026-10-28); [[run-shop]] updated by the Planner the same day. Run order confirmed: 351, 352, 353, 354, 355.
- confirmed_by_owner: yes (2026-10-28, "OK, tout par défaut" to Q1-Q9 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-352-01-shop-continue-fit-fix
- started: 2026-10-28T17:41:33Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-28 (owner "OK, tout par défaut"), values provisional.
- Q2 (352, point 2) Reroll and Continue plates drawn inside their boxes (ImageSize = box size), gaps under the panel reduced; Continue bottom at least 16 px above y = 1080; cards keep 330 x 470.
- Q7 Each task ends with its own -game capture (rule 2026-10-16) and doc subtask; one agent at a time, 351 to 355 in order.
Questions Q1 to Q9 were asked together; each lives in the task it applies to (Q8, Q9 in task-351).

## Subtasks
1. task-352-01-shop-continue-fit-fix (vault/reference/tasks/task-352-shop-continue-fit/task-352-01-shop-continue-fit-fix.md) — bp: WBP_ShopScreen fix of point 2.
2. task-352-02-shop-continue-fit-capture (vault/reference/tasks/task-352-shop-continue-fit/task-352-02-shop-continue-fit-capture.md) — cpp: -game capture compared with the look before and the mockup.
3. task-352-03-shop-continue-fit-doc (vault/reference/tasks/task-352-shop-continue-fit/task-352-03-shop-continue-fit-doc.md) — doc: update wbp-shop-screen.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-352-01-shop-continue-fit-fix]] — step 1, bp.
- [[task-352-02-shop-continue-fit-capture]] — step 2, cpp.
- [[task-352-03-shop-continue-fit-doc]] — step 3, doc.
- [[task-348-shop-screen-redesign]] — the redesign whose capture showed this defect.
