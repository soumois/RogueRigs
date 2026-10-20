---
id: style-loop
description: "Style loop for generated text: command, script subcommands, run layout, score file contract, gate and pass-limit rules. Open to run, debug or extend the loop."
parent: pipelines-index
kind: doc
---
# Style loop (Generate, Evaluate, Refine)

Assignment 6. The model steps are agents; everything that must not depend on a model is the script `Tools/style/style_loop.js` (Node, no dependency). Rules and scale: [[style-guide-text]].

## Roles
- Generator: [[agent-text]] (sonnet), pass 0 content and refinements.
- Evaluator: [[agent-style-guide]] in scored mode (opus, a different model from the generator).
- Script: format check by the validator, threshold, contract checks, pass limit, report.
- Driver: the Orchestrator session, command `.claude/commands/style-loop.md`. Scripts cannot call a model in this environment.

## Run layout (`Tools/style/runs/<run-id>/`)
- `loop.json`: threshold 8, max_refinements 2, round, status (open, done, escalated), lineage of every correction.
- `current/<stem>.json`: the content as it stands; only `apply` writes it.
- `format.json`: validator findings mapped to rules (T1, T7, T8, F1, F2).
- `roundN.score.json`: evaluator output. `roundN.gate.json`: items below threshold, with scores, rule, reason, suggestion.
- `refined/roundN/<stem>.json` and its log: one validated run per refinement, in the [[text-content-schema]] format.
- `report.md`, and `escalation.md` when the breaker opens.

## Score file
`{"round":N,"items":[{"file","item_id","scores":{"voice","lore","reference"},"rules":[ids],"reason","suggestion"}]}`. Integers 0 to 10. Every item has a reason; a failing item has rules and a suggestion.

## Script
`node Tools/style/style_loop.js init|check|gate|apply|report <runDir> …` and `--selftest` (12 checks). Exit 1 is a contract violation.
- `gate` also checks F4 (quoted film in the catalogue) on the current text of every round.
- `gate` refuses a missing, doubled or unknown item, a score outside 0 to 10, an unknown rule id, a wrong round. It prints `done`, `refine` or `escalate`.
- `apply` accepts only the failing ids, only the text field (and `tone` for barks, `register` for alerts) changed, a text different from before, and a refined file the validator accepts.

## Honesty limits
- The loop never edits a rule. A rule judged wrong is reported to the owner.
- After 2 refinements, remaining fails go to the owner with `escalation.md`.

## Relations
- [[pipelines-index]] — parent index.
- [[style-guide-text]] — dimensions, scale, threshold.
- [[text-content-validator]] — the format half of every round.
- [[agent-text]] — the refiner.
- [[agent-style-guide]] — the evaluator.
