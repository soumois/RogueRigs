---
id: task-085-01-alert-line-once
description: "The company alert line shows once for 8 s at the start of the shuttle countdown, then hides; the countdown number stays to the end."
parent: task-085-alert-line-once
kind: task
status: done
---
# Alert line shown once

- goal: The company alert line shows once for 8 s at the start of the shuttle countdown, then hides; the countdown number stays to the end.
- agent: bp
- depends_on: task-084-01-card-flavor-fix
- inputs: sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), sys-text-content (vault/reference/tech/systems/sys-text-content.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: in WBP_HUD, OnShuttleCountdown re-shows the alert line every countdown frame, so it stays for the whole countdown. Make shuttle_countdown show its line once, when the countdown starts (first call), for 8 s, then HideAlertLine; the "Shuttle: n s" text is untouched. The other alerts keep their 6 s. Group the work, commit after each group, no screenshot calls, update wbp-hud.
- done_when: logic reads back from the graph (once flag, 8 s timer); compile 0 errors; asset saved; wbp-hud updated; lint clean; committed.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: shuttle_countdown line shown once for 8 s (NoteShuttle, once flag reset on run end); other alerts keep 6 s; graph read back, compile 0 errors, saved. Timing not seen on screen. Commit 3b9f72d. Cost counted in the resumed bp agent.

## Relations
- [[task-085-alert-line-once]] — parent global task.
