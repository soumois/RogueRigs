---
id: task-340-02-ore-counter-total
description: "Bp: WBP_OreCounter shows carried + secured ore."
parent: task-340-ore-run-carry
kind: task
status: in-progress
---
# Ore kept for the whole run, reset at run end: WBP_OreCounter shows carried + secured ore

- goal: WBP_OreCounter shows the run total, carried + secured ore (Q1): ShowOre gets CarriedOre + SecuredOre from OnOreChanged, and the bind-time read uses GetSpendableOre. Nothing else changes.
- agent: bp
- depends_on: task-340-01-ore-run-reset
- inputs: wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), global task-340 `gdd_check`
- expected_output: WBP_OreCounter compiled (warnings as errors) and saved; a change summary for the doc subtask; one commit.
- done_when: compiled and saved; graph read back: the custom event and the bind-time call pass carried + secured; committed.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T06:18:43Z
- waiting_for: none
- result: open

## Relations
- [[task-340-ore-run-carry]] — parent global task.
