---
id: task-437-02-doc
description: "Doc: Document the new targets of the spark."
parent: task-437-shot-impact-on-walls
kind: task
status: done
---
# Shot impact sparks on walls: doc

- goal: Document the new targets of the spark.
- agent: doc
- depends_on: task-437-01-spark-on-block
- inputs: bp-shot-impact-spark (vault/reference/tech/blueprints/fx/bp-shot-impact-spark.md)
- expected_output: see done_when
- done_when: The node and the shot code doc it links state that blocks count; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-437
- usage: included in the executor run of task-437
- runs: 1
- milestone: M2
- started: 2026-11-05T17:59:52Z
- waiting_for: none
- result: Done. bp-shot-impact-spark and sys-projectiles state that a stop on any block the drone can dig plays the spark; commit 2780395a; lint clean.

## Relations
- [[task-437-shot-impact-on-walls]] — parent global task.
