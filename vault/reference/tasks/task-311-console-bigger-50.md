---
id: task-311-console-bigger-50
description: "Global task: The objective console is 50 % bigger and lands only on a free 3 x 3 floor footprint; chest unchanged."
parent: tasks-done-index
kind: task
status: done
---
# Console 50 % bigger

- request: "Aggrandir de 50% la console." (owner, 2026-10-24)
- goal: The objective console is 50 % bigger and lands only on a fully free 3 x 3 floor footprint; the bonus chest is unchanged.
- acceptance: BP_ObjectiveConsole Mesh scale about 1.361 (was 0.90754), AntennaHead keeps its relative offset under Mesh; ARRConsole FootprintCells 3 x 3, the console lands only where all 9 cells are reachable floor and stands at the footprint centre; ARRBonusChest footprint stays 2 x 2; hack zone radius unchanged (7 cells); GDD objective-console, tech doc and Blueprint doc updated; only tests this change breaks are updated; -game capture before/after for the owner; committed.
- gdd_check: objective-console read (2026-10-24): landing clearance 2 x 2 cells (provisional, owner after handover-2026-10-06) -> 3 x 3 (owner 2026-10-24, decision 11, provisional); zone radius 7 cells unchanged; no conflict, no open value. The bonus chest inherits ARRConsole: its footprint stays 2 x 2 (tasks 306-310 belong to another session, not touched).
- confirmed_by_owner: yes (2026-10-24, "ok" to the plan and every default; planned in one planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. Saved/ConsoleBigger/contact_sheet_console_before_after.png (6 before/after pairs, same cameras; frames cb_* in Saved/ConsoleBigger/, log run1.log): console about 1.5 x bigger (about 200-220 uu, pixel estimate), on open floor, head on the mast and turning in Idle, lamps and screen in place, zone circle fits with margin; no defect from the change; still washed out inside the light hack disc (task-312); no repo change

## Subtasks
1. task-311-01-console-footprint-3x3 — cpp: Console footprint 3 x 3
2. task-311-02-bp-console-scale — bp: BP_ObjectiveConsole scale 1.361
3. task-311-03-capture-console-bigger — cpp: Capture the bigger console

## Relations
- [[tasks-done-index]] — parent index.
- [[task-311-01-console-footprint-3x3]] — step 01, cpp.
- [[task-311-02-bp-console-scale]] — step 02, bp.
- [[task-311-03-capture-console-bigger]] — step 03, cpp.
