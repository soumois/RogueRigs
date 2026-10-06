---
id: task-026-02-feedback-events
description: "HUD events for a red damage vignette scaled by missing health, the horde banner, and upcoming alert-bar events with their positions; tests."
parent: task-026-pressure
kind: task
status: done
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
- model: opus
- tokens: input 104, cache write 131082, cache read 4275913, output 50134
- runs: 1
- milestone: M2
- result: URRHUDWidget BlueprintNativeEvents (HUD|Pressure): OnPlayerDamaged(Amount, HealthFraction), OnHordeIncoming(SecondsUntil), OnAlertEventsChanged(TArray<FRRAlertBarEvent> Type/Position/bDone/Index), GetAlertBarEvents; player controller binds the first spawner. Horde icon position = alert + seconds to front / base fill time, clamped, frozen when done. 3 HUD.Pressure* tests; build 0, 166/166. Docs sys-hud-pressure (new), sys-hud, sys-automation-tests. Commits a07ded9, 1d79928.

## Relations
- [[task-026-pressure]] — parent global task.
