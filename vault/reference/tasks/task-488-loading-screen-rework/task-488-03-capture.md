---
id: task-488-03-capture
description: "Capture: -game capture of the loading screen in both cases at the four resolutions, up to 3 rounds of capture and correction."
parent: task-488-loading-screen-rework
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-488
- usage: included in the executor run of task-488
- runs: 1
- milestone: M2
- started: 2026-11-07T04:35:10Z
- waiting_for: none
- result: 2 rounds of 3. Round 1 (4 launches, Saved/Captures/t488_r2_<res>/): C1 seen at all four resolutions (illustration fills the screen, no text in it); C2 seen (riveted frame bottom left, DROPPING IN, Level N, stage line readable); C3 seen between levels (orange fill in a dark riveted track, no engine bar look); C4 seen on New Run (spinning ore icon in the frame, angle changes between frames, no bar); C5 defect at 2560 x 1080 only: the cover-scaled image cut the shuttle top rotors. Correction: ScaleBox content top-aligned (commit ef55572e). Round 2 (1 launch, Saved/Captures/t488_r3_2560x1080/): shuttle whole, C1-C5 seen; other resolutions unaffected (16:9 has no vertical crop, 16:10 crops sideways). Sheet with the before frame: Saved/Captures/t488_final_sheet.png. The engine dev text 'Preparing Textures' shows top left in some frames (shader warm-up of the dev build, not the widget).

## Relations
- [[task-488-loading-screen-rework]] — parent global task.
