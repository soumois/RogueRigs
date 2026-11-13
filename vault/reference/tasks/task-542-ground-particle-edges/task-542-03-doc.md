---
id: task-542-03-doc
description: "Doc: Note the fix in the effect doc node"
parent: task-542-ground-particle-edges
kind: task
status: done
---
# Ground particles: no visible sprite edges: doc

- goal: Note the fix in the effect doc node
- agent: doc
- depends_on: task-542-02-capture
- inputs: 
- expected_output: see done_when
- done_when: The node of the effect states the fix and its cause; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-542
- usage: included in the executor run of task-542
- runs: 1
- milestone: M2
- started: 2026-11-13T23:30:55Z
- waiting_for: none
- result: Done (5490c561). New node bp-extractor-ground-wind (asset paths, material, instances, textures, the task-542 edge fix and its cause; emitters and BP wiring left open, not read back); entries in blueprints-fx-index, relations from bp-extraction-shuttle and bp-extractor-rotor-wash (which notes NS_ExtractorRotorWash no longer references M_ExtractorBlastWind). Lint clean.

## Relations
- [[task-542-ground-particle-edges]] — parent global task.
