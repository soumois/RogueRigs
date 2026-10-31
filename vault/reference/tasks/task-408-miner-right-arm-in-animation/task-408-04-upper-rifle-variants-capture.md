---
id: task-408-04-upper-rifle-variants-capture
description: "Capture: one frozen frame, idle and the three elbow proposals V1..V3 from two close views, labelled sheets for the owner"
parent: task-408-miner-right-arm-in-animation
kind: task
status: in-progress
---
# Miner right arm corrected in the animation, variants compared on one frame: capture

- goal: The owner gets images of one same frame with the unchanged idle and each variant, side by side and labelled with their values.
- agent: capture
- depends_on: task-408-03-import-upper-rifle-variants
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Commands: `rr.Stress.Peace 1`, `rr.Debug.Camera <Pitch> <ArmLength> [Yaw]` (close view of the right arm; the default top-down view hides the right hand, task-401-04), `rr.Debug.MinerAnimFreeze 1`, `rr.Debug.MinerUpperPose <name>` (A_Miner_Idle, A_Miner_UpperRifle_V1..V3), `rr.Debug.Shots`, `rr.Debug.Delayed`; sheets with `Tools/capture/sheet.js`
- expected_output: Frames and sheets in `Saved/Captures/UpperRifle408/` (one sheet per view, labels "idle", "V1 coude +3 cm", "V2 coude +6 cm", "V3 coude +9 cm"), report per point with paths, launches used.
- done_when: Miner standing, weapons stopped, animation frozen once, then A_Miner_Idle and V1..V3 set in turn with rr.Debug.MinerUpperPose and shot from the right side view and the three-quarter front view (8 frames, at most 5 launches), one labelled sheet per view. Checklist: C1 every frame of a view shows the same frame (legs, head, left arm, rifle, background identical apart from the right upper arm and forearm); C2 the right elbow is higher from idle to V3; C3 the right hand and the rifle stay at the same place in every frame; C4 the left hand stays on the rifle handguard; C5 no arm stretched straight, crossed through the body or in T-pose. Each point seen, not seen or ambiguous with a path; ambiguous points and the sheets go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T07:49:48Z
- waiting_for: none
- result: open

## Relations
- [[task-408-miner-right-arm-in-animation]] — parent global task.
