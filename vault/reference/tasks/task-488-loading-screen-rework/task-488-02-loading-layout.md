---
id: task-488-02-loading-layout
description: "Bp: WBP_Loading rebuilt to the intermission look: image, bottom gradient, riveted frame holding the texts and the restyled bar or spinner."
parent: task-488-loading-screen-rework
kind: task
status: done
---
# Richer loading screen (illustration and framed texts): bp

- goal: WBP_Loading rebuilt to the intermission look: image, bottom gradient, riveted frame holding the texts and the restyled bar or spinner.
- agent: bp
- depends_on: task-488-01-loading-image
- inputs: intermission (vault/reference/gdd/run/intermission.md), wbp-loading (vault/reference/tech/blueprints/ui/wbp-loading.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), wbp-run-lost (vault/reference/tech/blueprints/ui/wbp-run-lost.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), sys-run-transition-fade (vault/reference/tech/systems/world/sys-run/sys-run-transition-fade.md)
- expected_output: see done_when
- done_when: The widget shows the image full screen, the gradient and the riveted frame at bottom left with the three texts, the bar as an orange fill in a dark riveted track and the spinner in the frame when progress is not measured; events, texts lookup, progress, spinner and Collapsed logic unchanged; compiled with warnings as errors and saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-488
- usage: included in the executor run of task-488
- runs: 1
- milestone: M2
- started: 2026-11-07T04:30:21Z
- waiting_for: none
- result: WBP_Loading rebuilt (commit 5a6293ae): CanvasPanel Root; Backdrop kept full screen; BackgroundScale (ScaleBox ScaleToFill, full anchors) holding LoadingImage (the reserved image, now T_LoadingBackground, HitTestInvisible); BottomGradient (T_Loading_BottomGradient, anchors 0,0.5-1,1); Frame (Border, T_LevelEnd_Frame Box 0.0955/0.1878, anchor 0,1 align 0,1 offset 48,-48, auto size, padding 56/44/72/52) holding Column: TitleText 52, LevelText 32, StageText 22 left-aligned, LoadingBarBox (height 45, fill width) > BarTrack Overlay (LoadingBar with flat dark background and flat orange fill, no engine images, padding 13/10; BarFrame T_HUD_HealthStrip Box), LoadingAnimImage 128 centred. Variable names, graph, events, text lookup, progress and spinner logic untouched. CompileWidgetBlueprint true, compile_blueprint with warnings as errors gave no log warning, saved. Self-check -game 1920 x 1080 both cases (Saved/Captures/t488_r1_1080/bl_00.png, frame_crop.png): image full screen, frame bottom left, bar and spinner in the frame.

## Relations
- [[task-488-loading-screen-rework]] — parent global task.
