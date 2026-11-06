---
id: task-453-02-alert-bar
description: "Bp: The alert bar widget takes the retained look, marks driven as before."
parent: task-453-alert-bar-redesign
kind: task
status: done
---
# Alert bar redesigned and better integrated: bp

- goal: The alert bar widget takes the retained look, marks driven as before.
- agent: bp
- depends_on: task-453-01-art
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), sys-hud-pressure (vault/reference/tech/systems/ui/sys-hud-pressure.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved; marks still placed from the event list; PIE shows no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-453
- usage: included in the executor run of task-453
- runs: 1
- milestone: M2
- started: 2026-11-06T00:47:15Z
- waiting_for: none
- result: Commit d57f52fe. WBP_AlertObjectives: new Image AlertFrame (T_HUD_AlertFrame, HitTestInvisible, anchors 0.5,0, alignment 0,0, offsets -349,21 size 701 x 90, z 1) over AlertBar (600 x 36 at -300,48 unchanged, background tint 0.012/0.016/0.018 dark track, orange fill unchanged); AlertIconHorde0-3 replaced by Images (T_HUD_AlertMark, 29 x 60, Collapsed); every mark slot anchors 0.5,0, alignment 0.5,0.5, offset -300,66 (centred on the track; was -316 at y 0 or 82), C/B/S font 44 -> 28 outline 3, colours unchanged. PlaceAlertIcon rebuilt with a Widget Icon param (same body: Position x 600, opacity 0.35 when done); ShowAlertIcons rewritten (same switch); HideForOffer/HideForEnd collapse AlertFrame, RestoreAfterOffer sets it HitTestInvisible. Textures imported to /Game/RogueRigs/UI/HUD (UI group, no mips). Compiled with warnings as errors, saved; PIE L_ArenaM1 no LogScript error or Accessed None. Offline composite Saved/Scratch/hud-alert/layout-check-453.png (not a game frame). Not seen on screen (Q26); RRPressure horde test not run (editor open).

## Relations
- [[task-453-alert-bar-redesign]] — parent global task.
