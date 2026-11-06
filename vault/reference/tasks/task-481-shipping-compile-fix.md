---
id: task-481-shipping-compile-fix
description: "Global task: Removes the C2027 on ARRWeapon that stops RogueRigs Win64 Shipping from compiling. Open to run the fix behind the v0.1.3 build; gameplay unchanged."
parent: tasks-done-index
kind: task
status: done
---
# Fix the compile error that blocks the Shipping build

- request: "est-ce que tu peux faire un build RogueRigsv0.1.3 en non développement dans le dossier : C:\Users\soumo\Documents\Unreal Projects\RogueRigs\Saved\StagedBuilds" (owner words verbatim). This task covers only the code piece of that request, the compile error blocking the build. The main session runs the packaging itself, before and after this task (BuildCookRun, -clientconfig=Shipping, staged then renamed to Saved/StagedBuilds/RogueRigsv0.1.3). Owner answer to the plan, relayed by the main session on 2026-11-06: "Oui, corrige".
- goal: RogueRigs Win64 Shipping compiles again, without changing gameplay, so the main session can rerun the packaging.
- acceptance: A1 the target RogueRigs Win64 Shipping compiles, with no C2027 or C2672 on ARRWeapon; A2 the target RogueRigsEditor Win64 Development still compiles; A3 gameplay does not change, so build only under the test scope rule of build-toolchain: no test run, no packaging, no capture.
- gdd_check: No gdd node read: a compile fix with no gameplay change touches no game rule. No conflict, no open value, milestone scope not concerned. Failure established by the main session at HEAD 272de043 with a clean Source tree: RunUAT BuildCookRun -noP4 -platform=Win64 -clientconfig=Shipping -build -cook -stage -pak -map=/Game/RogueRigs/Maps/L_MainMenu+/Game/RogueRigs/Maps/L_ArenaM1 -unattended -utf8output, exit code 6 at the compile step. Errors in unit Module.RogueRigs.7.cpp: SubclassOf.h(98,38) C2027 undefined type 'ARRWeapon' and C2672 'StaticClass'; instantiation context Source/RogueRigs/Upgrades/RRUpgradeCard.h line 175, in the inline FRRUpgradeCard::IsSameChoice. Likely cause per the main session, not verified by the Planner: a latent header defect surfaced by unity grouping, since RRUpgradeCard.h last changed on 2026-10-14 and a Shipping build from 2026-11-05 compiled. No code read by the Planner. No doc subtask: nothing a sys-*, bp-* or tool node describes is expected to change; the Executor adds a line to the relevant node only if the fix does change one. No question for the owner.
- confirmed_by_owner: yes (2026-11-06, "Oui, corrige", relayed by the main session)
- milestone: M2
- next_step: none
- started: 2026-11-06T22:31:15Z
- waiting_for: none
- executor_tokens: input 60, cache write 77769, cache read 1620116, output 19093
- executor_usage: runs 1, turns 26, context start 17k peak 90k, resumes 0, 10 min, units 0.35 M
- planning_tokens: input 24, cache write 42262, cache read 314236, output 6828
- planning_usage: af036345 "Plan Shipping compile fix" 1/1 (tasks 481), units 0.12 M
- result: Done. Fixed in 9c2b50eba: FRRUpgradeCard::IsSameChoice moved unchanged from RRUpgradeCard.h to new Upgrades/RRUpgradeCard.cpp, stale workaround comments removed from three Run headers. Cause confirmed: in Shipping WITH_DEV_AUTOMATION_TESTS compiles out the RRWeapon.h includes of the test files, while the unguarded RRTestEventRecorder.cpp pulls the card header, so unit 7 instantiated TSubclassOf<ARRWeapon> on an incomplete type. Failure reproduced before the fix (Shipping exit 6). After the fix: Shipping exit 0, editor exit 0, Shipping with -DisableAdaptiveUnity exit 0, no C2027 or C2672. Build only, no gameplay change.

## Subtasks
1. task-481-01-cpp (vault/reference/tasks/task-481-shipping-compile-fix/task-481-01-cpp.md) — cpp: Remove the ARRWeapon compile error so both the Shipping game target and the Development editor target compile.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-481-01-cpp]] — step 1, cpp.
