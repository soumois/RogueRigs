---
id: task-506-04-gameplay-capture
description: "Capture: Capture the pause menu access and the Gameplay tab."
parent: task-506-options-gameplay-and-pause
kind: task
status: done
---
# Options screen: Gameplay tab and access from the pause menu: capture

- goal: Capture the pause menu access and the Gameplay tab.
- agent: capture
- depends_on: task-506-03-pause-menu-options
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: Frames show: C1 pause menu with Resume, Options, Exit at the four resolutions; C2 Gameplay tab with its toggles at the four resolutions, nothing clipped; C3 hits with damage numbers off show no number; C4 aim indicator off shows no chevrons; sheet sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-506
- usage: included in the executor run of task-506
- runs: 1
- milestone: M2
- started: 2026-11-12T05:08:41Z
- waiting_for: none
- result: done: sheet Saved/Captures/t506_04_sheet.png. C1 seen: Resume, Options, Exit on their plates at 1280x720, 1920x1080, 1920x1200, 2560x1080 (labels inside the beige centres; at 1920x1200 tight but inside). C2 seen: Gameplay tab with its 4 toggles On at the four resolutions, nothing clipped in the rows; the selected Gameplay label slightly overruns the narrow beige centre at 1920x1200 (same kind as the owner-kept selected-tab look, kept). C3 seen: with rr.Debug.Aim 0 on a Heavy, numbers On shows '15' and pushed 3 -> 6; Off shows no number and pushed stays 6 (t506_04_2560x1080 dnon_00 / off_01). C4 seen: the aim indicator is the laser line (node says 'chevrons'): On shows it (t506_04b on_00), Off none (off_00, off_01). Reset of the Gameplay tab restored all On after each run. 6 launches (rule 5): the 2560x1080 Gameplay frame was lost to frame lag (shot rendered after Back), one re-check launch per protocol 5.6. Not seen in a frame: screen shake Off (no still-frame evidence possible). First 1080 frame had missing plates once (shader warm-up, 'Preparing Shaders'); the rerun showed them.

## Relations
- [[task-506-options-gameplay-and-pause]] — parent global task.
