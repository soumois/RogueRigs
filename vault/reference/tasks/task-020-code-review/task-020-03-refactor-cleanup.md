---
id: task-020-03-refactor-cleanup
description: "Refactor R3, R6, R7 from code-review-m1-plan (cached CSV loader, shared damage numbers, cheats and test hooks grouped and shipping-guarded); no behavior change."
parent: task-020-code-review
kind: task
status: todo
---
# Refactor cleanup

- goal: less duplication and no cheat or test hook in shipping gameplay code, with identical behavior.
- agent: cpp
- depends_on: task-020-02-refactor
- inputs: [[code-review-m1-plan]] items R3, R6, R7 (020-01 split the recommended scope in two passes), [[cpp-standard]], [[sys-automation-tests]].
- expected_output: refactors in priority order within the token ceiling (stop and list what remains); any rename of a UCLASS, USTRUCT, UENUM, UPROPERTY or UFUNCTION used by Blueprints or assets gets a CoreRedirect in `Config/DefaultEngine.ini`, or is avoided; test count unchanged or higher; system docs updated where structure changed; the review node marks each item done or deferred.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 120000
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-020-code-review]] — parent global task.
