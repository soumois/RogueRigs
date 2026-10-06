---
id: task-026-03-bp-pressure
description: "HORDE EN APPROCHE banner, red damage vignette, event icons on the alert bar, and a light-halo test with a darker arena."
parent: task-026-pressure
kind: task
status: done
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
- model: sonnet
- tokens: input 100, cache write 120565, cache read 3735681, output 41468
- runs: 2
- milestone: M2
- result: M_HUDVignette (red edge gradient); WBP_HUD: DamageVignette (hit flash decay, low-health base below 40 %), HordeBanner shown for SecondsUntil, alert icons console/shuttle/horde0..3 placed at Position × 300, faded when done. Placed spawner MaxAlive 150 and DirectorConfig match C++ defaults. PIE: icons seen in a Slate snapshot; banner and vignette not observed (HUD missing in later snapshots), robot counts not read; no HUD/BP log error seen. Light halo not built (needs C++ bLightHalo cheat). Unchecked items and halo moved into task-026-04. Doc wbp-hud. Commit b0369f1.

## Relations
- [[task-026-pressure]] — parent global task.
