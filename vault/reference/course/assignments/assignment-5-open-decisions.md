---
id: assignment-5-open-decisions
description: "Assignment 5 decisions the owner has not taken yet, each with the crew's proposal; none answered on 2026-10-14. Open before building the agent."
parent: assignment-5-goal-oriented-agent
kind: note
---
# Assignment 5 — Open decisions

Only the repo folder is decided (2026-10-14). Each line is a proposal awaiting the owner's answer; record answers here with the date.

1. Language: Node.js in `Tools/architect/`, like the other tools (proposal), or Python like the course scaffold.
2. Model call: headless Claude Code (`claude -p`) with the [[agent-cpp]] prompt, no API key (proposal), or the Anthropic API through an environment variable (cost per run). Keys never go in the repo or the vault.
3. Run scope: one feature (proposal) or several.
4. Approval gate: dry run, then an explicit approval argument naming the feature (proposal), or automatic above a score threshold.
5. Output: a branch `architect/<feature-id>`; the owner merges after review (proposal). Pushing is the owner's job ([[cloud-session-git]]).
6. Veto rule: the owner accepts the top buildable feature; a veto needs a recorded reason, quoted in the README as direction (proposal).
7. Candidate with open fields (for example audio): decide the fields first only if it ranks first; otherwise leave it (proposal).
8. Repo folder `assignments/A5-goal-oriented-coding-agent/` with root README and `assignments/README.md` rows: decided and created by the owner, 2026-10-14. Tag `a5-submission` (proposal; the README is final-state, as for earlier assignments).
9. Internal target: submit Sunday October 18 evening, Monday 15:59 Pacific is the buffer.
10. One-click test: a cheat in [[sys-cheats]] that triggers the feature (proposal).
11. Name of the agent: Code Architect (the instructor's demo name; proposal).
12. Damage fraction of the 3 extra shots of 4-direction fire, absent from the GDD and the data tables ([[assignment-5-candidate-features]]). Ask only if that feature ranks first. Proposal: 0.5 of the main shot, in a data table.

## Relations
- [[assignment-5-goal-oriented-agent]] — parent: brief and rubric.
- [[assignment-5-plan]] — the plan these decisions unblock.
- [[assignment-5-agent-design]] — the design that depends on 1 to 5.
- [[assignment-5-review-checks]] — how the code-quality criteria are verified.
