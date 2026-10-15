---
id: orchestrator-runbook
description: "Orchestrator how-to: git, editor start and stop, delegation prompt, closing a subtask with the vault scripts, dashboard. Open when taking over."
parent: agents-index
kind: note
---
# Orchestrator runbook

Complements [[orchestration-protocol]] (the rules) with the commands that worked from task-013 to task-029.

## Git
- The repo path is in the global `safe.directory`: plain `git` works. The pre-commit hook is installed (`core.hooksPath = vault/scripts/hooks`) and runs the vault lint; never skip it.
- Commit only the files you edited, by name (`git commit -m "..." -- <file> <file>`): the agent also edits vault docs, and `-- vault` sweeps its unfinished work into your commit.

## Editor
- Exe: `C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe "<repo>\RogueRigs.uproject"`.
- After launch, wait until `http://127.0.0.1:8000/mcp` answers (curl loop in the background), then delegate to `bp`.
- Close it and handle a blocked close as in [[orchestrator-script-quirks]].
- C++ subtasks: editor closed. Blueprint subtasks: editor open.

## Delegation
- One background `Agent` call per subtask; the description must start with the subtask id (cost tracking).
- Prompt: the node path, the reading protocol, then notes: editor state, "do not modify task node status, model, tokens, runs fields", commit message `task-NNN-SS-slug: <summary>` with the session's Co-Authored-By line, and what to report.
- Resolved paths (owner 2026-10-06): the Orchestrator resolves once, through _paths.md, the path of every node a subtask needs (its inputs and the docs to update) and lists those paths in the prompt; the agent reads them directly instead of descending from root again. Re-reading these files in each new subtask is the accepted cost of independent subtasks with a small context; never merge subtasks to save that reading.
- On "turn limit": `SendMessage` to the same agent: continue, commit what works first.
- The previous run's left-over files: tell the next agent to inspect `git diff` and finish them.

## Creating tasks
- One logical owner point = one global task ([[orchestration-protocol]]). Write a spec module and run `node vault/scripts/task-create.js <spec.js> <last-task-id>`: it writes each global task with one subtask, resolves the input ids to paths (the delegation hands those paths to the agent) and adds them to tasks-to-do-index (at the end of the list when the anchor is not there). `task-status.js` moves a global task to tasks-done-index when it is set to done. A second subtask for another agent is written by hand from `vault/templates/task.md`.

## Closing a subtask
1. Check the report against `done_when`.
2. `node vault/scripts/task-next.js <global> <subtask> <runs> "<result>" [<next-global> <next-subtask>]`: records the cost, closes the subtask (and the global when it was the last one), starts the next one, lints and commits only those vault files. Pass the same global as next to continue its next subtask.
3. `node vault/scripts/crew-board.js "<note in French>"` writes `Saved/CrewBoard/crew-board.html`; publish it as an artifact for the owner when asked.

## Pitfalls
Script, git, editor, shell and permission pitfalls: [[orchestrator-script-quirks]]. Owner rules: [[orchestrator-lessons]].

## Relations
- [[agents-index]] — parent index.
- [[orchestration-protocol]] — the rules this runbook applies.
- [[orchestrator-lessons]] — owner rules and planning lessons.
- [[cost-log]] — filled by record-cost.js.
- [[orchestrator-script-quirks]] — defects of the scripts used here and their workarounds.
