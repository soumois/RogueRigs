---
id: task-510-07-options-widget
description: "Bp: WBP_Options on the C++ base, after the validated mockup, Root Canvas and anchors, three tabs"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: bp

- goal: WBP_Options on the C++ base, after the validated mockup, Root Canvas and anchors, tabs Sound, Graphics, Gameplay and the keep-or-revert dialog
- agent: bp
- depends_on: task-510-06-options-sprites-import
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md)
- expected_output: see done_when
- done_when: WBP_Options compiled on the C++ base, Root Canvas and anchors per umg-layout, laid out after the validated mockup, the three tabs filled with the controls of options-screen, the keep-or-revert dialog laid out; widget class wired.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:33:32Z
- waiting_for: none
- result: 3c85e594. /Game/RogueRigs/UI/WBP_Options on URROptionsWidget, built by MCP (CreateWidgetBlueprint, AddWidget, set_properties): Root CanvasPanel, Backdrop, title OPTIONS with T_Shop_TitleBar rails, three tab buttons, T_LevelEnd_Frame, TabSwitcher full screen with SoundPage (Volume: Master, Music, Effects sliders), GraphicsPage (mockup layout: Display column window mode, resolution, VSync toggle, frame limit, resolution scale; Quality column preset and five sub-settings; DividerV), GameplayPage (four toggles), Reset to defaults and Back plates, ConfirmDialog (dim, frame, title, countdown, Keep, Revert, collapsed). Anchors as fractions measured on the mockup. Seen in -game 1920x1080 (Saved/Captures/t500_g3, t500_g2 dialog): layout matches the mockup. Fixed on the way (one correction each): audio-thread crash at launch (sound classes collected: now held by the subsystem), Slate box caps drawn at texture size (six box sprites resized to their drawn size: Tab 396x77, SelectorPlate 346x46, SliderTrack 263x20, SliderFill 263x19, ToggleOn/Off 164x40, reimported), resolution scale 0 shown as 100 %, slider/checkbox write-back during refresh (it saved 50 % once), tab plate height. Left: the map L_ArenaM1.umap vanished from disk when the editor force-deleted WBP_Options with the map open; restored from git, untouched.

## Relations
- [[task-510-options-screen]] — parent global task.
