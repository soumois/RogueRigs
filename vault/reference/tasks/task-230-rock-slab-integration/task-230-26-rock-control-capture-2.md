---
id: task-230-26-rock-control-capture-2
description: "Control capture after 22 to 25: broken tops, lavender top, rim band, red heal crystals."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Control capture after 22 to 25: broken tops, lavender top, rim band, red heal crystals

- goal: -game captures at the 230-19 spots and camera values, side by side with Saved/RockSlab/after3/, plain description of each point (owner 2026-10-19, after the 230-19 captures (Saved/RockSlab/after3/)).
- agent: cpp
- depends_on: task-230-22-broken-top-texture, task-230-23-rock-top-lavender, task-230-24-rock-rim-visible, task-230-25-heal-crystal-red
- inputs: vault/reference/tech/performance/perf-rock-slab.md, vault/reference/agents/agent-cpp.md (BugItGo then Walk), vault/reference/tech/systems/core/sys-cheats.md
- expected_output: captures in Saved/RockSlab/after4/ with side by side against after3.
- done_when: captures sent to the owner; no game process left; committed if docs change.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
