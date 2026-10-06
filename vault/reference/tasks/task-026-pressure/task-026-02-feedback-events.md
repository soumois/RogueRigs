---
id: task-026-02-feedback-events
description: "HUD events for a red damage vignette scaled by missing health, the horde banner, and upcoming alert-bar events with their positions; tests."
parent: task-026-pressure
kind: task
status: in-progress
---
# Pressure feedback events

- goal: the HUD can show pressure feedback.
- agent: cpp
- depends_on: task-026-01-wave-director
- inputs: [[sys-hud]], [[sys-objectives]], [[sys-player]], task-026-01 result, [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: URRHUDWidget events: OnPlayerDamaged(Amount, HealthFraction) for the vignette, OnHordeIncoming(SecondsUntil), OnAlertEventsChanged(array of {Type, Position 0..1, bDone}) listing console, hordes and extraction on the alert bar; docs sys-hud. Tests.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-026-pressure]] — parent global task.
