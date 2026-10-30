---
id: task-399-02-total-duskite-capture
description: "Capture: level 1 stats screen at four resolutions: number beside the label, nothing else moved"
parent: task-399-total-duskite-number-overlap
kind: task
status: done
---
# Stats screen: Total Duskite number overlaps its label: capture

- goal: -game windowed captures through capture.js (--force-res for 1920 x 1200 and 2560 x 1080) of the level 1 stats screen with a two-digit ore balance: 1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080; one comparison sheet with the four frames and the look before. Report each point seen, not seen or ambiguous with the frame path.
- agent: capture
- depends_on: task-399-01-total-duskite-row
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), Look before: `Saved/LevelEndScreen/task-399/owner-bug-before.webp` (owner) and `Saved/LevelEndScreen/task-381/L1_1920x1080_v2.png`, Debug commands: `rr.Debug.KillRobots`, `rr.Cheat.GrantOre <N>`, `rr.Run.EndLevel` (opens the stats screen), `rr.Debug.Shots`, `rr.Run.Status`, sys-cheats (vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: frames and comparison sheet under `Saved/LevelEndScreen/task-399/` (not committed); a report per point C1 to C4.
- done_when: C1 the Total Duskite number is fully right of the label "Total Duskite:", with a visible gap, no letter covered, at every resolution; C2 icon, label and number sit on one horizontal line inside the top band of the frame, above the horizontal divider; C3 title, columns, dividers and NEXT LEVEL button look as in the look-before frame; C4 at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 nothing in the band is cut or leaves the frame. Each point seen, not seen or ambiguous with its frame path; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 24, cache write 72725, cache read 258725, output 5084
- usage: turns 11, context start 14k peak 41k, resumes 1 (message x1), 8 min, units 0.14 M
- runs: 1
- milestone: M2
- started: 2026-10-30T23:34:29Z
- waiting_for: none
- result: Closed on owner validation (2026-10-30, 'Ok je valide ce que j'ai vu a l'ecran'). Capture run: C1, C2, C4 seen at 1920x1080, 1280x720, 2560x1080 (Saved/LevelEndScreen/task-399/comparison_sheet.png); 1920x1200 not verified (frame was 1920x1080); C3 ambiguous (lists empty); 4 of 5 launches wasted by Git Bash path rewrite

## Relations
- [[task-399-total-duskite-number-overlap]] — parent global task.
