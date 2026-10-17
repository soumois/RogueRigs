---
id: task-212-02-writer
description: "cpp: At each level end"
parent: task-212-run-logs
kind: task
status: todo
---
# writer

- goal: At each level end (extracted, death, missed shuttle) the run log adds the level entry with level outcome, time per level, time left on reaching the shuttle, ore collected during the risk window, events met and their outcome, then writes the run JSON file; run-log-schema gets the format and field names.
- agent: cpp
- depends_on: task-212-01-collector
- inputs: run-log-schema (vault/reference/tech/pipelines/run-log-schema.md), sys-run-log (vault/reference/tech/systems/world/sys-run-log.md, written by task-212-01), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), sys-spawner-director (vault/reference/tech/systems/enemies/sys-spawner-director.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/RunLog/, sys-run-log and run-log-schema updated, a sample file path in the report.
- done_when: build succeeds; tests pass except the known failures; a -game run of 2 levels writes the file with both entries and every field; docs updated; committed.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-212-run-logs]] — parent global task.
