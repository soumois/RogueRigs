---
id: task-020-03-refactor-cleanup
description: "Refactor R3, R6, R7 from code-review-m1-plan (cached CSV loader, shared damage numbers, cheats and test hooks grouped and shipping-guarded); no behavior change."
parent: task-020-code-review
kind: task
status: done
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
- model: opus
- tokens: input 110, cache write 116713, cache read 3704761, output 39544
- runs: 1
- milestone: M1
- result: R3 FRRBaseStatsLoader caches one parsed table per CSV (re-parse on file timestamp change) for robots, weapons, player, explosions, XP curve, upgrades; test Enemies.StatsTableCached. R6 ARRFloatingDamageText::SpawnForHit shared by ARREnemy and ARRTargetDummy, class moved Debug/ → Damage/ (path unchanged). R7 run-flow and console cheat functions no-op in shipping (ARRConsole::CompleteHack split out so shipping hacking still completes), stress subsystem not created in shipping, test access structs merged into Tests/RRTestAccess.h. 102 → 103/103; Shipping target builds; no rename, no CoreRedirect; cheat property route and CVars unchanged. Docs plan marks + 12 system docs. Commits e3dbef5, de16507, c415928.

## Relations
- [[task-020-code-review]] — parent global task.
