---
id: task-402-capture-map-path-msys
description: "Global task: Fix the -game launch helpers so a /Game/... map path given from Git Bash reaches Unreal intact; capture.js, Run-StressTest.ps1, agent docs."
parent: tasks-done-index
kind: task
status: done
---
# Game launches: /Game map path rewritten by Git Bash

- request: Owner 2026-10-30, after task-399: "Oui, ouvre-la" (open a task to fix the capture agent map path bug). Context relayed by the main session: during task-399-02 the capture agent -game launches failed 4 times out of 5: Git Bash (MSYS) rewrote the map argument `/Game/RogueRigs/Maps/L_ArenaM1` into `C:/Program Files/Git/Game/RogueRigs/Maps/L_ArenaM1`, Unreal said "The map specified on the commandline ... could not be found" and the map did not load; the task-399-02 cost alert came from this. Goal: the capture agent (and every script or doc it relies on, its definition in .claude/agents/ and its vault/reference/agents/ node, Tools/ or vault/scripts/ launch helpers) launches the game with a correct /Game/... path; find where the launch command is defined and fix it at the source; check other agents that launch the game (cpp stress runs). Owner answer 2026-10-30 (relayed by the main session, verbatim): "Ok, par défaut" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: A -game launch started by an agent from Git Bash always opens the intended /Game/... map: the launch helpers repair a path MSYS rewrote, and the agent docs say how to pass a /Game path from Bash.
- acceptance: A1 `node Tools/capture/capture.js --map /Game/RogueRigs/Maps/L_ArenaM1 ...` run from Git Bash produces a run.log whose `LogInit: Command Line:` line starts with `/Game/RogueRigs/Maps/L_ArenaM1` and has no "could not be found" line. A2 Run-StressTest.ps1 applies the same repair to `-Map`. A3 tool-capture, sys-stress, agent-capture, orchestrator-script-quirks and .claude/agents/capture.md state the rule; agent-capture `visual_check` no longer suggests a raw UnrealEditor.exe launch from Bash.
- gdd_check: Tooling only, no gdd node touched; no conflict, no `open` value needed, inside M2. Findings (planner 2026-10-30): (1) the rewrite happens in the shell before node starts: capture.js default map (`/Game/RogueRigs/Maps/L_ArenaM1`, line 151) is passed by `child_process.spawn` and is never rewritten; only an explicit `--map /Game/...` typed in Git Bash is. Evidence: Saved/Captures/T399-1280x720, T399-1920x1080, T399-1920x1200, T399-2560x1080 run.log `LogInit: Command Line: "C:/Program Files/Git/Game/RogueRigs/Maps/L_ArenaM1"`; T399-all (no --map) has `/Game/RogueRigs/Maps/L_ArenaM1`. (2) Same defect outside capture.js: Saved/Logs/task388_explore-backup-2026.10.30-20.53.39.log, a raw UnrealEditor.exe launch from Bash, loaded `C:/Program Files/Git/Game/RogueRigs/Maps/L_MainMenu`. (3) Other launchers: Scripts/Run-StressTest.ps1 has `-Map` (default `/Game/RogueRigs/Maps/L_ArenaM1`), rewritten when given from Bash through `powershell -File`; Tools/capture/robot_shots.ps1 hard-codes `$Map` inside PowerShell, not affected. (4) agent-capture `visual_check` still documents a raw `UnrealEditor.exe <uproject> <map> -game` launch; orchestrator-script-quirks Shells has no MSYS entry. (5) `--exec` strings with `/Game/...` class paths after a space are not rewritten by MSYS (the argument does not start with `/`).
- confirmed_by_owner: yes (2026-10-30, "Ok, par défaut")
- milestone: M2
- next_step: none
- started: 2026-10-30T23:50:36Z
- waiting_for: none
- executor_tokens: input 24, cache write 40865, cache read 291848, output 3863
- executor_usage: runs 1, turns 10, context start 18k peak 41k, resumes 0, 5 min, units 0.10 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Done, commit 7503e227: tool-capture, sys-stress, agent-capture, orchestrator-script-quirks, .claude/agents/capture.md state the MSYS repair and MSYS_NO_PATHCONV=1 rule; lint 0 problems.

## Questions
Decided 2026-10-30 (owner "Ok, par défaut"): every default below is accepted; values provisional, owner 2026-10-30.
- Q1 Fix method. Default: the launch helpers repair the path themselves: capture.js (`--map`, and any `<drive>:/.../Git/[usr/]Game/...` prefix in `--exec`) and Run-StressTest.ps1 (`-Map`) turn a rewritten path back into `/Game/...` and print one line `map: repaired MSYS path -> /Game/...`; the docs add the rule "from Git Bash, prefix a raw UnrealEditor.exe launch with `MSYS_NO_PATHCONV=1`". Other choices: docs only (MSYS_NO_PATHCONV=1, no script change), or agents launch from PowerShell only.
- Q2 Check of the fix. Default: one short -game run of capture.js from Git Bash with `--map /Game/RogueRigs/Maps/L_ArenaM1` (a few seconds, one shot, then Quit), reading the `LogInit: Command Line:` line of run.log; Run-StressTest.ps1 checked without a launch (repair function called on the rewritten string). Other choice: no game launch at all.
- Q3 Who edits the agent files. Default: the doc subtask updates tool-capture, sys-stress, agent-capture (`visual_check`, `capture_script`), orchestrator-script-quirks (Shells) and one line in .claude/agents/capture.md. Other choice: leave agent-capture and .claude/agents/capture.md unchanged.

## Subtasks
1. task-402-01-map-path-repair (vault/reference/tasks/task-402-capture-map-path-msys/task-402-01-map-path-repair.md) — cpp: capture.js and Run-StressTest.ps1 repair a /Game map path rewritten by Git Bash (MSYS), one launch check
2. task-402-02-doc-map-path-msys (vault/reference/tasks/task-402-capture-map-path-msys/task-402-02-doc-map-path-msys.md) — doc: tool-capture, sys-stress, agent-capture, orchestrator-script-quirks and .claude/agents/capture.md state the repair and the MSYS_NO_PATHCONV=1 rule

## Relations
- [[tasks-done-index]] — parent index.
- [[task-402-01-map-path-repair]] — step 1, cpp.
- [[task-402-02-doc-map-path-msys]] — step 2, doc.
- [[task-399-total-duskite-number-overlap]] — the task whose capture lost 4 of 5 launches to the rewrite.
- [[tool-capture]] — capture.js documentation updated by step 2.
- [[agent-capture]] — capture agent node updated by step 2.
