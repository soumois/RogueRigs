---
id: orchestrator-runbook
description: "Orchestrator how-to: git, editor start and stop, delegation prompt, starting a task and closing a subtask with the vault scripts, dashboard. Open when taking over."
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
- Prompt: the node path (the subtask path as given by the `next:` line or the global `## Subtasks`, never rebuilt), the reading protocol, then notes: editor state, "do not modify task node status, model, tokens, runs fields", commit message `task-NNN-SS-slug: <summary>` with the session's Co-Authored-By line, and what to report.
- Resolved paths (owner 2026-10-06): the Orchestrator resolves once, through _paths.md, the path of every node a subtask needs (its inputs and the docs to update) and lists those paths in the prompt; the agent reads them directly instead of descending from root again. Re-reading these files in each new subtask is the accepted cost of independent subtasks with a small context; never merge subtasks to save that reading.
- Doc subtask ([[agent-doc]]): delegated to `doc`, description starting with its subtask id. Its brief, in the prompt after the notes: (1) commit hashes and changed paths of the implementing subtasks, from their change summaries (the agent runs `git show <hash> -- <paths>`; never paste the diff); (2) decided values: a pointer to the global `request` and `gdd_check`; (3) the cpp and bp change summaries (public API, properties set, values used); (4) the nodes to create or update, as resolved paths. Closed with `task-next.js` like any subtask.
- On "turn limit": `SendMessage` to the same agent: continue, commit what works first.
- The previous run's left-over files: tell the next agent to inspect `git diff` and finish them.

## Creating tasks
- One logical owner point = one global task ([[orchestration-protocol]]). Write one spec module for the whole planning pass and run `node vault/scripts/task-create.js <spec.js> [<index-anchor-task-id>]` once. Spec v2: `module.exports = {anchor?, edits?, globals: [...]}`. Global, required: `num, slug, title, description, request, goal, acceptance, gdd_check, subtasks`; optional: `confirmed_by_owner` (default `no (plan and questions handed back <today>)`), `milestone` (M2), `status` (`todo` or `wait-answers`, the latter needs `waiting_for`), `questions`, `questions_header`, `relations` (list of id and gloss pairs), `index_line`. Subtask, required: `slug, agent, line, goal, inputs, expected_output, done_when, editor_state, token_ceiling`; optional: `title, description, depends_on, milestone`. An input that is a bare id becomes `id (path)`, an unknown id is refused, literal paths and backtick text are kept. It refuses, before writing anything, when a target exists (it never overwrites), an id is unknown or a field is missing. The old array form still works (anchor required). It does not lint or commit: it prints `created <gid> (<n> subtasks)`, `defaults: <path>` and `files: ...` (to `git add` by name, then commit by path). When a global is `wait-answers` it also writes `Saved/Planner/<first-global-id>.defaults.json` from the Q ids and the spec `edits` (each `{file, old, new}`, `old` must exist in the file). Each `## Subtasks` line is `1. task-NNN-01-slug (vault/reference/tasks/task-NNN-slug/task-NNN-01-slug.md) — <agent>: <desc>`; a hand-written subtask line follows the same form, repo-relative), resolves the input ids to paths (the delegation hands those paths to the agent) and adds them to tasks-to-do-index (at the end of the list when the anchor is not there). `task-status.js` moves a global task to tasks-done-index when it is set to done.
- Id paths: `node vault/scripts/path-of.js <id> [<id>...]` prints `vault/<path>` per id, in argument order, for the exact id only (tasks and `_` meta nodes included; `[[id]]` and `[[id|alias]]` accepted). An unknown id prints `path-of: unknown id <id>` on stderr and exits 1 (known ids are still printed). Use it instead of reading `_paths.md`.
- Owner "ok": `node vault/scripts/task-confirm.js <defaults.json> "<owner words verbatim>"`, run by the main session on a pure "ok" / "tout par défaut": marks each global's `## Questions` decided with the date and words, appends the words to each `request`, sets `confirmed_by_owner: yes (<date>, "<words>")`, sets `todo` through `task-status.js`, applies the defaults file `edits`, lints and commits by path. Defaults file: `{globals:[{id, questions?}], edits:[{file, old, new}]}`. It refuses with nothing changed when the file is missing (resume the planner), a global is missing or already confirmed, or a Q or an edit is invalid. Any other answer resumes the planner.
- `started`: every node carries `- started:` (`open` at creation). Each `task-status.js status=in-progress` (so `task-start.js` and `task-next.js` too) writes the UTC time to the second (`2026-10-26T23:35:54Z`), replacing the previous value or inserting the bullet before `result` on older nodes.

