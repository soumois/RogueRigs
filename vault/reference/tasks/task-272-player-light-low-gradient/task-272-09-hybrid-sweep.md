---
id: task-272-09-hybrid-sweep
description: "Runtime sweep of the hybrid look: faint shadow-casting player light plus the radial light mask, measured against r7 and DRGS."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Hybrid runtime sweep

- goal: Runtime sweep of the hybrid look: faint shadow-casting player light plus the radial light mask, measured against r7 and DRGS.
- agent: cpp
- depends_on: task-272-08-light-mask-ppv
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-player (vault/reference/tech/systems/core/sys-player.md), Saved/LookTune/capture_cmd.txt
- expected_output: Saved/LookTune/r8/: hybrid variants (faint shadowed light + radial mask) on A, D, G, H, ring profiles, cost, comparison with r7 and the DRGS frames.
- done_when: Frames exist, described and measured; values read back; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
