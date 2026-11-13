---
id: task-spec
description: "How the Planner writes a plan: spec fields of task-create.js, resolving ids, wait-answers, defaults file and task-confirm. Read before writing a plan."
parent: agents-index
kind: note
---
# Writing a plan

Write the whole plan once: one spec module for the planning pass, one `node vault/scripts/task-create.js <spec.js>`, then `node vault/scripts/lint.js --write`, `git add` the printed `files:` and commit by path. Do not read the script. Put the spec in `Saved/Planner/`.

```js
module.exports = { edits: [{ file, old, new }], globals: [{
  slug, title, description, level, family, request, goal, acceptance, gdd_check,
  status: 'wait-answers', waiting_for: '...', questions: ['Q1 ... Default: ...'], // omit the three when nothing is asked
  subtasks: [{ slug, agent, line, inputs: ['node-id', 'path/file.ext'], done_when }],
}] };
```

- `family`: `comportement`, `visuel` or `gros`, the family the relay stated ([[request-families]]), written `- family:` on the global; `open` when omitted. `reglage` is written only by `task-reglage.js`.
- `level`: `light`, `standard` or `exploratory` ([[verification-levels]]), written as `verification_level` on the global; any other value or none is refused.
- `request`: the owner's words verbatim. `acceptance`: one string, never a list (A1 ...; A2 ...). `gdd_check`: nodes read, conflicts, `open` values and decisions, no copied prose. `edits`: the gdd edits the defaults imply, exact text.
- Subtask optional fields, only when they say something the Executor cannot decide: `goal` (default `line`), `expected_output`, `editor_state`, `token_ceiling`, `depends_on` (default the previous subtask), `milestone`.
- `inputs`: a bare id becomes `id (path)`; an unknown id is refused; literal paths are kept. Point at sources, never copy them.
- Numbers and index place: do not list `vault/reference/tasks`. Without `num`, the globals get the next free numbers in their order; without `anchor`, their index lines go in task-number order. Use the ids of the printed `created <global-id>` lines. Optional overrides: `num` on a global (refused if the number is taken), `anchor` in the spec or as a second argument (the task id whose index line they follow).
- The script refuses, before writing anything, an existing node, a taken num, an unknown id or a missing field.
- Id paths: `node vault/scripts/path-of.js <id> [<id>...]`, exact ids only; an unknown id exits 1.

## Status
- Questions left: set `wait-answers`. The script also writes `Saved/Planner/<first-global-id>.defaults.json`, not committed.
- Pure "ok" or "tout par défaut": the main session runs `node vault/scripts/task-confirm.js <file> "<owner words verbatim>"`, without resuming the Planner. It marks the questions decided, appends the words to `request`, sets `confirmed_by_owner` and `todo`, applies the edits, lints and commits. Any other answer, or a refusal, resumes the Planner. On a level word (leger, standard, exploratoire) the Planner edits `verification_level` on the global, resizes `acceptance` and the capture checklist to it, lints and commits; the script keeps the level at confirmation.
- Do not repeat those transitions with `task-status.js`.
- `task-status.js` syntax, e.g. back to `wait-answers`: `node vault/scripts/task-status.js <global-id>.md status=wait-answers "waiting_for=<question>"`. The path is relative to `vault/reference/tasks/` (a subtask: `<global-id>/<subtask-id>.md`; a `vault/...` path fails); one quoted `key=value` argument per field, an existing bullet field only; `waiting_for` without `status=wait-answers` is refused.

## Relations
- [[agents-index]] — parent index.
- [[agent-planner]] — writes the plan.
- [[orchestration-protocol]] — step 3 says what the plan holds.
- [[verification-levels]] — the values of `level`.
- [[request-families]] — the values of `family`.
