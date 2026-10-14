---
id: assignment-5-agent-design
description: "Proposed design of the Assignment 5 agent (Code Architect): inputs, gap detection, scoring factors, blackboard, state file, approval gate, writing step. Not decided."
parent: assignment-5-goal-oriented-agent
kind: note
---
# Code Architect — proposed design (all choices open, see [[assignment-5-open-decisions]])

## Layout (proposed)
`Tools/architect/`: one entry script plus modules for GDD parsing, code scan, scoring, writing and blackboard; `weights.json` for scoring weights (no literals in code); `state.md`; `runs/<run-id>/blackboard.md` and `.json`.

## Pipeline
1. GDD features: parse `vault/reference/gdd/` nodes of `kind: spec`. Keep `approved` and `provisional`; skip `deferred` and anything listed only in [[wishlist]]. Record milestone, pillar fit, wikilink dependencies and every `open` field.
2. Code inventory: file list of `Source/RogueRigs/`, class names, the system docs in `tech/systems` ([[systems-index]]), data tables in `Content/`.
3. Gap detection: each feature is `built`, `partial` or `missing`, with the file or symbol as evidence. A claimed evidence path that does not exist is rejected.
4. Scoring: weights in `weights.json`.
| Factor | Source | Effect |
|---|---|---|
| Project priority | milestone (M1 over M2 over M3), pillar fit | raises |
| Blocks | number of other gaps that depend on it | raises |
| Dependencies ready | all required features `built` | else score 0, reason "blocked by X" |
| State | `partial` is cheaper than `missing` | raises |
| Open fields | owner decision needed | not buildable, reported |
5. Gate: dry run by default (blackboard only). Writing needs an explicit approval argument naming the feature.
6. Writing: prompt = GDD node verbatim + tech doc of the touched system + conventions + one existing file as pattern ([[agent-cpp]]). The exact prompt is logged before the model call. Files may only be created or changed under `Source/RogueRigs/` and `Content/`, on a branch `architect/<feature-id>`; never on master.
7. Loop: rescan; the feature becomes `built`; stop when no buildable gap remains or after one feature (proposed).
8. State: update `state.md` with built, failed, left, decisions ([[lesson-07-acting-and-agent-state]]).

## Relations
- [[assignment-5-goal-oriented-agent]] — parent: brief and rubric.
- [[assignment-5-plan]] — schedule and actors.
- [[assignment-5-candidate-features]] — what the scan is expected to find.
- [[assignment-5-review-checks]] — checks run on the generated code before the owner reads it.
- [[agent-cpp]] — conventions the generated code must follow.
- [[sys-cheats]] — route for the one-click test of the feature.
