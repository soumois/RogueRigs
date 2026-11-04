---
id: task-427-02-clear-zone-capture
description: "Capture: check the clear zone, the task-426 corner value and (Q17) the task-425 light"
parent: task-427-darkener-clear-zone
kind: task
status: todo
---
# Darkener shader: clear zone around the screen centre with a transition radius: capture

- goal: One -game run on L_ArenaM1 at 1920x1080: frames with Enable 1 and Enable 0 at the same spot, the same pair with the tonemapper off (ShowFlag.Tonemapper 0), one after the miner has moved; if Q17 is yes, frames with FloorWallLight on and hidden or intensity 0 at the same spot with robots near the miner. Then check the list.
- agent: capture
- depends_on: task-427-01-clear-zone-material
- inputs: agent-capture (vault/reference/agents/agent-capture.md), vault/reference/tasks/task-427-darkener-clear-zone.md (acceptance, Questions), task-425-03-light-capture (vault/reference/tasks/task-425-miner-floor-wall-light/task-425-03-light-capture.md) (its C1 to C5 are L1 to L5 here), look-before frames: Saved/Captures/task426-02b/
- expected_output: Report per point seen / not seen / ambiguous with frame or crop paths in Saved/Captures/task427-02/.
- done_when: C1 pixels within about 165 px of the screen centre equal between Enable 1 and Enable 0. C2 between about 165 and 385 px the darkening grows smoothly, no ring or hard edge. C3 beyond about 385 px the darkening matches the task-426 frames (circles around the screen centre). C4 the pattern stays fixed on screen when the miner moves. C5 corner ratio Enable 1 / Enable 0: about 0.15 with the tonemapper off, and the tonemapped ratio reported against the expected on-screen value (about 0.4); say whether this explains the task-426 0.27 to 0.60. C6 void pixels at most halved, if void is in view. If Q17 is yes: L1 to L5 = C1 to C5 of task-425-03-light-capture. Each point reported; ambiguous points go to the owner.
- editor_state: Unreal editor closed (-game run).
- token_ceiling: 0.5 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-427-darkener-clear-zone]] — parent global task.
