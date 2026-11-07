---
id: task-488-03-capture
description: "Capture: -game capture of the loading screen in both cases at the four resolutions, up to 3 rounds of capture and correction."
parent: task-488-loading-screen-rework
kind: task
status: todo
---
# Richer loading screen (illustration and framed texts): capture

- goal: -game capture of the loading screen in both cases at the four resolutions, up to 3 rounds of capture and correction.
- agent: capture
- depends_on: task-488-02-loading-layout
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), intermission (vault/reference/gdd/run/intermission.md), Saved/PreloadNextLevel/t318_loading_00.png
- expected_output: see done_when
- done_when: Checklist, each seen on a frame at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 (or closed on the owner "je valide"): C1 the illustration fills the screen, no text in it; C2 riveted frame bottom left with DROPPING IN, Level N and stage line readable; C3 between levels the bar is an orange fill in a dark riveted track, no engine bar look; C4 on New Run the spinning ore icon is in the frame and no bar shows; C5 nothing overlaps or is clipped. At most 3 rounds; sheet with the before image sent to the owner.
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
- [[task-488-loading-screen-rework]] — parent global task.
