---
id: task-520-01-preview-sound
description: "Cpp: Effects slider plays the sample effect per Q1"
parent: task-520-audio-effects-slider-preview
kind: task
status: done
---
# Curseur Effects : jouer un son d'effet: cpp

- goal: Effects slider plays the sample effect per Q1
- agent: cpp
- depends_on: none
- inputs: sys-options (vault/reference/tech/systems/ui/sys-options.md), sys-audio (vault/reference/tech/systems/core/sys-audio.md), options-screen (vault/reference/gdd/run/options-screen.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1 to A3 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-520
- usage: included in the executor run of task-520
- runs: 1
- milestone: M2
- started: 2026-11-13T13:17:35Z
- waiting_for: none
- result: Commit 9ba8f051. URROptionsWidget::UpdateEffectsPreview plays URROptionsSettings::EffectsPreviewEvent (row EffectsPreview: SW_Shot, Volume 0.5, centred, bUI, MaxConcurrent 2) when the Effects volume reaches a new 5 % step, at most one per EffectsPreviewInterval 0.15 s (real time), a step reached inside the interval plays from NativeTick; debug rr.Options.Slide. Build green. A1: -game runs (arena paused, main menu) with rr.Options.Slide: 100->50 % in 1 s = 7 plays 0.15 s apart, 50->52 % = none, final step played after the drag, plays paused, class SC_Effects. A2: Master and Music slides play nothing. A3: no audio automation test exists (none to run); build only.

## Relations
- [[task-520-audio-effects-slider-preview]] — parent global task.
