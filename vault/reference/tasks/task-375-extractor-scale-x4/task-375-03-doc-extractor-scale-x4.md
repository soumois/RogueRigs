---
id: task-375-03-doc-extractor-scale-x4
description: "Doc: doc: update sys-objectives-shuttle, sys-objectives-run-flow and bp-extraction-shuttle."
parent: task-375-extractor-scale-x4
kind: task
status: done
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
- model: sonnet
- tokens: input 10, cache write 37929, cache read 339189, output 3754
- usage: turns 5, context start 59k peak 82k, resumes 0, 0 min, units 0.10 M
- runs: 1
- milestone: M2
- started: 2026-10-29T23:47:08Z
- waiting_for: none
- result: sys-objectives-shuttle, sys-objectives-run-flow, bp-extraction-shuttle updated; commit c29c0886

## Relations
- [[task-375-extractor-scale-x4]] — parent global task.
