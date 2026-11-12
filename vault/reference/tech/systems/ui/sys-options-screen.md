---
id: sys-options-screen
description: "Options screen C++ as built: URROptionsWidget builds the Sound and Graphics rows, tabs, Reset, Back, keep-or-revert panel, main menu opening, rr.Options."
parent: systems-ui-index
kind: doc
---
# Options screen

- responsibility: the screen of [[options-menu]]: shows every Sound and Graphics option with its value, sends each change to [[sys-game-settings]], switches tabs, resets the current tab, shows the keep-or-revert panel of a display change, closes on Back or Escape. The static layout lives in [[wbp-options]].
- main_classes:
  - `URROptionsWidget` (`Source/RogueRigs/UI/RROptionsWidget.h/.cpp`, Abstract `UUserWidget`, task-505-02, 868a1718): `NativeOnInitialized` builds the 14 rows (`ERROptionRow`, display order: 3 volumes into `SoundList`; window mode, resolution, VSync, frame rate limit, resolution scale, a divider, preset and its 5 details into `GraphicsList`). A row is a CanvasPanel: label, then selector arrows and value, a slider (volumes, resolution scale) or a toggle (VSync); column positions are anchor fractions of the list width (`LeftArrowX`, `ValueX`, `RightArrowX`, `SliderMinX/MaxX`, `ToggleMinX`) and control sizes fractions of the row height, applied in `NativeTick` when the row height changes, so rows follow the frame at every resolution. Each control has a `URROptionsRowHandler` relay. Resolution arrows are disabled in Windowed fullscreen.
  - Functions: `ShowTab(ERROptionsTab Sound|Graphics)` (`NativeConstruct` syncs the display from the window, opens on Sound and focuses the first tab), `ResetCurrentTab`, `Back` (reverts a pending display change, removes the screen, fires `OnOptionsClosed`), `RefreshValues`, `FocusFirstTab`, `StepRow(Row, Direction)`, `SetRowSliderValue(Row, Value)`. `NativeOnKeyDown`: Escape is Revert while `ConfirmPanel` shows, else Back. `HandleDisplayConfirmChanged` shows or collapses `ConfirmPanel`; the countdown text updates each tick.
  - Hover: tabs, Reset, Back, Keep, Revert and the row arrows and toggle take the plate hover of [[sys-button-hover]].
  - `URRMainMenuWidget` (task-505-03, 4c45d661): `OptionsWidgetClass` (soft class, WBP_Options), `OptionsZOrder` 10, `IsOptionsAvailable` (true when the class is set), `OpenOptions` (adds the screen over the menu, false if already open), `GetOptionsWidget`; `HandleOptionsHovered` gives keyboard focus on hover (bound in C++, MCP cannot add the OnHovered event), `HandleOptionsClosed` puts the focus back on OptionsButton. See [[sys-main-menu]].
  - `RROptionsDebugCommands.cpp` (not shipping): `rr.Options open | dump | tab <sound|graphics> | step <Row> <1|-1> | reset | back` (`open` needs the main menu).
- entry_points: `URRMainMenuWidget::OpenOptions` (Options plate); `rr.Options`.
- data: row look on the class defaults of [[wbp-options]] (textures, box margins, Roboto Bold 24, colours).
- depends_on: [[sys-game-settings]], [[sys-button-hover]].
- implements: [[options-menu]] Opening (main menu part), Layout, Sound, Graphics.
- gotchas:
  - Not built: the Gameplay tab (shown disabled, task-506) and the pause menu Options button.
  - Verified (task-505-04): four resolutions of [[umg-layout]], restart, countdown run-out, `Saved/Captures/task505_04_menu_sound_sheet.png`, `task505_04_graphics_sheet.png`, `task505_04_fix_sheet.png`; owner validation in [[wbp-options]].
  - Not seen: mouse hover brightening on the main menu Options plate, Escape by key (`rr.Options back` used).

## Relations
- [[systems-ui-index]] — parent index.
- [[options-menu]] — the design this implements.
- [[wbp-options]] — the widget Blueprint on `URROptionsWidget`.
- [[sys-game-settings]] — the settings it edits.
- [[sys-main-menu]] — opens it from the Options plate.
- [[sys-button-hover]] — hover of its buttons.
- [[task-505-options-screen-sound-graphics]] — the task that built it.
