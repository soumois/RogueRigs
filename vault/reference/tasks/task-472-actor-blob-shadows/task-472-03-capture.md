---
id: task-472-03-capture
description: "Capture: Check the shadows in game against the seven owner points."
parent: task-472-actor-blob-shadows
kind: task
status: in-progress
---
# Light-independent purple-grey shadows under actors: capture

- goal: Check the shadows in game against the seven owner points.
- agent: capture
- depends_on: task-472-02-shadow-material-wiring
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 each listed actor has one shadow directly beneath it; C2 the shadow stays under a moving robot and the drone on consecutive frames; C3 same shadow look inside and outside the miner light pool; C4 purple-grey, not black; C5 soft edge, silhouette readable, not too transparent or blurred; C6 no second (light) shadow under these actors, walls still shadowed; sheet before / after sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T18:18:21Z
- waiting_for: none
- result: open

## Relations
- [[task-472-actor-blob-shadows]] — parent global task.
