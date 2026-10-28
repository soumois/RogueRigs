---
id: task-339-05-heal-chunk-capture
description: "cpp: run the two existing BP_PickupHeal tests, then a -game capture of heal chunks resting on the floor after a heal vein breaks."
parent: task-339-heal-chunk-mesh
kind: task
status: todo
---
# Heal pickup chunks: tests and in-game capture

- goal: the change is on screen, so the task ends with a `-game` capture ([[orchestrator-lessons]], showing and verifying). (1) Run the existing tests that spawn BP_PickupHeal, `RogueRigs.Pickups.HealFullHealthWaits` and `RogueRigs.Pickups.HealPartialClamp` ("settled near its rest point, on the floor"); if the new mesh breaks one, fix only its expectation, no new test. (2) One capture through `Tools/capture/capture.js`: a heal vein broken in the M1 arena with the miner at full health (a full-health miner refuses heal pickups, so they stay on the floor), game camera, several heal chunks at rest; then a close shot of the chunks. Describe colour (pure red, no orange), size against the miner and the ore chunk, how they lie (on a face, not on a tip), and whether the three variants and random orientations show. Compare with the red cube look described in [[bp-pickup-heal]] (a before capture with heal cubes in `Saved/Captures/` if one exists). capture_rules of [[agent-cpp]] apply: at most 5 launches; a defect seen on the first sheet is reported, not searched.
- agent: cpp
- depends_on: task-339-04-heal-pickup-blueprint
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: test results; `Saved/Captures/HealChunk/` with `contact_sheet.png`; a short description of what the frames show; change summary if a test expectation was fixed.
- done_when: both tests pass (or the fixed expectation is explained); the contact sheet exists and is described; the Executor sends it to the owner with SendUserFile; committed by path if a test file changed.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-339-heal-chunk-mesh]] — parent global task.
