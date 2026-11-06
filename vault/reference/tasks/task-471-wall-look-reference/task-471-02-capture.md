---
id: task-471-02-capture
description: "Capture: Final independent check of the walls against image 2."
parent: task-471-wall-look-reference
kind: task
status: todo
---
# Walls look like the owner in-game reference: capture

- goal: Final independent check of the walls against image 2.
- agent: capture
- depends_on: task-471-01-wall-match-loop
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 lilac-violet wall tops lighter than the floor; C2 thin glowing orange rim along the wall top edges, also far from the miner; C3 dark stratified sides; C4 ore crystals and heal ore keep their colours; C5 a damaged wall still shows its cracks or tilted plates; C6 the rim does not read as a barrel; side-by-side sheets before / after / image 2 sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-471-wall-look-reference]] — parent global task.
