---
id: wbp-options
description: "WBP_Options: options screen widget, title, Sound/Graphics/Gameplay tabs, frame with row lists, Reset and Back, keep-or-revert panel; anchors table."
parent: blueprints-ui-index
kind: doc
---
# WBP_Options

- asset_path: /Game/RogueRigs/UI/WBP_Options (task-505-02, 868a1718; labels 30 -> 25 in caeb7d8f; GameplayList and Gameplay tab enabled in d2d0040e, task-506-02)
- parent_class: `URROptionsWidget` ([[sys-options-screen]])
- purpose: static layout of the options screen of [[options-menu]], opened from [[wbp-main-menu]] and [[wbp-pause-menu]] (Z 42 over the pause menu), drawn from the validated task-504 mockup ([[art-prompts-options-screen-mockup]]); C++ fills the rows.
- components (Root CanvasPanel, top-level widgets, anchors as fractions of the screen, offsets 0 unless noted, all stretch except the point anchors):

| Widget | Anchors min - max | Alignment / offsets | Notes |
|---|---|---|---|
| Backdrop (Border) | (0,0)-(1,1) | | black, alpha 0.93 |
| TitleBarL, TitleBarR (Image) | point (0.5,0.0723) | align (1,0.5) / (0,0.5); left -240 / 240, size 210x29 | T_Shop_TitleBar, R mirrored (scale X -1) |
| Title (TextBlock) | point (0.5,0.0723) | align (0.5,0.5), auto size | "OPTIONS", size 56, outline 3 |
| SoundTab (Overlay) | (0.2213,0.1275)-(0.3977,0.186) | | tab, see below |
| GraphicsTab (Overlay) | (0.4067,0.1275)-(0.5921,0.186) | | tab |
| GameplayTab (Overlay) | (0.6011,0.1275)-(0.7775,0.186) | | tab, enabled (task-506) |
| Frame (Border) | (0.1752,0.1913)-(0.8254,0.831) | | T_LevelEnd_Frame, Box margin 0.0955 / 0.1878 |
| SoundList, GraphicsList, GameplayList (VerticalBox) | (0.2303,0.2391)-(0.7715,0.7917) | | empty in the designer, rows built by C++; one shown per tab |
| ResetButton (Button) | (0.245,0.85)-(0.49,0.944) | | "Reset to defaults", size 25 |
| BackButton (Button) | (0.511,0.85)-(0.755,0.944) | | "Back", size 30 |
| ConfirmPanel (CanvasPanel) | (0,0)-(1,1) | | Collapsed until a display change; children below |

- tab: Overlay of `<Tab>TabButton` (T_Options_TabPlate, Box margin L/R 0.12), `<Tab>TabSelected` (Image T_Shop_ButtonContinue, Hidden, shown on the selected tab) and `<Tab>TabLabelBox` (ScaleBox, HitTestInvisible) with `<Tab>TabText` (size 25).
- buttons: Reset, Back, Keep, Revert use T_Shop_ButtonContinue, Box margin L/R 0.155, Pressed tint 0.7, Disabled tint 0.5 alpha 0.85, as [[wbp-pause-menu]]; each label in a ScaleBox (down only).
- ConfirmPanel children: ConfirmBackdrop (0,0)-(1,1), black alpha 0.75; ConfirmFrame (0.27,0.33)-(0.73,0.67), T_LevelEnd_Frame; ConfirmTitle point (0.5,0.43) "Keep these display settings?" size 30; ConfirmCountdownText point (0.5,0.49) "Reverting in 15 s" size 26; KeepButton (0.335,0.555)-(0.49,0.625); RevertButton (0.51,0.555)-(0.665,0.625); labels size 30.
- exposed_defaults: ArrowTexture T_Options_ArrowButton, SliderTrackTexture T_Options_SliderTrack, SliderKnobTexture T_Options_SliderKnob, ToggleTrackTexture T_Options_ToggleTrack, ToggleThumbTexture T_Options_ToggleThumb (all `/Game/RogueRigs/UI/Options/`); RowFont Roboto Bold 24; colours and column fractions from the C++ defaults ([[sys-options-screen]]).
- events: none in the graph; clicks, Escape, tabs and the confirmation are bound in C++.
- logic_in_bp: none.
- assets: T_Options_* above, T_Options_TabPlate, T_Shop_TitleBar, T_Shop_ButtonContinue, T_LevelEnd_Frame; Roboto.
- owner_agent: agent-bp
- verified (task-505-04): -game at 1280x720, 1920x1080, 1920x1200, 2560x1080, nothing clipped; owner validated the screen as is (2026-11-12, "je le valide"), keeping two differences from the mockup: rows, arrows and labels smaller; selected tab drawn as the narrow plate with a beige centre.
- verified (task-506-04, `Saved/Captures/t506_04_sheet.png`): Gameplay tab with four toggles at the four resolutions, nothing clipped in the rows; the selected Gameplay label slightly overruns the beige centre at 1920x1200 (kept, owner look point).

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-options-screen]] — the C++ base and behaviour.
- [[options-menu]] — the design.
- [[wbp-main-menu]] — opens it from the Options plate.
- [[wbp-pause-menu]] — source of the plate button style; opens it from the Options button.
- [[art-prompts-options-screen-mockup]] — the mockup and its sprites.
- [[umg-layout]] — the layout rule of the anchors table.
