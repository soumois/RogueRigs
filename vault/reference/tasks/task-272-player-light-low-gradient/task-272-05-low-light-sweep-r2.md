---
id: task-272-05-low-light-sweep-r2
description: "Second runtime sweep of the low player light matched to the measured DRGS ring profile: softer core, darker far ambient, DRGS colour."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Low light sweep round 2

- goal: Second runtime sweep of the low player light matched to the measured DRGS ring profile: softer core, darker far ambient, DRGS colour.
- agent: cpp
- depends_on: task-272-01-player-light-low-gradient
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), Saved/LookTune/capture_cmd.txt, Saved/LookTune/r6/macros/
- expected_output: Saved/LookTune/r6b/: variants on frames A, D, G, H with values read back and ring profiles (80 px rings, method of the main session's DRGS analysis) compared with the DRGS target.
- done_when: Frames exist, are described and measured against the target; values read back; no game process left; Orchestrator judged them.
- editor_state: closed
- token_ceiling: 80k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.
