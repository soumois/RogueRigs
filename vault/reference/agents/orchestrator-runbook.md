---
id: orchestrator-runbook
description: "Orchestrator how-to: git, editor start and stop, delegation prompt, closing a subtask with the vault scripts, dashboard. Open when taking over."
parent: agents-index
kind: note
---
# Orchestrator runbook

Complements [[orchestration-protocol]] (the rules) with the commands that worked from task-013 to task-029.

## Git
- The repo is flagged "dubious ownership": prefix every git command with `git -c "safe.directory=C:/Users/soumo/Documents/Unreal Projects/RogueRigs"` (never change the global config). The pre-commit hook is not installed (`git config core.hooksPath vault/scripts/hooks` would fix it; owner's call).
- While an agent works in the background, commit only the vault: `git commit -m "..." -- vault`.

## Editor
- Exe: `C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe "<repo>\RogueRigs.uproject"`.
- After launch, wait until `http://127.0.0.1:8000/mcp` answers (curl loop in the background), then delegate to `bp`.
- Close with `CloseMainWindow()`; if a "Save Content" dialog blocks, force-close only when `git status` is clean.
- C++ subtasks: editor closed. Blueprint subtasks: editor open.

## Delegation
- One background `Agent` call per subtask; the description must start with the subtask id (cost tracking).
- Prompt: the node path, the reading protocol, then notes: editor state, the git prefix, "do not modify task node status, model, tokens, runs fields", commit message `task-NNN-SS-slug: <summary>` with the session's Co-Authored-By line, and what to report.
- On "turn limit": `SendMessage` to the same agent: continue, commit what works first.
- The previous run's left-over files: tell the next agent to inspect `git diff` and finish them.

## Closing a subtask
1. Check the report against `done_when`.
2. `node vault/scripts/record-cost.js task-NNN-SS <runs>`.
3. `node vault/scripts/task-status.js <global>/<subtask>.md status=done "result=..."`, set the global `next_step`, mark the next subtask `status=in-progress`.
4. `node vault/scripts/lint.js --write`, commit.
5. `node vault/scripts/crew-board.js "<note in French>"` writes `Saved/CrewBoard/crew-board.html`; publish it as an artifact for the owner.

## Lessons
See [[orchestrator-lessons]].

## Relations
- [[agents-index]] — parent index.
- [[orchestration-protocol]] — the rules this runbook applies.
- [[orchestrator-lessons]] — agent limits and owner preferences.
- [[cost-log]] — filled by record-cost.js.
