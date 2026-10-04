---
id: task-020-02-refactor
description: "Apply the high-value, low-to-medium-risk refactors from code-review-m1 with no behavior change; all tests green before and after."
parent: task-020-code-review
kind: task
status: todo
---
# Refactor

- goal: cleaner, more general C++ with identical behavior.
- agent: cpp
- depends_on: task-020-01-review
- inputs: `code-review-m1` (created in 020-01), [[cpp-standard]], [[sys-automation-tests]].
- expected_output: refactors in priority order within the token ceiling (stop and list what remains); any rename of a UCLASS, USTRUCT, UENUM, UPROPERTY or UFUNCTION used by Blueprints or assets gets a CoreRedirect in `Config/DefaultEngine.ini`, or is avoided; test count unchanged or higher; system docs updated where structure changed; the review node marks each item done or deferred.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 150000
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-020-code-review]] — parent global task.
