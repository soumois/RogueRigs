---
id: task-358-planner-cost
description: "Global task: Cut planner cost: a script applies an all-defaults answer, a resolver prints one id path, task-create.js writes a whole plan from one spec."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Planner cost: defaults script, id resolver, one-call task creation

- request: owner, 2026-10-29, in French, verbatim: "Fait de 1 à 3 et est-ce que tu penses qu'un modéle sonnet suffirait pour les captures ? Peut être aussi prévoir un agent pour ça ? Par exemple, on lui donne une liste de chose à vérifier, mais il n'a pas besoin d'avoir tout le contexte. Je suppose qu'actuellement, l'agent BP ou CPP qui vérifie les captures et celui qui a créé ce qu'il doit vérifier". "1 à 3" = the three levers proposed by the main session the same day; this task is lever 1 (Planner, Opus, ~20 % of the total; target about -30 % of planner cost). Context relayed by the main session (cost analysis of 2026-10-29, `node vault/scripts/agent-usage.js --from 280` and subagent transcripts, weighted M units): tasks 332-357 cost ~49 M; workers 27.6 (bp 13.3, cpp capture subtasks 4.24 over 18 runs, 3d 3.6, art 2.4, doc 1.24), executor 6.3, planning ~9.7 (~20 %), main sessions ~5.4. Lever 4 (main session per request) is not requested. Lever 1 as proposed: (a) on "OK, tout par défaut" the planner is resumed only to write answers (phase 2 ~0.15-0.2 M per run at 107k+ context): a script applies the defaults instead; (b) `_paths.md` (~32 KB, ~9k tokens) is read early in every planner run and stays in context: a resolver script prints the path of one given id (resolution, not search); (c) task-357 planning spent ~15 of 36 turns writing nodes: `task-create.js` takes one spec file and creates the globals, every subtask and the index lines in one call. Planner cost per run ~0.72 M (~35 turns, context 100-130k). Owner answer 2026-10-29 (relayed by the main session, verbatim): "OK, tout par défaut" = the plan and every default of Q1 to Q12 accepted (owner intent for the rule edits of `_navigation` and `orchestration-protocol` steps 3 and 4 given through Q1, Q2, Q7).
- goal: A planning pass writes its whole plan in one script call, resolves ids without loading _paths.md, and an all-defaults owner answer is applied without resuming the planner.
- acceptance: `node vault/scripts/path-of.js <id> [<id>...]` prints the repo-relative path of each exact id (task ids included) and exits 1 on an unknown id; `task-create.js` accepts a spec with full global fields and an ordered list of subtasks of any agents and keeps the old form working; `task-confirm.js` applies a defaults file and the owner words, sets `confirmed_by_owner`, status `todo`, lints and commits by path; a dry run of each on a scratch copy shown in the change summary; planner, runbook, protocol, navigation and main-session instructions describe them; lint clean; committed.
- gdd_check: crew tooling, no gdd node involved (Planner, 2026-10-29). Read: task-create.js, agent-planner, orchestration-protocol, orchestrator-runbook (Creating tasks), _navigation, `.claude/commands/task.md`. Findings: task-create.js already exists but writes one subtask per global and generic `request`/`gdd_check` lines, so the planner rewrites them by hand; a resolver of an exact id is compatible with [[_navigation]] (a wikilink id is resolved to its path through `_paths.md`; searching to locate a node is forbidden) provided it never matches partial ids; `_navigation` and `orchestration-protocol` are `kind: rule`: editing them needs explicit owner intent (Q2, Q1). The apply-defaults script changes who acts on a pure "ok" (main session instead of the planner), so CLAUDE.md and `.claude/commands/task.md` change (Q1).
- confirmed_by_owner: yes (2026-10-29, "OK, tout par défaut" to Q1-Q12 and the plan, relayed by the main session)
- milestone: M2
- next_step: task-358-01-planner-cost-scripts
- started: 2026-10-29T14:09:28Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-29 (owner "OK, tout par défaut"): every default below is accepted; values provisional, owner 2026-10-29.
- Q1 (lever 1a) At step 4 the planner also writes `Saved/Planner/<first-global-id>.defaults.json` (not committed): the globals concerned, the gdd edits its defaults imply as exact replacements (file, old text, new text), and the Questions to mark decided. On a pure "ok" / "tout par défaut", the main session runs `node vault/scripts/task-confirm.js <file> "<owner words verbatim>"`: it applies the edits, marks `## Questions` decided with the date and words, appends the words to each `request`, sets `confirmed_by_owner: yes (<date>, "<words>")` and status `todo`, lints and commits by path. Any other answer, or a missing file, resumes the planner as today. Default: yes; this request is the owner intent to change orchestration-protocol step 4, CLAUDE.md and `.claude/commands/task.md` accordingly.
- Q2 (lever 1b) `node vault/scripts/path-of.js <id> [<id>...]`: exact ids only (no partial or fuzzy match, so it stays resolution, not search), non-task ids from the same map lint.js builds, task ids from the tasks folder; the planner and agents use it instead of opening `_paths.md`, which stays generated for other tools. Default: yes, with one line added to [[_navigation]] "Resolving a wikilink" (this request is the owner intent for that rule edit).
- Q3 (lever 1c) `task-create.js` spec v2: one module per planning pass; each global carries request, goal, acceptance, gdd_check, confirmed_by_owner, milestone, optional `## Questions` lines and initial status (`todo` or `wait-answers` with `waiting_for`), and an ordered list of subtasks (slug, agent, goal, inputs as ids or literal paths, depends_on, expected_output, done_when, editor_state, token_ceiling); it writes nodes, `## Subtasks`, Relations and index lines; the old array form keeps working. Default: yes.
- Q4 The three scripts are one owner point (lever 1, all in `vault/scripts/`, one agent): one cpp subtask, then the doc subtask. Default: yes.
- Q12 Run order of the four globals of this request: 358, 359, 360 (after 359: its definition uses tool-capture), 361; one executor per global, no editor needed for any. Default: yes.

## Subtasks
1. task-358-01-planner-cost-scripts (vault/reference/tasks/task-358-planner-cost/task-358-01-planner-cost-scripts.md) — cpp: path-of.js, task-confirm.js, task-create.js spec v2; change summary with dry runs.
2. task-358-02-planner-cost-doc (vault/reference/tasks/task-358-planner-cost/task-358-02-planner-cost-doc.md) — doc: runbook, planner, protocol step 4, _navigation, planner.md, task.md command, CLAUDE.md describe the three scripts.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-358-01-planner-cost-scripts]] — step 1, cpp.
- [[task-358-02-planner-cost-doc]] — step 2, doc.
- [[task-359-capture-tooling]] — lever 2, same request.
- [[task-360-capture-agent]] — capture agent, same request.
- [[task-361-bp-agent-cost]] — lever 3, same request.
