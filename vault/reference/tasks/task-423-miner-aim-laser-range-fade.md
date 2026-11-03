---
id: task-423-miner-aim-laser-range-fade
description: "Global task: give the miner aim laser a visible maximum range and a fade-out at its end, at open floor, rock block or robot; before/after captures."
parent: tasks-done-index
kind: task
status: done
---
# Miner aim laser: visible maximum range and fade-out at its end

- request: Owner, 2026-11-03, verbatim (English, relayed by the main session): "Miner aim laser: add a maximum range and a fade-out. The miner's aim laser already exists. I want it to have a maximum range and to fade out at its end until it disappears. If it hits something before the maximum range, it should fade out there too. I want before/after screenshots shown in the conversation (not stored in the vault), for these cases: 1. Aiming at open ground with nothing in the way: the laser reaches its maximum range and fades out. 2. Aiming at a rock block closer than the maximum range: the laser fades out at the block. 3. Aiming at a robot closer than the maximum range: the laser fades out at the robot." Main session context: captures are wanted, shown in the conversation by the main session (SendUserFile), never committed nor stored in the vault: frames go to Saved/Scratch/ and are temporary; before = current behaviour, taken before the change; after = once implemented. Separate from the deleted task-422 (shooter laser), not resurrected. Owner answer 2026-11-03 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5 accepted.
- goal: The miner aim laser visibly ends at its maximum range on open floor, and fades smoothly to nothing over its last stretch at every end (maximum range, first rock block, first robot); before/after frames of the three owner cases are shown to the owner. Values per the Questions.
- acceptance: Before frames of cases 1 to 3 taken on the current build before any laser change; C++ builds; on open floor the laser ends at MaxLength (600 uu by default) and its last 150 uu fade to zero; aimed at a rock block 350 uu away it ends at the block face and fades out there, never beyond; aimed at a living robot 350 uu away it ends at the robot and fades out there; a line shorter than 300 uu fades over its last half; colour, core, glow, Muzzle origin, height, hiding and the stop rule unchanged; M_AimLaser asset and BP_Miner not edited; `rr.Debug.Aim` fixes the miner aim for visual checks without gameplay change (Q5); existing tests pass, no new test; after frames taken with the same setup as the before frames; before/after pairs of the three cases in Saved/Scratch/task-423/ (temporary, not committed), paths reported for the main session to show; doc nodes updated; gdd aim-indicator updated (edits on confirmation); lint clean; committed by path.
- gdd_check: Read 2026-11-03: [[aim-indicator]] (provisional, M3) already specifies both points (task-415, owner 2026-11-02): length = nearer of the first robot and the first block, at most 1200 uu; look: the last 15 % of the length fades to zero at the tip; the laser is never red; weapon-agnostic; light mask: no compensation, the far part fades with the screen darkening (owner 2026-10-27, "Laisser s'éteindre"). [[sys-aim-indicator]] and [[bp-aim-laser-material]]: implemented (commits 2b0f9193, 33158869, 1700f6ff): MaxLength 1200, GetClearDistance and robot capsule sweep stop, TipFadeFrac 0.15 pushed once at BeginPlay. CONFLICT (Q1): the request restates the existing spec; the task-415 capture (task-415-04) was closed on the owner validation, its point C4 (range and tip fade) never checked on a frame. Likely cause the owner sees neither: [[mpp-player-light-mask]] (Radius 200 uu, Exponent 1.8, FarBrightness 0.06) leaves about 37 % brightness at 300 uu, 17 % at 600 uu and 10 % at 1200 uu, so the laser dims into darkness with no visible end; and 15 % of a 350 uu line is about 50 uu. Open values the work needs: maximum range (Q2), fade length (Q3), light mask treatment (Q4). Capture setup: no console command fixes the aim in a -game run (mouse aim follows the cursor; commands.js lists rr.Debug.Camera, rr.Debug.SpawnNear, BugItGo, none for aim): Q5. Milestone: aim feedback is M3 (juice), done now on the owner request as task-415 was. Captures are requested by the owner (not unrequested work). No representative slice: one effect, values only, no expansion.
- confirmed_by_owner: yes (2026-11-03, "ok")
- milestone: M3
- next_step: none
- started: 2026-11-03T02:02:14Z
- waiting_for: none
- result: Done. Doc nodes updated (commit d2119732): sys-aim-indicator, bp-aim-laser-material, tool-capture; gdd aim-indicator already carried the values (e572c60c)

