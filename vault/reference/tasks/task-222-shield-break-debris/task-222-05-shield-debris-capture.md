---
id: task-222-05-shield-debris-capture
description: "cpp: -game capture of a shield chaser's shield breaking into fragments, frames for the owner."
parent: task-222-shield-break-debris
kind: task
status: todo
---
# Shield debris: in-game capture

- goal: an image of the shield fragments flying and one of them on the floor.
- agent: agent-cpp
- depends_on: task-222-04-bp-shield-chaser-debris
- inputs: `vault/reference/agents/agent-cpp.md` (headless game run, rr.Debug.Shots), `vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shield-chaser.md`, the capture method of task-220-04 (`vault/reference/tasks/task-220-shield-chaser-integration/task-220-04-shield-chaser-capture.md`).
- method: no C++ change and no build; editor closed. Break a shield chaser's shield near the miner, capture within 0.3 s and at about 2 s. Save `vault/assets/shield-chaser-debris-burst.png` and `-floor.png`. Can run in the same game session as task-221-04.
- done_when: the images show the fragments (the Orchestrator opens them); no Blueprint or Niagara error in the log.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-222-shield-break-debris]] — parent global task.
