---
id: task-220-04-shield-chaser-capture
description: "cpp: -game capture of a shield chaser running with its shield, bashing, and after the shield breaks; frames for the owner."
parent: task-220-shield-chaser-integration
kind: task
status: todo
---
# Shield chaser: in-game capture

- goal: images that show the shield chaser in game: run with the shield, bash, plain run after the break.
- agent: agent-cpp
- depends_on: task-220-03-bp-shield-chaser-wiring
- inputs: `vault/reference/agents/agent-cpp.md` (headless game run, rr.Debug.Shots), `vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shield-chaser.md`, reference shots `vault/assets/chaser-ingame-run.png` and `vault/assets/chaser-ingame-attack.png`.
- method: no C++ change and no build needed (Blueprint and asset changes only); the editor is closed. With the existing debug shot tooling, put shield chasers near the miner (spawn a BP_ShieldChaser close, or let the director bring them after 1:00), capture a run frame, a bash frame and, after taking one past its 30 shield points, a frame without the shield. Save `vault/assets/shield-chaser-ingame-run.png`, `-bash.png`, `-broken.png`.
- done_when: the three images exist and show what their names say (the Orchestrator opens them); no Blueprint error in the log.
- editor_state: closed (the Orchestrator closes the editor it launched)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-220-shield-chaser-integration]] — parent global task.
