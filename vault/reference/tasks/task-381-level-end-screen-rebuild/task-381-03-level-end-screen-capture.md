---
id: task-381-03-level-end-screen-capture
description: "Capture: -game check of the rebuilt stats screen at four resolutions and the level 4 variant against the mockup."
parent: task-381-level-end-screen-rebuild
kind: task
status: in-progress
---
# Level-end screen rebuild: capture

- goal: -game windowed captures through capture.js (--force-res for 1920 x 1200 and 2560 x 1080): level 1 stats screen with ore and destroyed robots at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080; the level 4 variant at 1920 x 1080; one comparison sheet with the four resolutions, the look before and the mockup. Report each point seen, not seen or ambiguous with the frame path.
- agent: capture
- depends_on: task-381-02-intermission-widget-rebuild
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), Look before: `Saved/StatsCards/sc_a_stats_l1_00.png` (level 1) and `Saved/StatsCards/sc_c_stats_l4_00.png` (level 4), task-317-05, Reference: `Art/Source/level-end-screen/reference/chatgpt-level-end-mockup-2026-10-30.webp`, Debug commands: `rr.Debug.KillRobots`, `rr.Cheat.GrantOre <N>`, `rr.Run.EndLevel` (opens the stats screen), `rr.Run.NextLevel` (to reach level 4), `rr.Debug.Shots`, `rr.Run.Status` (details in sys-cheats, vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: frames and comparison sheet under `Saved/LevelEndScreen/task-381/` (not committed); a report per point C1 to C8.
- done_when: C1 the title is drawn between two riveted title bars, the right one mirrored, and the subtitle sits under it between two thin lines; C2 the riveted frame is drawn with undistorted teal corners at every resolution; C3 the top band of the frame shows the ore icon and two ore lines with numbers; C4 a bolted horizontal divider separates the band from the columns and a bolted vertical divider separates the two columns, ends not stretched; C5 each column shows its label with a thin line under it and at least one entry with the name left and the value right-aligned; C6 the button shows the cream Continue plate with NEXT LEVEL in upper case; C7 the dimmed arena is visible behind, and at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 nothing is cut, overlaps or leaves the screen; C8 the level 4 frame shows CLAIM EMPTIED and CASH OUT in the same layout. Each point seen, not seen or ambiguous with its frame path; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T04:40:19Z
- waiting_for: none
- result: open

## Relations
- [[task-381-level-end-screen-rebuild]] — parent global task.
