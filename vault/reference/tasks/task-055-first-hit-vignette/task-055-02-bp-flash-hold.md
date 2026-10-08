---
id: task-055-02-bp-flash-hold
description: "WBP_HUD vignette flash held at full strength for at least 0.15 s before fading."
parent: task-055-first-hit-vignette
kind: task
status: done
---
# Vignette flash hold

- goal: a long frame never hides the first-hit vignette.
- agent: bp
- depends_on: task-055-01-first-hit-vignette
- inputs: wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md)
- expected_output: WBP_HUD UpdateVignette: after OnPlayerDamaged sets HitFlash, the flash is held at its value for at least 0.15 s (a hold timer variable, editable) before the existing decay starts; the C++ warm opacity floor (VignetteWarmOpacity) stays untouched; docs wbp-hud; PIE: no log error.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 40, cache write 48254, cache read 635660, output 10211
- runs: 1
- milestone: M2
- result: WBP_HUD UpdateVignette: HitHoldTime 0.15 s (editable) holds the flash at full value after a new hit before the 2 per second decay; hold decremented after the flash is applied so a long frame still shows it; a lower new hit during a decay is not seen as new (after healing only). With task-055-01 (warm opacity floor, packaged vignette on the first hit at normal frame rate). PIE: no log error. Commit a10d0f2.

## Relations
- [[task-055-first-hit-vignette]] — parent global task.
