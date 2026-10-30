---
id: task-396-02-end-ore-text-default
description: "Bp: WBP_AlertObjectives: EndOreText designer default "Ore secured: 0" becomes "Duskite secured: 0""
parent: task-396-duskite-leftover-labels
kind: task
status: todo
---
# Duskite: leftover on-screen "ore" labels: bp

- goal: In WBP_AlertObjectives set the design-time Text of EndOreText from "Ore secured: 0" to "Duskite secured: 0"; the runtime graph (label "Duskite secured: " + GetSecuredOre) and everything else unchanged.
- agent: bp
- depends_on: task-396-01-shop-short-label
- inputs: agent-bp (vault/reference/agents/agent-bp.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), global request and gdd_check: vault/reference/tasks/task-396-duskite-leftover-labels.md
- expected_output: WBP_AlertObjectives saved, compiled with warnings as errors; commit `task-396-02-end-ore-text-default: ...`; change summary.
- done_when: EndOreText Text read back "Duskite secured: 0", ShowRunEnd graph unchanged; no "Ore secured" string left in the package; compiled with warnings as errors, no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-396-duskite-leftover-labels]] — parent global task.
