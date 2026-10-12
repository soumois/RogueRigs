---
id: sys-hud-minimap
description: "Minimap data as built: one-pixel-per-cell seen-terrain texture, update and seen rules, marker state, UV mapping; open for the minimap widget."
parent: sys-hud
kind: doc
---
# HUD minimap data

- responsibility: data of the round minimap ([[readability-rules]] Minimap line): a terrain texture of the cells already seen and the miner, console, bonus chest and shuttle positions. Drawing (circle, material, markers) is the widget Blueprint's (task-061-02).
- main_classes: `URRMinimap : UObject` (`Source/RogueRigs/UI/RRMinimap.h/.cpp`), owned by `URRHUDWidget` (`Minimap`, Transient); structs `FRRMinimapConfig`, `FRRMinimapState`.
- texture: transient `UTexture2D` B8G8R8A8, sRGB, nearest filter, clamp, never streamed, one pixel per grid cell (240 × 240 on `arena_m1_fine`). Orientation screen-aligned for camera yaw 0 (screen up = world +X, right = world +Y): pixel u = cell Y, v = grid width − 1 − cell X. Colors: Rock, Ore, SpecialOre, Barrel → `RockColor`; Floor, Spawn → `FloorColor`; HealOre → `HealVeinColor`; Void → `VoidColor`; never seen → `UnseenColor` (transparent).
- seen rule ([[readability-rules]] Minimap reveal, task-064-01): each HUD tick reads the miner cell (`BoundMiner`, else the owning pawn); when it differs from the last one, every unseen cell whose center lies within `RevealRadiusCells` of the miner cell center becomes seen for the rest of the level. A circle, the same in every direction (replaces the camera footprint trapezoid, which revealed more upward).
- tick owner after the HUD split (task-094-10): `URRHUDWidget::NativeTick` still calls `UpdateMinimap` (`Minimap->Tick`), the root HUD ticks even though WBP_HUD has no Blueprint tick; the reveal follows the moving miner and no event drives it, so it stays per frame. WBP_MinimapLocator draws from the root HUD getters (`GetMinimapTexture`, `GetMinimapState`); no widget lookup by name is involved.
- update rule and cost: a pixel is written only when a cell becomes seen or a seen cell is destroyed (`ARRArenaGrid::OnCellDestroyed`, [[sys-grid]]); the changed pixels of a frame are copied as one dirty rectangle to `UpdateTextureRegions`; nothing is sent on a frame without change, and the seen pass (one disc of about π R² bit tests, row half-widths from one square root per row) runs only when the miner enters another cell.
- entry_points (`URRHUDWidget`, category HUD|Minimap): pure `GetMinimapTexture()` (null until a built grid is found); event `OnMinimapTextureReady(UTexture2D* Texture)` (once per HUD); pure `GetMinimapState(FRRMinimapState& OutState)` (false without texture) with `TextureSize`, `MinerPosition`, `MinerUV`, `ViewRadiusCells`, `ViewRadiusUV`, `bHasConsole` / `ConsolePosition`, `bHasBonusChest` / `BonusChestPosition`, `bHasShuttle` / `ShuttlePosition` (positions in texture pixels, float; a console or chest is listed whatever its state, the shuttle once landed); static pure `GetMinimapOffset(Position, Center, Radius, bClampToCircle, bOutOfRange)` = (Position − Center) / Radius, clamped to length 1 when asked.
- UV mapping for the widget: sample UV = `MinerUV` + (WidgetUV − 0.5) × 2 × `ViewRadiusUV`, masked to the circle; a marker sits at the circle center + `GetMinimapOffset(MarkerPosition, MinerPosition, ViewRadiusCells)` × circle radius (100 px).
- data: `MinimapConfig` (`FRRMinimapConfig`, EditDefaultsOnly on the HUD widget Blueprint): `bEnabled` true, `ViewRadiusCells` 40, `RevealRadiusCells` 18 (provisional, owner 2026-10-09), `RockColor` (176, 166, 196), `FloorColor` (58, 56, 66), `HealVeinColor` (214, 38, 38), `VoidColor` (8, 8, 12), `UnseenColor` (0, 0, 0, 0). Floor and void colors are provisional choices of task-061-01: [[color-code]] has no floor or void entry.
- depends_on: [[sys-grid]], [[sys-objectives]] (console, bonus chest, shuttle getters), [[sys-player]] (miner camera).
- implements: [[readability-rules]] Minimap line (data side).
- gotchas: a camera yaw other than 0 needs the image and offsets rotated by the widget. Headless tests create no HUD, so no minimap. A grid rebuilt at runtime is not re-read. No test covers the minimap (task scope).

## Relations
- [[sys-hud]] — parent: the HUD widget that owns the minimap.
- [[sys-grid]] — the cell types and destruction event mirrored.
- [[sys-objectives]] — the objective actors marked.
- [[readability-rules]] — the owner's minimap rule.
- [[color-code]] — the terrain colors.
