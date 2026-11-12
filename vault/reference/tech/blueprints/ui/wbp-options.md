---
id: wbp-options
description: "WBP_Options: options screen widget (tabs Sound, Graphics, Gameplay, keep-or-revert dialog) with its anchors table; open to retouch the options layout."
parent: blueprints-ui-index
kind: doc
---
# WBP_Options

- asset_path: /Game/RogueRigs/UI/WBP_Options
- parent_class: URROptionsWidget (/Script/RogueRigs.RROptionsWidget, [[sys-options]])
- purpose: the options screen after the validated mockup ([[art-prompts-options-screen-mockup]]); built through MCP (task-510-07, 3c85e594). Root CanvasPanel, anchors are screen fractions measured on the 1672x941 mockup. The C++ base finds every control by name.
- components (screen anchors (minX,minY)-(maxX,maxY) or point + alignment):

| Widget | Anchors | Notes |
|---|---|---|
| Backdrop (Border) | (0,0)-(1,1) | black alpha 0.94 |
| TitleBarL / TitleBarR (Image) | point (0.5,0.115), align (1,0.5) / (0,0.5) | T_Shop_TitleBar 210x29, offsets -250 / +250, R mirrored |
| Title | point (0.5,0.115) | "OPTIONS", Roboto Bold 56, outline 3 |
| SoundTabButton / GraphicsTabButton / GameplayTabButton | (0.2093,0.1759)-(0.3977,0.2466) / (0.4055..0.5921) / (0.6011..0.7895) | plate set by C++ per tab; label `<Tab>TabText` Roboto Bold 28 |
| Frame (Border) | (0.1388,0.2550)-(0.8612,0.7991) | T_LevelEnd_Frame Box 0.0955 / 0.1878 |
| TabSwitcher (WidgetSwitcher) | (0,0)-(1,1) | pages SoundPage, GraphicsPage, GameplayPage (CanvasPanels) |
| `<Page>Header` + `<Page>Line` | header point (x0,0.3316) align (0,0.5); line (x0,0.3634)-(x1,0.3634) | Roboto Bold 30; line 2 px grey 0.75 alpha 0.6 |
| Row labels `<Key>Label` | point (0.1944 or 0.5371, rowY), align (0,0.5) | size 22 |
| Selector `<Key>Group` (Overlay) | (0.3331,y-0.0213)-(0.4767,y+0.0213) Display; (0.6746..0.8134) Quality | `<Key>Plate` T_Options_SelectorPlate Box 0.08; `<Key>Prev` (mirrored) and `<Key>Next` buttons T_Options_Arrow 22x20, padding 5; `<Key>Value` size 18 |
| Slider `<Key>Group` | ResolutionScale (0.3231..0.4426, ±0.0139); Sound sliders (0.36..0.74) | `<Key>Fill` ProgressBar (T_Options_SliderTrack / SliderFill Box 0.03, padding 5), `<Key>Slider` (thumb T_Options_SliderKnob 21x28, no bar); `<Key>Value` text at x1 + 0.008 |
| Toggle `<Key>Group` | (x1-0.0854,y-0.0185)-(x1,y+0.0185); VSync x1 0.4767, gameplay x1 0.8134 | `<Key>Toggle` CheckBox ToggleButton (T_Options_ToggleOn / ToggleOff Box 0.08), `<Key>Knob` T_Options_ToggleKnob 32x32 and `<Key>State` ON/OFF moved left or right by C++ |
| Rows | Sound y 0.4123 / 0.4950 / 0.5777; Display 0.4123 / 0.4718 / 0.5335 / 0.5951 / 0.6567; Quality 0.4123 / 0.4676 / 0.5239 / 0.5802 / 0.6355 / 0.6908; Gameplay 0.4123 + 0.07 i | keys: Master, Music, Effects; WindowMode, Resolution, VSync, FrameLimit, ResolutionScale; Preset, Shadows, EffectsQuality, AntiAliasing, PostProcessing, Textures; DamageNumbers, AimIndicator, ScreenShake, PauseOnFocusLoss |
| GraphicsDividerV | (0.4910,0.2870)-(0.5090,0.7650) | T_LevelEnd_DividerV, Graphics page only |
| ResetButton / BackButton | point (0.3708,0.8608) / (0.6316,0.8608), align (0.5,0.5), 460x104 | T_Shop_ButtonContinue as the pause menu; labels 27 / 36 |
| ConfirmDialog (CanvasPanel) | (0,0)-(1,1), Collapsed | ConfirmDim (black 0.7), ConfirmFrame (0.30,0.34)-(0.70,0.66), ConfirmTitle point (0.5,0.43) "Keep these display settings?" 32, ConfirmCountdownText (0.5,0.49) 24, KeepButton (0.415,0.575) and RevertButton (0.585,0.575) 300x78 |

- exposed_defaults: none; tunables in `URROptionsSettings` ([[sys-options]]).
- events: none in the graph; buttons, sliders and toggles are bound by the C++ base.
- logic_in_bp: none.
- assets: T_Shop_TitleBar, T_Shop_ButtonContinue, T_LevelEnd_Frame, T_LevelEnd_DividerV, and `/Game/RogueRigs/UI/Options/T_Options_*` (Tab 396x77, SelectorPlate 346x46, Arrow, SliderTrack 263x20, SliderFill 263x19, SliderKnob, ToggleOn and ToggleOff 164x40, ToggleKnob). Box sprites are stored at their drawn size: Slate sizes Box margins from the texture, not from ImageSize.
- verified (task-510-10): -game captures at 1920x1080, 1280x720, 1920x1200, 2560x1080 (`Saved/Captures/t500_c_*`, sheets `Saved/Captures/t500_sheets/`). Re-checked on master after the import ([[task-509-options-old-workflow-on-master]], 2026-11-12): same four resolutions from the main menu and the pause menu, frames identical to the old ones (`Saved/Captures/t509_c_*`, sheets `Saved/Captures/t509_sheets/`); the t500 captures are in the worktree C:/Workspace/RogueRigs-old.
- owner_agent: agent-bp

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-options]] — the C++ base and settings.
- [[options-screen]] — the design.
- [[art-prompts-options-screen-mockup]] — mockup and sprites.
- [[wbp-pause-menu]] — style source and the menu it opens over.
- [[wbp-main-menu]] — the other menu it opens over.
