---
id: task-340-03-ore-carry-capture
description: "Cpp: -game capture: level 2 HUD and next shop show the carried total, new run at 0."
parent: task-340-ore-run-carry
kind: task
status: in-progress
---
# Ore kept for the whole run, reset at run end: -game capture: level 2 HUD and next shop show the carried total, new run at 0

- goal: -game capture: grant ore on level 1, extract (rr.Run.EndLevel), show the level 2 HUD counter with the total, the next shop balance, then a restart at 0; contact sheet sent to the owner, compared with the old look (counter at 0 on level 2).
- agent: cpp
- depends_on: task-340-02-ore-counter-total
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run (vault/reference/tech/systems/world/sys-run.md), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md)
- expected_output: frames and contact sheet in `Saved/OreRunCarry/` (not committed), the observed values in the report.
- done_when: contact sheet exists; the level 2 counter shows the level-1 secured ore; the restart shows 0; what was not seen is listed.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T06:22:52Z
- waiting_for: none
- result: open

## Relations
- [[task-340-ore-run-carry]] — parent global task.
