---
id: orchestrator-script-quirks
description: "Pitfalls of task scripts, git, editor, shells and the permission classifier, with workarounds; open before closing a subtask or committing."
parent: agents-index
kind: note
---
# Orchestrator script, git and editor quirks

Commands themselves: [[orchestrator-runbook]]. Observed 2026-10-11 to 2026-10-14.

## Task scripts
- `task-next.js` closes the global task when the subtask it closes is the last done one it finds: create every planned subtask node BEFORE closing, `git add` new ones first (untracked nodes fail its commit with "pathspec did not match"), and check the global is still `in-progress` afterwards. For a global with several subtasks, `task-status.js` per subtask is safer.
- Run it ONCE: a second run overwrites the `result` field.
- It hard-codes its Co-Authored-By line: if it differs from the session's, amend ONLY your own just-made commit (check `git log -1`).
- `record-cost.js` matches usage by the subtask id at the start of the agent's description. A resumed agent keeps its first description: close that subtask with `task-status.js status=done "result=..." runs=1` and note the cost is counted elsewhere.
- `task-create.js` refuses to overwrite an existing node and writes nothing: pick a free number or remove the stale draft first. It does not lint or commit.
- `task-confirm.js` hard-codes its Co-Authored-By line (Opus 5.5), like `task-next.js`.
- Two sessions create task numbers in parallel: `ls vault/reference/tasks` right before creating.

## Git
- The pre-commit hook (`core.hooksPath = vault/scripts/hooks`) runs the vault lint and blocks on another session's broken nodes: wait or ask, never skip it.
- Commit by explicit path (`git commit -m "..." -- <paths>`), never `git add vault`: agents and other sessions stage files. A new file is `git add`ed by name first; adding an ignored path (`Reference/`) fails the whole command.

## Editor
- Start with `Start-Process`, then poll `http://127.0.0.1:8000/mcp` (HTTP 405 on GET means up).
- Close only an editor this session launched, with `CloseMainWindow()`. A "Save Content" dialog may keep it alive: `Stop-Process -Force` only when `git status` shows no modified tracked file, and never close it while that dialog is open without asking.
- `Build.bat` refuses to build while Live Coding runs in the owner's editor: ask him to close it, never kill it.
- Headless tests and builds need the editor closed; a build "up to date" after asset-only work means the tests do not exercise the Blueprints.

## Shells
- The Windows Store `python3` alias hangs on stdin and leaves stuck shells: use node; kill stuck `bash.exe` and `python3.exe` trees.
- Git Bash rewrites an argument starting with `/` into `C:/Program Files/Git/...` (a `/Game/...` map path broke launches, task-402): prefix the command with `MSYS_NO_PATHCONV=1` or use the helpers (capture.js, Run-StressTest.ps1, [[tool-capture]]), which repair it.
- An agent may leave a shell stuck on an unterminated heredoc: find it by command line (`Win32_Process`) and stop it after checking its work was committed.

## Permission classifier
- It denied a subagent's `taskkill` of the editor and a `CaptureViewport`, and refuses actions approved only through a relayed message (new network host). Ask the owner to run the command or add a rule in `.claude/settings.local.json`; never route around a denial ([[kie-image-workflow]]).

## Tools of the desktop app
- `SendUserFile` shows files to the owner; `Read` shows images to the model only.
- The task panel is `update_plan`; `TaskCreate` and `TaskUpdate` are not available.

## Relations
- [[agents-index]] — parent index.
- [[orchestrator-runbook]] — the commands these quirks qualify.
- [[orchestrator-lessons]] — owner rules.
- [[cost-log]] — where record-cost writes.
- [[tool-capture]] — capture.js and Run-StressTest.ps1 repair the MSYS-rewritten /Game map path.
