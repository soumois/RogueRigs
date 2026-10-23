---
id: task-299-kamikaze-fuse-shake
description: "Global task: During the 0.65 s fuse the whole kamikaze body shakes lightly like a dug wall, faster and faster; core pulse kept."
parent: tasks-done-index
kind: task
status: done
---
# Kamikaze shakes during its fuse

- request: "tout le body se met à vibrer légèrement comme les murs (la vibration s'accélère pendant les 0.65 s) et en parallèle tu continues l'effet de scale sur le cœur qui explose" (owner, 2026-10-23)
- goal: While Armed, VisualRoot (body and core) jitters with the amplitudes of the existing rock dig shake (8 uu horizontal, 3 uu vertical), its frequency rising over the fuse (default 8 to 24 Hz); core growth and pulse unchanged (ArmedCoreScale as built); actor position, fuse and explosion unchanged (provisional, owner 2026-10-23).
- acceptance: shake only while armed, rising rate, cleared on explosion and pool reuse; kamikaze tests pass (armed drift under 1 uu); -game frames over a fuse; committed.
- gdd_check: enemy-kamikaze and sys-grid-rock-damage read (2026-10-23): fuse shake line added (provisional, owner 2026-10-23). Defaults accepted (provisional, owner 2026-10-23): rock shake amplitudes reused (8 uu horizontal, 3 uu vertical), frequency 8 to 24 Hz; core growth left as built (BP_Kamikaze ArmedCoreScale 1.2 since task-283-11).
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. 14 -game frames at slomo 0.1 over one fuse, 0.05 s apart, plus sheet (Saved/RobotMeshes/kamikaze-shake/kamikaze_sheet.png): body offset against a fixed cross visible between frames (about 21 px travel each screen axis); core bigger at the end but mixed with its pulse. Not verified: uu amplitude, rate rise 8 to 24 Hz, look at normal speed.

## Subtasks
1. task-299-01-cpp-kamikaze-fuse-shake — cpp: Kamikaze fuse shake
2. task-299-02-capture-kamikaze-shake — cpp: Capture the kamikaze fuse

## Relations
- [[tasks-done-index]] — parent index.
- [[task-299-01-cpp-kamikaze-fuse-shake]] — step 01, cpp.
- [[task-299-02-capture-kamikaze-shake]] — step 02, cpp.
