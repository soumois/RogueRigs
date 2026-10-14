---
id: assignment-5-review-checks
description: "Proposed checks for the Assignment 5 code-quality criterion: path limits, pattern file, literal scan, build, tests, owner diff review. Run after the agent writes."
parent: assignment-5-goal-oriented-agent
kind: note
---
# Assignment 5 — Review checks (proposal)

The rubric asks that generated code follows existing patterns, has no obvious bug and no hard-coded test values. Nothing in the course says how to verify it; this is the crew's proposal. Results go in the blackboard and in the README.

## Automatic, run by the Code Architect after writing
1. Paths: every file is under `Source/RogueRigs/` or `Content/`; no existing file is overwritten unless the prompt listed it.
2. Pattern file: the prompt included one existing file of the same system; the blackboard names it.
3. Literal scan: numeric literals other than 0, 1 and -1 in the new `.cpp` and `.h` files are listed. Tunable values must come from a data table or `UPROPERTY(EditDefaultsOnly)` ([[cpp-standard]]).
4. Conventions: `RR` prefix, header comment on each class and function, logs through `LogRogueRigs` ([[cpp-standard]]).

## Run by the crew
5. Command-line build with the editor closed exits 0, with no new warning in touched files ([[agent-cpp]]).
6. The headless `RogueRigs.*` automation tests pass ([[sys-automation-tests]]).
7. A `sys-*` doc node for the feature exists in `tech/systems`, and `node vault/scripts/lint.js` is clean.

## Run by the owner
8. Reads the whole diff. What he changes is listed in the README with the reason ("follows the patterns" is not "matches the design intent", [[lesson-07-acting-and-agent-state]]).
9. Plays the feature through the one-click test ([[sys-cheats]]) and checks a capture.

## Relations
- [[assignment-5-goal-oriented-agent]] — parent: brief and rubric.
- [[assignment-5-agent-design]] — the writing step these checks follow.
- [[assignment-5-open-decisions]] — decisions that change the checks.
