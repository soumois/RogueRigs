---
id: assignment-5-vault-check
description: "Cold-start test of 2026-10-14: 26 questions a fresh agent answered from the vault alone about Assignment 5, results, fixes. Open to re-run it after changes."
parent: assignment-5-goal-oriented-agent
kind: note
---
# Assignment 5 — Vault readiness check (2026-10-14)

Why: the owner changes Claude account, so the vault must carry everything the conversation knew. Do NOT open this node during a cold test: it lists the questions.

## Method
1. Write the questions and a private answer key BEFORE writing nodes (course facts from the lesson 07 summary, project facts from a scan of the repo).
2. Write the nodes.
3. Start a fresh sub-agent with the vault protocol (root, `_navigation`, state a clause before each open, no search, no invented paths, mark `open` fields as undecided, no reading outside `vault/`). Give it the questions, not the key.
4. Compare answer by answer with the key; fix the vault, not the answer; re-test only the gaps.

## Questions (themes)
Deadline and timezone; deliverables; rubric; starter scaffold; blackboard; merge and approval rules; utility scoring; codebase perception; agent state; post-run owner duties; frameworks; zero-credit cases; status and hashes of A1 to A4; features missing from the code and why; which need an owner decision; decisions already taken (expected: none); owner and crew task lists; assets reused by the writing step; verification of code quality; trap questions (scoring formula chosen? feature chosen? expected: no); cloud git procedure; folder, README and tag; time plan; trajectory.

## Result
- Round 1: 22 of 26 answered from the vault; the three trap or decision questions were answered "open". Gaps: no verification procedure for code quality, A3 commit missing, value of the extra shots of 4-direction fire absent, README rule not recorded, cloud git procedure not recorded.
- Fixes: [[assignment-5-review-checks]], [[assignment-5-candidate-features]], [[assignment-5-open-decisions]] item 12, [[cloud-session-git]], [[course-overview]], [[assignments-index]].
- Round 2 (fresh agent, 5 gap questions): all found.
- Not covered: the owner's personal conversation preferences (language, reply style) live in the Claude account memory, not in the vault.

## Relations
- [[assignment-5-goal-oriented-agent]] — parent: brief and rubric.
- [[assignment-5-plan]] — the plan that was tested.
- [[_navigation]] — the protocol the test agent followed.
