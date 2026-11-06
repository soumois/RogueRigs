---
id: task-481-01-cpp
description: "Cpp: Remove the ARRWeapon compile error so both the Shipping game target and the Development editor target compile."
parent: task-481-shipping-compile-fix
kind: task
status: done
---
# Fix the compile error that blocks the Shipping build: cpp

- goal: Remove the ARRWeapon compile error so both the Shipping game target and the Development editor target compile.
- agent: cpp
- depends_on: none
- inputs: build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), Source/RogueRigs/Upgrades/RRUpgradeCard.h
- expected_output: see done_when
- done_when: RogueRigs Win64 Shipping and RogueRigsEditor Win64 Development both compile, with no C2027 or C2672 on ARRWeapon; no gameplay behaviour changed; no test run, packaging or capture added.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-481
- usage: included in the executor run of task-481
- runs: 1
- milestone: M2
- started: 2026-11-06T22:31:15Z
- waiting_for: none
- result: Fixed in 9c2b50eba: FRRUpgradeCard::IsSameChoice moved unchanged from RRUpgradeCard.h to new Upgrades/RRUpgradeCard.cpp, stale workaround comments removed from three Run headers. Cause confirmed: in Shipping WITH_DEV_AUTOMATION_TESTS compiles out the RRWeapon.h includes of the test files, while the unguarded RRTestEventRecorder.cpp pulls the card header, so unit 7 instantiated TSubclassOf<ARRWeapon> on an incomplete type. Failure reproduced before the fix (Shipping exit 6). After the fix: Shipping exit 0, editor exit 0, Shipping with -DisableAdaptiveUnity exit 0, no C2027 or C2672. Build only, no gameplay change.

## Relations
- [[task-481-shipping-compile-fix]] — parent global task.
