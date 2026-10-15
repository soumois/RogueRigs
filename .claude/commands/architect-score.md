---
description: Code Architect, scoring only. Scans the GDD against the code and proposes the missing features in priority order, in the chat only; writes nothing, builds nothing.
argument-hint: [optional: a constraint or focus, in your own words]
---
Act as the **Code Architect** of `.claude/commands/architect.md`, but run **only its steps 1 (Scan) and 2 (Score)**, then answer in the chat.

Owner constraint for this run (may be empty): $ARGUMENTS

## Read-only
- Write nothing: no blackboard, no `state.md`, no task node, no vault edit, no code, no commit. No sub-agent that writes.
- Read-only scan agents are allowed (one per GDD branch, as in run 1) to keep the main context small.

## Steps
1. **Context.** Read `assignments/A5-goal-oriented-coding-agent/state.md` (built, left, decisions) and `vault/reference/tasks/tasks-to-do-index.md`. The `Left` table is a hint only: rescan, never copy it.
2. **Scan.** Same rule as `architect.md` step 1: every GDD `kind: spec` node (approved or provisional; skip deferred and wishlist), each sub-feature `built` / `partial` / `missing` with an evidence path that exists.
3. **Score.** Same factors as `architect.md` step 2: milestone (M1 = 3, M2 = 2, M3 = 1), pillar fit (0–2), dependencies ready (no = 0, "blocked by X"), gaps it closes or unblocks, cost (+1 `partial`), `open` or missing value = not buildable. Total = milestone + pillar + unblocks + cost.

## Answer in the chat
- **Ranked list** of the buildable gaps, most relevant first: feature (spec), total and the score per factor, one-line reason, rough size (one subtask, several, very large), and the agents it would need.
- Mark a gap already covered by an open task in `tasks-to-do-index` (name the task) instead of proposing it twice.
- **Not buildable**, grouped by blocker: the `open` value or the missing dependency to settle first.
- **Changes since the last run** of `state.md`: what is now built, what is new.
- End with your top recommendation in one sentence. Do not start anything: to build it, the owner runs `/architect` or `/task`.
