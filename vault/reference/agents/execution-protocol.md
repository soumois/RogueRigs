---
id: execution-protocol
description: "Executor steps 5-6: start, do, check and close each subtask, preview gate, owner wait, recorded budget, global close. Open to change execution."
parent: agents-index
kind: rule
applies_to: orchestrator
---
# Execution protocol

Follows the planning steps of [[orchestration-protocol]]. Since owner 2026-11-05 the [[agent-executor]] is one Opus agent per global task that does every subtask itself in one context; a subtask's `agent` names the role whose rules apply, and only [[agent-style-guide]] is launched. It owns all execution and resumes and does not recall the Planner. The nodes say what to reach; it chooses how, and reads the code it needs.

Executor step 0: check `git status` and the task's `next_step`, then run `node vault/scripts/task-start.js <global-id>` (a no-op when already started) and open the subtask path on its last line `next: <path>` directly (execution exception of [[_navigation]], owner 2026-10-27: no descent, no search). It does not read `tasks-index` or report other tasks. A global with `family: reglage` was written by `task-reglage.js` without a planner; its wrong-family stop is in [[request-families]].

## 5. Execute, one subtask at a time
1. Start the global with `task-start.js`: it sets the global and its first open subtask `in-progress` and `next_step` to it, lints and commits. Later subtasks are started by `task-next.js` when the previous one closes. Both print only lines that need action and end with `next: <absolute path>` or `next: none`. Refusals and failures print no `next:` line.
2. Prepare the editor state (close Unreal before C++ builds; launch it before BP work).
3. Do the subtask itself, from its node and resolved input paths, following the planned order. After a text or image generation, launch `style-guide` with a description starting with the subtask id. The doc subtask is written the same way, from its own commits.
4. Run the `done_when` checks, at the verification size of the level of the global ([[verification-levels]]). A placement, layout or visual choice also needs a look at a frame of the game.
5. Pass: close with `task-next.js <global> <subtask> <runs> "<result>"`, which records the cost, sets `done`, moves `next_step`, lints and commits. The tokens of the task stay on the Executor's run ([[agent-executor]] `cost`); write the cause of any ALERT line in `result`. A refused call changes nothing and says so (for example a missing next subtask: it prints the full command); a late failure (lint, hook, commit) is finished by rerunning the same command once its cause is fixed ([[orchestrator-runbook]]).
   Closure gate (owner 2026-11-01): `task-status.js` refuses to mark a global `done` unless every listed subtask is `done`, its `model`, `tokens`, `usage` and `runs` fields are recorded, and its row exists in [[cost-log]].
6. Fail: one focused correction and a new check. Second failure: set `blocked`, record why, stop and report. At the exploratory level a look not yet found is not a failure: it uses the capture rounds of [[verification-levels]], and at the limit goes to the owner wait below.
7. Preview gate (visuel family, owner 2026-11-11): once the preview subtask has its mockup or capture, it always sets that subtask and the global `wait-answers` with `waiting_for=owner go on <image path>`, even when its own checklist passes, and reports the image path. On the go it closes the preview with `task-next.js` and builds; on a refusal the build subtasks stay `todo`, so nothing is reverted, and the owner's answer decides what follows.
8. Owner wait: when it stops for an owner action or answer (close the editor, a missing decision), it sets the current subtask and its global `wait-answers` with `node vault/scripts/task-status.js <path>.md status=wait-answers "waiting_for=<the action or question>"`, commits them by path, and reports. On resume it sets both back with `status=in-progress` (or `task-start.js`; `task-next.js` does it for the next subtask), which resets `waiting_for` to `none`.
9. Budget (owner 2026-11-12, [[task-508-workflow-back-to-old]]): the budget of the level is only recorded. A closing over it prints an ALERT line; the Executor writes the overrun and its cause in `result`, lists it in the final report and goes on: no handover, no owner wait. One Executor runs the whole global.

## 6. Close
Run the global acceptance checks, verify every subtask cost row and usage field is present, fill `result`, set `done`, `next_step: none`, commit, report to the owner. `task-status.js` enforces the cost gate when the global is closed.

## Relations
- [[agents-index]] — parent index.
- [[orchestration-protocol]] — steps 0 to 4, what it runs.
- [[agent-executor]] — runs these steps.
- [[orchestrator-runbook]] — the commands that apply them.
- [[cost-log]] — where step 5 records tokens.
- [[_navigation]] — execution exception for opening task nodes by path.
- [[verification-levels]] — verification size and budget per global.
- [[task-508-workflow-back-to-old]] — owner decision behind step 5.9: no handover, budget only recorded (replaces task-502 and task-507).
- [[request-families]] — réglage globals and their wrong-family stop.
