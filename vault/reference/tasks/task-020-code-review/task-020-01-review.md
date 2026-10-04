---
id: task-020-01-review
description: "Read all of Source/RogueRigs: readability, comments, naming, duplication, base-class generalization; write a ranked refactor list with risk."
parent: task-020-code-review
kind: task
status: done
---
# Code review

- goal: know what to refactor and why.
- agent: cpp
- depends_on: task-019-01-asset-manager
- inputs: `Source/RogueRigs/` (all modules), [[cpp-standard]], [[cpp-bp-split]], [[naming]], [[systems-index]].
- expected_output: new node `code-review-m1` under tech conventions (or a new review subfolder with an index if over 400 words): per system, what a new human reader would stumble on (long functions, unclear names, missing or stale comments, magic numbers, test-only hooks in gameplay code, duplicated logic, classes that should share a base: robots, weapons, pickups, objectives, cheats), and a ranked refactor list (gain, risk, hours, Blueprint impact: reflected names that would need CoreRedirects). No code change in this subtask. Also list dead code and files over ~800 lines.
- done_when: the review node exists with concrete file and function references; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: opus
- tokens: input 116, cache write 187782, cache read 7196739, output 37830
- runs: 1
- milestone: M1
- result: Verdict: readable (complete header comments, short functions, data-driven values, cheats centralised) but not generalised at robot and query level. Top findings: robot state plumbing duplicated in chaser and kamikaze; ARRProjectile hits any ASC (enemy shots would hit robots); grid and miner lookups copied 13 times; reachable-cell scan copied 4 times; CSV loading copied 3 times and EnemyStats parsed per spawn; ARRWeapon tied to the miner (non-virtual aim); kamikaze tuning split; damage text duplicated; cheats and test Access structs inside gameplay and shipping; dead Variant_TwinStick (1,875 lines) and URRGameplayAbility. Recommended R1 to R7 (about 11 h, no CoreRedirect), deferred R8 to R16 with triggers. Nodes code-review-m1 (index) + gameplay, systems, hooks, plan, deferred. Commit 52541ca. Orchestrator split the recommended scope into 020-02 (R1, R2, R4, R5) and 020-03 (R3, R6, R7); the Blueprint check became 020-04.

## Relations
- [[task-020-code-review]] — parent global task.
