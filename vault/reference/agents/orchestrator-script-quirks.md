---
id: orchestrator-script-quirks
description: "Pitfalls of task scripts, git, editor, shells and the permission classifier, with workarounds; open before closing a subtask or committing."
parent: agents-index
kind: note
---
# Orchestrator script, git and editor quirks

Commands themselves: [[orchestrator-runbook]]. Observed 2026-10-11 to 2026-10-14.

## Task scripts
- `task-next.js` checks before its first write and, on a refusal, prints `Nothing changed.` (task-538): a global that keeps an open subtask while the next-global and next-subtask arguments are missing or name another global (it prints the command with the next open subtask), a double-bracket link to an unknown id in the result text (drop the brackets or use backticks), or a closing the `task-status.js` gate would refuse. Create every planned subtask node before closing; it `git add`s the untracked nodes it commits.
- A late failure (lint problem in another node, hook or commit refused) leaves its writes in place: fix the cause and rerun the same command. It skips what is already applied (one cost row, `started` unchanged) and commits; a rerun after a committed close prints `already closed; nothing to commit.`.
- It hard-codes its Co-Authored-By line: if it differs from the session's, amend ONLY your own just-made commit (check `git log -1`).
- `record-cost.js` matches usage by the subtask id at the start of the agent's description; a subtask with no such agent counts as included in the executor run of its global, and it refuses only if that global has no executor run either. The executor is tied to its global by the first task number of its prompt, and its description must not start with a subtask id. A resumed agent keeps its first description: close that subtask with `task-status.js status=done "result=..." runs=1` and note the cost is counted elsewhere.
- `task-create.js` refuses to overwrite an existing node and writes nothing: omit `num` to get the next free number, or remove the stale draft first. It does not lint or commit.
- `task-confirm.js` hard-codes its Co-Authored-By line (Opus 5.5), like `task-next.js`.
- Two sessions create tasks in parallel: omit `num`; `task-create.js` reads `vault/reference/tasks` when it runs, so a number the other session took meanwhile is skipped (task-533).

## Git
- The pre-commit hook (`core.hooksPath = vault/scripts/hooks`) runs the vault lint and blocks on another session's broken nodes: wait or ask, never skip it.
- `lint.js` hides by default the warnings of files equal to HEAD, so a warning of a node you already committed no longer shows: run `lint.js --all` to see every warning ([[orchestrator-runbook]]).
- Commit by explicit path (`git commit -m "..." -- <paths>`), never `git add vault`: agents and other sessions stage files. A new file is `git add`ed by name first; adding an ignored path (`Reference/`) fails the whole command.

## Editor
- Start with `Start-Process`, then poll `http://127.0.0.1:8000/mcp` (HTTP 405 on GET means up).
- Before closing, save every asset you touched with `save_assets` ([[agent-bp]] step order), so that the "Save Content" dialog that stopped tasks 424 and 314 has nothing to ask. Not yet checked on a real close (owner 2026-11-05).
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
