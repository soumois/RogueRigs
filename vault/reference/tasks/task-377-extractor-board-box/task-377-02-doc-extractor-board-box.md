---
id: task-377-02-doc-extractor-board-box
description: "Doc: update sys-objectives-shuttle, sys-objectives-run-flow, bp-extraction-shuttle, sys-tests-console-extraction and sys-tests-dig-ore."
parent: task-377-extractor-board-box
kind: task
status: done
---
# Extractor: adjustable box collision for the end of the level: doc

- goal: The doc nodes state the BoardZone box, its defaults, where the owner adjusts it, and the removed radius.
- agent: doc
- depends_on: task-377-01-cpp-extractor-board-box
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), sys-tests-console-extraction (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-console-extraction.md), sys-tests-dig-ore (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-dig-ore.md), task-377-extractor-board-box (vault/reference/tasks/task-377-extractor-board-box.md) request, gdd_check and Questions (decided values)
- expected_output: the five nodes updated from the cpp change summary with its commit hash (bp-extraction-shuttle: inherited BoardZone listed in components, adjustable in the Details panel, C++ defaults kept until the owner edits it); one commit.
- done_when: nodes written; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 22, cache write 36183, cache read 245505, output 6711
- usage: turns 11, context start 12k peak 36k, resumes 0, 1 min, units 0.10 M
- runs: 1
- milestone: M2
- started: 2026-10-30T00:48:21Z
- waiting_for: none
- result: Five nodes updated (sys-objectives-shuttle, run-flow, bp-extraction-shuttle, console-extraction tests, dig-ore tests); gdd extraction already had box wording; commit 8e331c67

## Relations
- [[task-377-extractor-board-box]] — parent global task.
