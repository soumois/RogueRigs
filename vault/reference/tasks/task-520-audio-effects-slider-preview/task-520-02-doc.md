---
id: task-520-02-doc
description: "Doc: sys-audio / sys-options: the Effects preview"
parent: task-520-audio-effects-slider-preview
kind: task
status: done
---
# Curseur Effects : jouer un son d'effet: doc

- goal: sys-audio / sys-options: the Effects preview
- agent: doc
- depends_on: task-520-01-preview-sound
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), sys-options (vault/reference/tech/systems/ui/sys-options.md)
- expected_output: see done_when
- done_when: A3 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-520
- usage: included in the executor run of task-520
- runs: 1
- milestone: M2
- started: 2026-11-13T13:24:53Z
- waiting_for: none
- result: Commit 8b6ec60d. sys-options: Effects preview flow (UpdateEffectsPreview, step taken on open, throttle, deferred last step, Master and Music silent), defaults EffectsPreviewEvent / EffectsPreviewInterval, rr.Options.Slide, measured check (stale 'no sound asset' gotcha replaced). sys-audio: EffectsPreview call site and row. Back-links on the global. gdd lines of audio-direction and options-screen named in gdd_check not edited: outside the doc subtask inputs. Lint clean.

## Relations
- [[task-520-audio-effects-slider-preview]] — parent global task.
