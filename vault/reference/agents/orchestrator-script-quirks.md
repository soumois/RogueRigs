---
id: orchestrator-script-quirks
description: "Known defects of the vault task scripts and how to work around them: task-next, record-cost, attribution, amend, editor close. Open before closing a subtask."
parent: agents-index
kind: note
---
# Orchestrator script quirks

Commands themselves: [[orchestrator-runbook]]. Observed 2026-10-11.

## task-next.js
- It hard-codes `Co-Authored-By: Claude Opus 5.5` in its commit. Fix the line right after, by amending ONLY your own just-made commit (check `git log -1` is the script's commit; an amend once rewrote an agent's commit message).
- The commit fails when the next subtask node is new and untracked ("pathspec did not match"). The files are still written: lint, `git add` the new nodes and commit by hand.
- It closes the global task when the subtask it closes is the last one it finds done: create every planned subtask node BEFORE closing, and check the global is still `in-progress` afterwards (task-101 was closed by mistake once and reopened with `task-status.js`).
- Run it ONCE: a second run with another result text overwrites the `result` field.
- `record-cost.js` needs usage under the subtask id: it fails ("no usage") for a subtask run by a resumed agent that kept its first description. Close those by hand with `task-status.js status=done "result=..." runs=1` and note the cost is counted elsewhere.
- Set `in-progress` on a new subtask and on the global with `task-status.js`; moving a global back from done works through `status=in-progress`.

## Git
- Every command needs `-c safe.directory=C:/Users/soumo/Documents/Unreal Projects/RogueRigs`.
- A pathspec commit of a new, unstaged file fails: `git add` it by name first.
- The pre-commit hook runs the vault lint and blocks on another session's broken nodes: wait for that session or ask, never skip the hook.

## Editor
- `Start-Process` the editor, then poll `http://127.0.0.1:8000/mcp` (HTTP 405 on GET means up).
- To close it: `CloseMainWindow()`; a "Save Content" dialog may keep it alive: when `git status` shows no modified tracked file, `Stop-Process -Force` loses nothing.

## Tools of this session
- `SendUserFile` shows files to the owner; `Read` shows images to the model only.
- `TaskCreate` and `TaskUpdate` were not available: the vault nodes were the only tracking.

## Relations
- [[agents-index]] — parent index.
- [[orchestrator-runbook]] — the commands these quirks qualify.
- [[art-3d-lessons]] — the owner and agent lessons of the same session.
- [[cost-log]] — where record-cost writes.