## Questions
Decided 2026-11-03 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-03.
- Q1 Already in the GDD: the aim-indicator node and the code (task-415, 2026-11-02) already give the laser a 1200 uu maximum, a stop at the first rock block or robot, and a fade over the last 15 % of its length at every end; it was never checked on a frame (you validated before the capture ran). Most likely you do not see it because the screen darkening around the miner leaves the laser at about 10 % brightness at 1200 uu, and 15 % of a short line is a few dozen uu. Default: make range and fade clearly visible with the values of Q2 to Q4; the before frames are taken first. Or: before frames only, then you decide (no change planned now).
- Q2 Maximum range: Default: 600 uu (about 9 cells; the darkening leaves about 17 % brightness there, against 10 % at 1200 uu). Or: keep 1200 uu, or another value (400 uu leaves about 27 %).
- Q3 Fade-out: Default: a fixed fade over the last 150 uu (about 2 cells) of the line, whatever ends it (maximum range, rock block, robot); a line shorter than 300 uu fades over its last half. Or: keep a fraction of the length (15 % today, or another percent), or another fixed length.
- Q4 Screen darkening: Default: unchanged, the laser keeps darkening with the screen around the miner (your choice 2026-10-27, "Laisser s'éteindre"). Or: the laser stays at full brightness through the darkening (technical feasibility to be checked by the cpp agent; may change how it sits over muzzle flashes).
- Q5 Capture setup: in a -game capture nothing can point the aim (it follows the mouse cursor). Default: add a debug console command `rr.Debug.Aim <YawDegrees>` that fixes the miner aim for visual checks only (no gameplay change), built before the before frames. Or: the capture agent uses the aim as it comes at start, risking frames that do not show the three cases.

## Subtasks
1. task-423-01-debug-aim-command (vault/reference/tasks/task-423-miner-aim-laser-range-fade/task-423-01-debug-aim-command.md) — cpp: add the rr.Debug.Aim <YawDegrees> console command that fixes the miner aim for visual checks
2. task-423-02-capture-before (vault/reference/tasks/task-423-miner-aim-laser-range-fade/task-423-02-capture-before.md) — capture: before frames of the current aim laser for the three owner cases
3. task-423-03-aim-laser-range-fade (vault/reference/tasks/task-423-miner-aim-laser-range-fade/task-423-03-aim-laser-range-fade.md) — cpp: MaxLength 600 uu and a fixed 150 uu tip fade at every end of the aim laser
4. task-423-04-capture-after (vault/reference/tasks/task-423-miner-aim-laser-range-fade/task-423-04-capture-after.md) — capture: after frames of the three owner cases, same setup, before/after pairs
5. task-423-05-doc (vault/reference/tasks/task-423-miner-aim-laser-range-fade/task-423-05-doc.md) — doc: update sys-aim-indicator, bp-aim-laser-material and tool-capture for the range, fade and rr.Debug.Aim

## Relations
- [[tasks-done-index]] — parent index.
- [[task-423-01-debug-aim-command]] — step 1, cpp.
- [[task-423-02-capture-before]] — step 2, capture.
- [[task-423-03-aim-laser-range-fade]] — step 3, cpp.
- [[task-423-04-capture-after]] — step 4, capture.
- [[task-423-05-doc]] — step 5, doc.
- [[aim-indicator]] — gdd node updated on confirmation (range, fade).
- [[sys-aim-indicator]] — the component that draws the laser.
- [[bp-aim-laser-material]] — the laser material and its TipFadeFrac parameter.
- [[task-415-aim-laser]] — the task that created the laser with a 1200 uu cap and 15 % tip fade.
