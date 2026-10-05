---
id: task-026-03-bp-pressure
description: "HORDE EN APPROCHE banner, red damage vignette, event icons on the alert bar, and a light-halo test with a darker arena."
parent: task-026-pressure
kind: task
status: todo
---
# Pressure visuals

- goal: the pressure is visible.
- agent: bp
- depends_on: task-026-02-feedback-events
- inputs: [[wbp-hud]], [[visual-style]], task-026-02 result, [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: WBP_HUD: centered banner "HORDE EN APPROCHE !" for 4 s; full-screen red vignette on hit, stronger when health is low; icons on the alert bar (console, horde, shuttle); light halo test: a point/spot light on the miner and darker map lighting, behind a cheat or a setting so the owner can compare (default off unless it reads well); docs wbp-hud, map or lighting doc. PIE check.
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-026-pressure]] — parent global task.
