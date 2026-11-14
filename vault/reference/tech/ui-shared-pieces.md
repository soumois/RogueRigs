---
id: ui-shared-pieces
description: "Inventory of repeated UI patterns and UI build scripts (screen, pattern, where), the shared pieces and tools, and what stays per screen with why."
parent: tech-index
kind: doc
---
# UI shared pieces and scripts

Built by [[task-549-ui-shared-patterns]] (commit 685bc5ea). Rule (Q15, owner 2026-11-13): a pattern on 3 or more screens, or a script repeated in 3 or more tasks, is shared; no visible change.

## Shared (use these)
- Plate button hover, all screen buttons: C++ [[sys-button-hover]].
- End-screen entrance, run-lost and stats: C++ [[sys-screen-entrance]].
- Bolted bars, pause and stats dividers: [[wbp-ui-bolted-bar]].
- `Tools/unreal/mcp-client.js` (Node, editor open): the JSON-RPC MCP client that tasks 541, 545, 546, 547 and 548 each rewrote. CLI `list`, `call <toolset> <tool> [json]`, `batch <calls.json>`, `tree <WBP>`, `brushes [WBP...]` (every textured brush grouped by texture; about 6 min for the 14 screens). Library: `call`, `json`, `getProps`, `setProps`, `slotOf`, `brush`, `point`, `stretch`, `addWidget`, `addImage`, `compile`, `save`, and `UI` presets: `frameBrush()`, `titleBarBrush(size, box)`, `plateStyle(size)`, the bolted bar classes. Toolset aliases umg, obj, asset, bp, tex, app, logs; shapes in `Tools/unreal/mcp-tool-sheet.md`.
- `Tools/capture/ui-screens.js`: captures of the seven screens, with before/after comparison ([[tool-capture]]).
- `UI/RRUIDebugCommands.cpp`: `rr.Debug.Cursor`, `CursorOn`, `ButtonHover`, `PressKey`, the UI input of captures.

## Inventory (audit 2026-11-14, `brushes`)
| Pattern | Screens | Shared as |
|---|---|---|
| Frame T_LevelEnd_Frame, Box 942 x 458, margins 0.0955 / 0.1878 | pause, death, stats and run-won, options (and its dialog), loading | `UI.frameBrush()` |
| Plate button T_Shop_ButtonContinue, Box, side margins 0.155 | main menu, pause, options, stats, shop, death, level-up, alert | `UI.plateStyle()`, hover C++ |
| Title bars T_Shop_TitleBar, left and mirrored right | pause, options, stats, shop, death | `UI.titleBarBrush()` |
| Ore icon T_Icon_Ore | stats, shop, card, loading, level-up, alert | texture only |
| Card frames, burn material | shop card, level-up | not shared (2 screens) |

## Kept per screen, with the reason
- Frame, plate and title bar stay plain brushes in each widget: a wrapper widget would change the widget type and names that graphs and the entrance read ("Frame", "TitleBarL"), for no visible gain. Sizes differ on purpose: plates 640 x 163, 416 x 106 (stats and shop Continue), 302 x 77 (options tabs); title bars 210 x 29 (death, options), 356 x 48 (stats, shop), pause Box 276 x 38. Hovered and disabled tints differ (main menu disabled 0.5 at alpha 0.85; options tab hover 1.15; shop Continue 1.25).
- Entrance: 2 screens; the shop has none (Q13 of task-548).
- Bolted bars: 2 screens; the options graphics divider keeps T_LevelEnd_DividerV (a swap would be visible).
- T_LevelEnd_DividerH: no longer referenced; kept, not deleted (art source rule).
- Key presses through Windows (task-540 script): replaced by `rr.Debug.PressKey` (2 tasks). `rr.Debug.Click` stays in the audio debug file.
- `rr.Debug.HordeGauge`: one system, stays in `Debug/`.
- Sprite cut scripts (`Art/Source/*/scripts/*.py`): one asset each (2 tasks).
- Open defect, out of scope: the level 4 subtitle crosses its thin lines (task-381).

## Relations
- [[tech-index]] — parent index.
- [[sys-button-hover]] — shared hover.
- [[sys-screen-entrance]] — shared entrance and rr.Debug.PressKey.
- [[wbp-ui-bolted-bar]] — shared bars.
- [[tool-capture]] — ui-screens.js beside capture.js and sheet.js.
- [[umg-layout]] — layout rule the presets follow.
- [[task-549-ui-shared-patterns]] — the task that made this inventory.
