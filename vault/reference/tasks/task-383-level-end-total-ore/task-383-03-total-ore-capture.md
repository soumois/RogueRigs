---
id: task-383-03-total-ore-capture
description: "Capture: -game check of the Total Ore band and the taller columns at four resolutions and on level 4."
parent: task-383-level-end-total-ore
kind: task
status: in-progress
---
# Stats screen Total Ore: capture

- goal: -game windowed captures through capture.js (--force-res for 1920 x 1200 and 2560 x 1080): level 1 stats screen with ore granted and robots destroyed at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080; level 4 at 1920 x 1080; one comparison sheet with the look before (task-381 frames) and the mockup. Report each point seen, not seen or ambiguous with the frame path.
- agent: capture
- depends_on: task-383-02-total-ore-widget
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), Look before: `Saved/LevelEndScreen/task-381/L1_1920x1080_v2.png`, `L1_1280x720_v2.png`, `L1_1920x1200.png`, `L1_2560x1080.png`, `L4_1920x1080_v2.png` (task-381-03), Reference: `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp`, Debug commands and procedure: task-381-03-level-end-screen-capture (`rr.Debug.KillRobots`, `rr.Cheat.GrantOre <N>`, `rr.Run.EndLevel`, `rr.Run.NextLevel`, `rr.Debug.Shots`, `rr.Run.Status`)
- expected_output: frames and comparison sheet under `Saved/LevelEndScreen/task-383/` (not committed); a report per point C1 to C6.
- done_when: C1 the ore band shows the ore icon, "Total Ore:" and one number, and no "Ore secured" text; C2 the number equals the run secured ore logged by `rr.Run.Status` or the stats screen log line ("run N"); C3 the band is visibly thinner than in the look before and the horizontal divider sits higher; C4 the column labels, their thin lines and the lists start higher than in the look before, list bottoms at the same place; C5 at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 nothing is cut, overlaps or leaves the screen, frame corners undistorted; C6 on level 4 the band shows Total Ore with the same layout and the button reads CASH OUT. Each point seen, not seen or ambiguous with its frame path; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T05:23:47Z
- waiting_for: none
- result: open

## Relations
- [[task-383-level-end-total-ore]] — parent global task.
