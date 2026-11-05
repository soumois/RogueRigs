---
id: task-spec
description: "How the Planner writes a plan: spec fields of task-create.js, resolving ids, wait-answers, defaults file and task-confirm. Read before writing a plan."
parent: agents-index
kind: note
---
# Writing a plan

Write the whole plan once: one spec module for the planning pass, one `node vault/scripts/task-create.js <spec.js> [<index-anchor-task-id>]`, then `node vault/scripts/lint.js --write`, `git add` the printed `files:` and commit by path. Do not read the script. Put the spec in `Saved/Planner/`.

```js
module.exports = { anchor: 'task-NNN-slug', edits: [{ file, old, new }], globals: [{
  num, slug, title, description, request, goal, acceptance, gdd_check,
  status: 'wait-answers', waiting_for: '...', questions: ['Q1 ... Default: ...'], // omit the three when nothing is asked
  subtasks: [{ slug, agent, line, inputs: ['node-id', 'path/file.ext'], done_when }],
}] };
```

- `request`: the owner's words verbatim. `gdd_check`: nodes read, conflicts, `open` values and decisions, no copied prose. `edits`: the gdd edits the defaults imply, exact text.
- Subtask optional fields, only when they say something the Executor cannot decide: `goal` (default `line`), `expected_output`, `editor_state`, `token_ceiling`, `depends_on` (default the previous subtask), `milestone`.
- `inputs`: a bare id becomes `id (path)`; an unknown id is refused; literal paths are kept. Point at sources, never copy them.
- The script refuses, before writing anything, an existing node, an unknown id or a missing field. Numbers: list `vault/reference/tasks` right before creating.
- Id paths: `node vault/scripts/path-of.js <id> [<id>...]`, exact ids only; an unknown id exits 1.

## Status
- Questions left: set `wait-answers`. The script also writes `Saved/Planner/<first-global-id>.defaults.json`, not committed.
- Pure "ok" or "tout par défaut": the main session runs `node vault/scripts/task-confirm.js <file> "<owner words verbatim>"`, without resuming the Planner. It marks the questions decided, appends the words to `request`, sets `confirmed_by_owner` and `todo`, applies the edits, lints and commits. Any other answer, or a refusal, resumes the Planner.
- Do not repeat those transitions with `task-status.js`.

## Relations
- [[agents-index]] — parent index.
- [[agent-planner]] — writes the plan.
- [[orchestration-protocol]] — step 3 says what the plan holds.
