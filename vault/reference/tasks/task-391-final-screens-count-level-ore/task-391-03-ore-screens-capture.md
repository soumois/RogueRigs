---
id: task-391-03-ore-screens-capture
description: "Capture: -game frames: Total Ore on the stats screens and Ore secured on the run-won screen with known ore per level"
parent: task-391-final-screens-count-level-ore
kind: task
status: todo
---
# End screens count the ore of the level just extracted: capture

- goal: Check on frames that the end screens count the ore of the level just extracted.
- agent: capture
- depends_on: task-391-02-run-won-ore-line
- inputs: agent-capture (vault/reference/agents/agent-capture.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), task-391-01-level-ore-in-recap (vault/reference/tasks/task-391-final-screens-count-level-ore/task-391-01-level-ore-in-recap.md) change summary (scenario and expected values), debug commands: rr.Cheat.GrantOre, rr.Run.EndLevel, rr.Run.Stats.Next, rr.Shop.Buy, rr.Run.Continue, rr.Debug.Shots
- expected_output: Frames and crops under Saved/Captures/task-391/, report per point.
- done_when: With the task-391-01 scenario: C1 level 1 stats screen Total Ore 3. C2 level 2 stats screen Total Ore 8 (level 2 ore counted). C3 level 4 stats screen Total Ore 26 (purchase not subtracted). C4 run-won end screen "Ore secured: 26". Every point reported seen, not seen or ambiguous; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-391-final-screens-count-level-ore]] — parent global task.
