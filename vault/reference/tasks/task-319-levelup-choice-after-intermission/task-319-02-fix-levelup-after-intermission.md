---
id: task-319-02-fix-levelup-after-intermission
description: "Fix the cause found in step 1 so level-ups after the intermission queue an upgrade choice again; the reproduction test passes."
parent: task-319-levelup-choice-after-intermission
kind: task
status: todo
---
# Fix: level-up choice after the intermission

- goal: fix the cause written in the result of task-319-01 so every level-up after an intermission queues its upgrade choice, on the fade path and the loading path. Smallest change at the cause; keep CancelChoices at the end of a level (no choice screen over the intermission). Runs right after step 1 if step 1 reproduced the bug; owner confirmed Q1 "Oui (Recommended)" (2026-10-26): fix in the same task, no capture, no other new test.
- agent: cpp
- depends_on: task-319-01-levelup-choice-after-intermission
- inputs: task-319-01-levelup-choice-after-intermission (vault/reference/tasks/task-319-levelup-choice-after-intermission/task-319-01-levelup-choice-after-intermission.md, its result), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ fix in Source/RogueRigs/; the system doc of the changed system updated (sys-level-up or sys-run-transition*)
- done_when: build passes; the task-319-01 test passes on both transition paths; all existing automation tests pass; no choice screen opens during the intermission; level-up choices in level 1 unchanged; doc updated; lint clean. No capture, no new test, no bot run.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-319-levelup-choice-after-intermission]] — parent global task.
- [[task-319-01-levelup-choice-after-intermission]] — the reproduction and cause it fixes.
