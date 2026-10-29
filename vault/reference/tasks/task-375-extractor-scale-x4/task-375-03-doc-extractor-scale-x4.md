---
id: task-375-03-doc-extractor-scale-x4
description: "Doc: doc: update sys-objectives-shuttle, sys-objectives-run-flow and bp-extraction-shuttle."
parent: task-375-extractor-scale-x4
kind: task
status: todo
---
# Extractor: four times bigger, with its level-ending zone and footprint: doc

- goal: The doc nodes state the x4 scale, the 16 x 12 footprint, the 12-cell board radius and the chevron threshold.
- agent: doc
- depends_on: task-375-01-cpp-extractor-scale-footprint, task-375-02-bp-shuttle-scale-x4
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md)
- expected_output: the three nodes updated from the change summaries with the commit hashes; one commit.
- done_when: nodes written; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-375-extractor-scale-x4]] — parent global task.
