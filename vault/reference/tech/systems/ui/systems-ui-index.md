---
id: systems-ui-index
description: "HUD, minimap, pressure events, subtitles, text content: player-facing display systems."
parent: systems-index
kind: note
---
# C++ systems — ui

HUD, minimap, pressure events, subtitles, text content: player-facing display systems.

## Children
- [[sys-hud]] — HUD widget C++ base: health, XP, level-up, upgrade choice and game-over display events, created by the player controller.
- [[sys-subtitles]] — subtitle subsystem: one voice line at a time, speaker prefix, C++ timer, company alert hooks, widget delegates.
- [[sys-text-content]] — runtime loader of the A4 text files: upgrade flavor, alert lines, barks, fallback, hooks.
- [[sys-main-menu]] — main menu: menu game mode, controller, widget base (New Run, Exit), L_MainMenu start-up map, rr.Menu commands.
- [[sys-pause-menu]] — pause menu: controller open/resume/exit, Options, focus-loss pause, pause keys, stats struct and text formats, rr.Debug.PauseMenu.
- [[sys-game-settings]] — player settings: Sound volumes, Graphics options, Gameplay toggles, display keep-or-revert, saved per user, rr.Settings.
- [[sys-options-screen]] — options screen widget base: Sound, Graphics and Gameplay rows, tabs, Reset, Back, main and pause menu opening, rr.Options.
- [[sys-button-hover]] — shared mouse hover of screen buttons: subsystem, plate material, settings, attach calls, rr.Debug.CursorOn.

## Relations
- [[systems-index]] — parent index.
- [[sys-hud-events]] — system
- [[sys-hud-minimap]] — system
- [[sys-hud-pressure]] — system
