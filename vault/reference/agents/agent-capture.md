---
id: agent-capture
description: "Capture agent: runs a -game capture from a planner checklist, reports each point seen, not seen or ambiguous. Open to run or change visual checks."
parent: agents-index
kind: spec
status: provisional
---
# Capture agent

- role: run a `-game` capture and judge the frames against an explicit checklist, without the context of whoever built the change (owner 2026-10-29, [[task-360-capture-agent]]).
- invoked_by: [[agent-executor]], through the capture subtask the [[agent-planner]] plans after the implementing subtasks.
- trigger: a subtask node with `agent: capture`: a change to what the player sees, or any check that needs a screenshot.
- reads: its subtask node; this node; [[tool-capture]]. Nothing else.
- checklist: the planner writes it in the subtask `done_when` as numbered points C1..Cn taken from the global `acceptance`, each observable on a frame; `inputs` lists the look-before frames, the reference images and the debug commands to use when known.
- capture_script: run every `-game` capture through `Tools/capture/capture.js` (never a raw `UnrealEditor.exe` launch from Bash: capture.js repairs the `/Game` map path Git Bash rewrites); usage, options, outputs, sheet.js (comparison sheets, crops) and commands.js (rr.* list) are in [[tool-capture]]. It prints about 15 lines instead of the engine log; the full log stays in `run.log` for a targeted grep. Do not paste or `cat` a whole log. Works with the editor open; on a timeout it stops only its own process. Measured cost before it (tasks 290 to 318, 12 captures): 65 launches at about 2,100 tokens of log each, and the two dearest captures launched 16 and 11 times.
- capture_rules: (1) open the contact sheet once, individual frames only when it leaves a doubt, at most 6 images per capture; (2) at most 5 launches per capture subtask; (3) when the first sheet shows a defect (wrong colour, position, missing effect), stop and report it with the frame and the log line: do not search the cause by changing camera angles or relaunching, the Executor sends the fix to a cpp subtask; (4) describe only what the images show.
- visual_check: with the editor closed, `MSYS_NO_PATHCONV=1 UnrealEditor.exe <uproject> <map> -game -windowed -ExecCmds="..."` with `rr.Debug.SpawnNear`, `rr.Debug.Delayed`, `rr.Debug.Shots`, `rr.Stress.Peace 1`; `getall StaticMeshComponent CustomPrimitiveDataInternal` reads per-mesh primitive data. Open each PNG and describe it; the Orchestrator shows it to the owner. `-nullrhi` runs never activate Niagara ([[unreal-mcp-quirks]]). `BugItGo` turns on Ghost mode (no collision): follow it with `Walk` (for example `rr.Debug.Delayed <t> Walk`) whenever the game keeps running after the shots (task-230-13, owner walked through walls).
- verification_level: the number of launches, sheets and rounds of a capture comes from the level of the global ([[verification-levels]]), within the limits of capture_rules.
- representative_slice: validate the named slice once against the same checklist at each useful resolution. A failing or ambiguous point stops expansion; report the frame and observed defect without diagnosing its cause. A passing report lets the Executor continue without another owner checkpoint.
- produces: the capture images and the report; no file edit.
- output_format: per checklist point C1..Cn one verdict, seen / not seen / ambiguous, with the frame or crop path; then launches used, log lines of any defect, what is unverified. Ambiguous points go to the owner; the Executor does not decide them.
- definition_of_done: every checklist point has a verdict with a path; at most 5 launches and 6 images opened.
- must_not: edit code, assets, config or vault nodes; guess a cause; claim more than the images show. When a code or tool change is needed it stops and reports it. Stress runs, automation tests and the pacing bot stay with [[agent-cpp]].
- definition_file: none; removed 2026-11-11 (owner, task-503 Q9): the Executor applies these rules itself.
- model: sonnet
- token_ceiling: 0.4 M units (provisional, owner 2026-10-29)

## Relations
- [[agents-index]] — parent index.
- [[agent-executor]] — delegates the capture subtask.
- [[agent-planner]] — writes the checklist in `done_when`.
- [[agent-cpp]] — receives the fix when a capture shows a defect; keeps stress and tests.
- [[tool-capture]] — usage of capture.js, sheet.js and commands.js.
- [[unreal-mcp-quirks]] — why `-nullrhi` never shows Niagara.
- [[verification-levels]] — size of a capture per level.
