---
id: task-520-audio-effects-slider-preview
description: "Global task: Moving the Effects volume slider plays a sample effect sound so the player hears what it sets."
parent: tasks-done-index
kind: task
status: done
---
# Curseur Effects : jouer un son d'effet

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « Lorsqu'on bouge l'effects volume, on doit entendre un bruit d'effect pour savoir à quoi ça correspond ». Owner answer 2026-11-13 (relayed by the main session, verbatim): "tout par défaut" = the plan and the defaults of Q1 accepted.
- goal: Moving the Effects slider of the Options screen plays a sample effect at the new level (Q1).
- acceptance: A1 in a -game run with Verbose LogRRAudio, dragging the Effects slider logs the sample sound per Q1 (one per 5 % step, throttled, through the Effects class, also on the paused Options screen); A2 Master and Music sliders play nothing new; A3 build green, existing audio tests pass; sys-audio or sys-options documents it.
- gdd_check: Read options-screen, audio-direction, sys-audio. No conflict. Open: which sound and how often (Q1). Edit: audio-direction and options-screen lines. No code read.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-13T13:17:35Z
- waiting_for: none
- executor_tokens: input 92, cache write 120711, cache read 3632915, output 25267
- executor_usage: runs 1, turns 46, context start 16k peak 121k, resumes 0, 8 min, units 0.64 M, USD 1.84
- planning_tokens: input 6, cache write 18666, cache read 174110, output 3853
- planning_usage: a163c449 "Plan owner feedback batch" 1/9 (tasks 520, 521, 522, 523, 524, 525, 526, 527, 528), units 0.06 M, USD 0.21
- result: Done. Commit 8b6ec60d. sys-options: Effects preview flow (UpdateEffectsPreview, step taken on open, throttle, deferred last step, Master and Music silent), defaults EffectsPreviewEvent / EffectsPreviewInterval, rr.Options.Slide, measured check (stale 'no sound asset' gotcha replaced). sys-audio: EffectsPreview call site and row. Back-links on the global. gdd lines of audio-direction and options-screen named in gdd_check not edited: outside the doc subtask inputs. Lint clean.

## Questions
Decided 2026-11-13 (owner "tout par défaut"): every default below is accepted; values provisional, owner 2026-11-13.
- Q1 Son joué en bougeant le curseur Effects : le son du tir du joueur, centré, à chaque cran de 5 %, au plus un toutes les 0,15 s, au nouveau volume, aussi quand le jeu est en pause ; Master et Music ne jouent rien. Default: oui.

## Subtasks
1. task-520-01-preview-sound (vault/reference/tasks/task-520-audio-effects-slider-preview/task-520-01-preview-sound.md) — cpp: Effects slider plays the sample effect per Q1
2. task-520-02-doc (vault/reference/tasks/task-520-audio-effects-slider-preview/task-520-02-doc.md) — doc: sys-audio / sys-options: the Effects preview

## Relations
- [[tasks-done-index]] — parent index.
- [[task-520-01-preview-sound]] — step 1, cpp.
- [[task-520-02-doc]] — step 2, doc.
- [[sys-options]] — documents the Effects preview rules.
- [[sys-audio]] — documents the EffectsPreview row.
