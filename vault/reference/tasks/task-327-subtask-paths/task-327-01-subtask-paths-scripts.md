---
id: task-327-01-subtask-paths-scripts
description: "task-start.js and task-next.js print only errors, alerts, refusals, commit line and next: path; task-create.js writes subtask paths in Subtasks."
parent: task-327-subtask-paths
kind: task
status: done
---
# Quiet task scripts ending with the subtask path

- goal: apply owner decisions (1) and (2) and Planner decisions D1 to D4 of [[task-327-subtask-paths]] `gdd_check`. (a) `task-start.js` and `task-next.js` capture the stdout and stderr of every child they run (`task-status.js`, `record-cost.js`, `lint.js`, `git` with its pre-commit hook) and print only the D3 kept lines, dropping the D3 noise; a failing child makes the script print that child's own error output (lint problem lines, hook output, refusal text such as `confirmed_by_owner` not yes, `tokens is still open`, `waiting_for` missing) without a Node stack, and exit non-zero. (b) After the commit, each prints the D4 leftover lines, then as its last line `next: <absolute path>` of the subtask it started (`task-start.js`: the picked subtask, also on the idempotent "already in-progress" run; `task-next.js`: `<next-global>/<next-subtask>.md` when given), or `next: none` when it started none. (c) `task-start.js` parses both Subtasks forms (`1. <id> — ...` and the D2 form `1. <id> (vault/...md) — ...`). (d) `task-create.js` writes the Subtasks line in the D2 form. (e) Update the header comments of the three scripts. Shared filtering code may go in a small helper module in `vault/scripts/`. No other behaviour change; edit no vault node: return a change summary (files, commit hash, kept and dropped line patterns, exact `next:` format, Subtasks format) for the doc subtask.
- agent: cpp
- depends_on: none
- inputs: task-327-subtask-paths (vault/reference/tasks/task-327-subtask-paths.md), task-start.js (vault/scripts/task-start.js), task-next.js (vault/scripts/task-next.js), task-create.js (vault/scripts/task-create.js), task-status.js (vault/scripts/task-status.js), record-cost.js (vault/scripts/record-cost.js), lint.js (vault/scripts/lint.js), pre-commit hook (vault/scripts/hooks/pre-commit)
- expected_output: the edited scripts (and the helper if any); a change summary.
- done_when: tested in a throwaway `git worktree` on a temporary branch with a throwaway global and two subtasks (worktree and branch removed after; no live task node touched): `task-start.js` prints no `warn:`, `vault OK` or line-ending warning line and ends with `next: <absolute path of the first subtask>`; a second run prints the "already in-progress" line and the same `next:`; with `confirmed_by_owner: no` it prints the refusal and exits non-zero; with a node that breaks lint the lint problem lines appear and it exits non-zero; `task-next.js` closing subtask 1 with subtask 2 as next ends with `next: <absolute path of subtask 2>`, and closing the last one ends with `next: none`; an `ALERT ...` line and a `record-cost.js` error still reach the output (in the worktree copy only, `record-cost.js` may be stubbed to print them, since a throwaway id has no agent usage); `task-create.js` on a throwaway spec writes the D2 Subtasks form and `task-start.js` parses it; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 48, cache write 61619, cache read 1087836, output 23831
- usage: turns 24, context start 11k peak 62k, resumes 0, 4 min, units 0.31 M
- runs: 1
- milestone: M2
- started: 2026-10-27T14:58:21Z
- waiting_for: none
- result: Done: commit fb1d7dd0; task-quiet.js helper, task-start/next/create updated; tested in throwaway worktree.

## Relations
- [[task-327-subtask-paths]] — parent global task.
