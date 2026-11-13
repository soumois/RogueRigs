---
id: task-529-01-doc-check
description: "Doc: audio-direction and options-screen checked against the sys nodes after the confirmation edits"
parent: task-529-gdd-audio-options-sync
kind: task
status: done
---
# GDD audio-direction et options-screen alignés sur ce qui est construit: doc

- goal: audio-direction and options-screen checked against the sys nodes after the confirmation edits
- agent: doc
- depends_on: none
- inputs: audio-direction (vault/reference/gdd/art-audio/audio-direction.md), options-screen (vault/reference/gdd/run/options-screen.md), sys-options (vault/reference/tech/systems/ui/sys-options.md), sys-audio (vault/reference/tech/systems/core/sys-audio.md), sys-audio-distance-sounds (vault/reference/tech/systems/core/sys-audio-distance-sounds.md), task-520-audio-effects-slider-preview (vault/reference/tasks/task-520-audio-effects-slider-preview.md), task-521-options-tab-button-shrinks (vault/reference/tasks/task-521-options-tab-button-shrinks.md), task-522-audio-distance-sound-rotors (vault/reference/tasks/task-522-audio-distance-sound-rotors.md), task-523-audio-impact-near-quieter (vault/reference/tasks/task-523-audio-impact-near-quieter.md), task-527-audio-zone-objective-complete (vault/reference/tasks/task-527-audio-zone-objective-complete.md)
- expected_output: see done_when
- done_when: A1 to A4 of the global hold on the two gdd nodes as committed at confirmation; every value of the lines of task-520, 522, 523, 527 equals its sys node; any mismatch or stale sentence left is reported in the result with the node and line, not fixed in a gdd node; no game run, no capture; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-529
- usage: included in the executor run of task-529
- runs: 1
- milestone: M2
- started: 2026-11-13T17:35:15Z
- waiting_for: none
- result: A1-A4 hold. Values equal to sys nodes: task-520 (SW_Shot row EffectsPreview, centred, VolumeStep 0.05, interval 0.15 s, bUI, Master/Music silent = sys-options, sys-audio), task-522 (FRRDistanceSound base, FRRExplosionSound from radius edge, ExtractorRotors loop FadeInner 800 FadeOuter 2150 = sys-audio-distance-sounds), task-523 (Impact NearVolume 0.6 to 150 uu, 1.0 at 900 uu, Impact.Wall unchanged = sys-audio near_reduction), task-527 (ObjectiveComplete one-shot at zone centre, console and chest, 200/1075 uu = sys-audio, sys-audio-distance-sounds). A2: description no longer per alert level, Sources and Placeholders updated. A3: options-screen no 'no sound' line, Effects preview refers to audio-direction, tab size rule present. No mismatch with a sys node. Residual stale text reported, not fixed (gdd): audio-direction line Music still opens 'tracks per alert level' before the task-519 qualifier; line Sound effects still says 'found and scored by the sound agent' while kept sounds are owner ElevenLabs files; Relations level-flow 'the alert bar that drives music' while one in-game track. No game run, no capture; lint clean (no warning on these nodes).

## Relations
- [[task-529-gdd-audio-options-sync]] — parent global task.
