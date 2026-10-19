---
id: task-230-26-rock-control-capture-2
description: "Control capture after 22 to 25: broken tops, lavender top, rim band, red heal crystals."
parent: task-230-rock-slab-integration
kind: task
status: done
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
- model: opus
- tokens: input 46, cache write 45660, cache read 617566, output 4165
- runs: 1
- milestone: M2
- result: Commit 06ca2903 (exact capture command lines in perf-rock-slab). Captures Saved/RockSlab/after4/ against after3: heal crystals now red-pink (fixed); broken tops textured at spot D but the 0.7 cells of the sag wall still flat lavender; top colour almost unchanged (RGB 189,182,197 -> 185,177,195) despite TopBrightness 1.12 -> 0.85; no rim band visible, a thin light line appears at the wall foot instead. Parameter changes estimated without seeing barely show: method to change.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