- Cancelling: `node vault/scripts/task-status.js <global>.md status=cancelled [result=<why>]` moves the global to tasks-done-index (line ending `Status cancelled.`), sets each of its subtasks that is not done to `cancelled` and its `next_step` to `none`; lint and commit those files by path. Setting another status moves it back to tasks-to-do-index.
- Waiting for the owner: `node vault/scripts/task-status.js <path>.md status=wait-answers "waiting_for=<question or owner action>"` (refused without `waiting_for=`) writes `- waiting_for:` (inserted before `result` on older nodes); a global stays in tasks-to-do-index with `Status wait-answers.`. Any later status, including through `task-start.js` and `task-next.js`, resets it to `none`. Every node carries `- waiting_for: none` at creation.

## Starting a task
- `node vault/scripts/task-start.js <global-id>` (no `.md`): refuses unless `confirmed_by_owner: yes`; picks the global `next_step` subtask if not done, else the first `todo` one of `## Subtasks`; sets both `in-progress` and `next_step` through `task-status.js`, lints and commits only those vault files (`<subtask>: in-progress`). A second run changes nothing and exits 0 (it prints `<global>: already in-progress at <sub>; nothing to do.`). Output: only action lines (ALERT lines, lint problem lines and `N problem(s)`, hook and git errors, the `git log --oneline -1` line, refusals); dropped: lint `warn:` and `vault OK` lines, git LF/CRLF warnings, the record-cost summary line, Node stack traces. A failing child prints `<child> failed (exit N):` then its own text and the script exits with its code. After the commit, `left: <git status --porcelain line>` lists committed files still modified or untracked (`usage-snapshot.json` excluded). The last line is `next: <absolute path of the started subtask>` (also on the idempotent run) or `next: none`; open that path directly. Refusals and failures print no `next:` line and exit 1. Later subtasks start through `task-next.js`.

## Closing a subtask
1. Check the report against `done_when`.
2. `node vault/scripts/task-next.js <global> <subtask> <runs> "<result>" [<next-global> <next-subtask>]`: records the cost, closes the subtask (and the global when it was the last one), starts the next one, lints and commits only those vault files. Pass the same global as next to continue its next subtask. Same filtered output and `left:` lines as `task-start.js`; the last line is `next: <absolute path of next-global/next-subtask.md>` or `next: none`. It prints an ALERT line per budget overrun (units over `token_ceiling`, large start context, turn limit, resume): write the cause in the subtask `result` and tell the main session. `task-status.js` refuses `status=done` on a subtask whose `tokens` is still `open`.
3. `node vault/scripts/crew-board.js "<note in French>"` writes `Saved/CrewBoard/crew-board.html`; publish it as an artifact for the owner when asked.

## Pitfalls
Script, git, editor, shell and permission pitfalls: [[orchestrator-script-quirks]]. Owner rules: [[orchestrator-lessons]].

## Relations
- [[agents-index]] — parent index.
- [[orchestration-protocol]] — the rules this runbook applies.
- [[orchestrator-lessons]] — owner rules and planning lessons.
- [[cost-log]] — filled by record-cost.js.
- [[orchestrator-script-quirks]] — defects of the scripts used here and their workarounds.
- [[agent-doc]] — the agent the doc brief goes to.
