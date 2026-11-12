---
id: sys-options-screen
description: "Options screen C++ as built: URROptionsWidget builds the Sound, Graphics and Gameplay rows, tabs, Reset, Back, keep-or-revert panel, main and pause menu opening, rr.Options."
parent: systems-ui-index
kind: doc
---
# Options screen

- responsibility: the screen of [[options-menu]]: shows every Sound, Graphics and Gameplay option with its value, sends each change to [[sys-game-settings]], switches tabs, resets the current tab, shows the keep-or-revert panel of a display change, closes on Back or Escape. The static layout lives in [[wbp-options]].
- main_classes:
  - `URROptionsWidget` (`Source/RogueRigs/UI/RROptionsWidget.h/.cpp`, Abstract `UUserWidget`, task-505-02, 868a1718): `NativeOnInitialized` builds the 14 rows (`ERROptionRow`, display order: 3 volumes into `SoundList`; window mode, resolution, VSync, frame rate limit, resolution scale, a divider, preset and its 5 details into `GraphicsList`; task-506, 12080e3e: Damage numbers, Aim indicator, Screen shake, Pause when the window loses focus into the optional `GameplayList`, On/Off toggles in the VSync look, 18 rows in all). A row is a CanvasPanel: label, then selector arrows and value, a slider (volumes, resolution scale) or a toggle (VSync); column positions are anchor fractions of the list width (`LeftArrowX`, `ValueX`, `RightArrowX`, `SliderMinX/MaxX`, `ToggleMinX`) and control sizes fractions of the row height, applied in `NativeTick` when the row height changes, so rows follow the frame at every resolution. Each control has a `URROptionsRowHandler` relay. Resolution arrows are disabled in Windowed fullscreen.
  - Gameplay tab: optional bindings `GameplayTab`, `GameplayTabButton` (enabled when `GameplayList` exists), `GameplayTabSelected`, `GameplayTabText`; each toggle calls `SetGameplayOption` of [[sys-game-settings]].
  - Functions: `ShowTab(ERROptionsTab Sound|Graphics|Gameplay)` (`NativeConstruct` syncs the display from the window, opens on Sound and focuses the first tab), `ResetCurrentTab` (current tab only), static `GetTabName`, `Back` (reverts a pending display change, removes the screen, fires `OnOptionsClosed`), `RefreshValues`, `FocusFirstTab`, `StepRow(Row, Direction)`, `SetRowSliderValue(Row, Value)`. `NativeOnKeyDown`: Escape is Revert while `ConfirmPanel` shows, else Back. `HandleDisplayConfirmChanged` shows or collapses `ConfirmPanel`; the countdown text updates each tick.
  - Hover: tabs, Reset, Back, Keep, Revert and the row arrows and toggle take the plate hover of [[sys-button-hover]].
  - `URRMainMenuWidget` (task-505-03, 4c45d661): `OptionsWidgetClass` (soft class, WBP_Options), `OptionsZOrder` 10, `IsOptionsAvailable` (true when the class is set), `OpenOptions` (adds the screen over the menu, false if already open), `GetOptionsWidget`; `HandleOptionsHovered` gives keyboard focus on hover (bound in C++, MCP cannot add the OnHovered event), `HandleOptionsClosed` puts the focus back on OptionsButton. See [[sys-main-menu]].
  - `URRPauseMenuWidget` (task-506, 12080e3e): optional `OptionsButton`, `OptionsWidgetClass` (soft class, WBP_Options), `OptionsZOrder` 42 (pause menu 40, run-lost 45), `OpenOptions`, `CloseOptions`, `IsOptionsOpen`; the game stays paused, closing puts the focus back on OptionsButton, `NativeDestruct` removes a screen still open. The pause key goes to Back while it is open ([[sys-pause-menu]]).
  - `RROptionsDebugCommands.cpp` (not shipping): `rr.Options open | dump | tab <sound|graphics|gameplay> | step <Row> <1|-1> | reset | back` (`open` needs the main menu; in a run `rr.Debug.PauseMenu options`).
- entry_points: `URRMainMenuWidget::OpenOptions` (Options plate), `URRPauseMenuWidget::OpenOptions` (Options button); `rr.Options`.
- data: row look on the class defaults of [[wbp-options]] (textures, box margins, Roboto Bold 24, colours).
- depends_on: [[sys-game-settings]], [[sys-button-hover]].
- implements: [[options-menu]] Opening, Layout, Sound, Graphics, Gameplay.
- gotchas:
  - Verified (task-506-04, `Saved/Captures/t506_04_sheet.png`): Gameplay tab with its four toggles at the four resolutions, nothing clipped; opened from the pause menu, the pause key returns to it. Kept for the owner: the selected Gameplay label slightly overruns the narrow beige centre at 1920x1200; the pause menu shows faintly through the backdrop.
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
- [[task-506-options-gameplay-and-pause]] — added the Gameplay tab and the pause menu opening.
- [[sys-pause-menu]] — opens it from the Options button.
