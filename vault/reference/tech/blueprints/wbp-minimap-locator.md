---
id: wbp-minimap-locator
description: "WBP_MinimapLocator, HUD panel for the round minimap, its markers and the locator arrow with distance; open for its tree and graphs."
parent: blueprints-index
kind: doc
---
# WBP_MinimapLocator

- asset_path: /Game/RogueRigs/UI/WBP_MinimapLocator
- parent_class: URRHUDPanelWidget (/Script/RogueRigs.RRHUDPanelWidget, [[sys-hud-events]])
- purpose: shows the minimap disc, the console / chest / shuttle / miner markers and the off-screen locator arrow with its distance in metres; a sub-widget of [[wbp-hud]], which only places it (variable MinimapLocator, anchors 0,0 to 1,1, zero offsets, z-order 6, HitTestInvisible).
- components: CanvasPanel Root > MinimapImage (Image 200 x 200 at 20,20, z 6, brush material M_Minimap), MarkerConsole (teal 0/0.85/1), MarkerChest (gold 1/0.78/0.1), MarkerShuttle (white), MarkerMiner (teal), all 10 x 10 at 120,120, alignment 0.5, z 7 (miner 8), the three objective markers Collapsed; LocatorArrow (Image 64 x 64, alignment 0.5, z 5, material M_LocatorArrow, Collapsed), LocatorDistance (TextBlock 24 bold, outline 2, auto size, alignment 0.5, z 5, Collapsed). Values copied from WBP_HUD (task-094-08); all are variables. Variables MinimapMID, HUDRef (URRHUDWidget), bMinimapReady, bHasTarget.
- exposed_defaults: none.
- events: hook BindToRunFlow(RunFlow, PreviousRunFlow): valid run flow: assigns OnLocatorTargetChanged to a custom event (editor name OnLocatorTargetChanged_Event), unbinds all from PreviousRunFlow, then reads GetLocatorTarget and sets bHasTarget (None also collapses the arrow and distance); invalid: bHasTarget false, hidden. The event does the same switch on Target. Construct starts a looping timer (function TickMinimapLocator, 0.016 s); Destruct clears it and unbinds all from GetRunFlow.
- functions: TickMinimapLocator: no HUD reference yet -> FindHUD (GetAllWidgetsOfClass URRHUDWidget, top level, cast, stored in HUDRef); else UpdateMinimapView, and UpdateLocatorView only while bHasTarget. SetupMinimap: when GetMinimapTexture is valid, MID from the image brush, texture parameter MinimapTex, SetBrushResourceObject, bMinimapReady (replaces the HUD event OnMinimapTextureReady, which is not a delegate). UpdateMinimapView: GetMinimapState of the HUD, vector MinerUV, scalar ViewRadiusUV (ByInfo setters), markers: visible with RenderTranslation = GetMinimapOffset(clamped) x 100, else Collapsed (the old PlaceMarker is inlined three times). UpdateLocatorView: GetObjectiveLocator; off screen: arrow translation = EdgePosition, angle = ArrowAngle, scale 1 + 0.15 sin(360 x game time), distance text round(distance to the pawn / 100) + " m" at EdgePosition y +-50, teal for Console, white for Shuttle; else HideLocator.
- tick: timer 0.016 s always runs (the minimap scroll is continuous and the texture has no event); the arrow part runs only while a target exists. WBP_HUD has no EventTick any more. Minimap data is still ticked by C++ (URRHUDWidget::NativeTick, Minimap->Tick).
- logic_in_bp: display only.
- assets: M_Minimap, M_LocatorArrow.
- owner_agent: agent-bp
- verified: compiled with warnings as errors, saved. Not seen on screen or in PIE; binding, texture hookup, markers and arrow unverified. No C++ or test looks the moved widgets up by name (grep of Source: only OnMinimapTextureReady in RRHUDWidget.cpp/.h, now unimplemented in Blueprint, harmless). The panel reaches the HUD through GetAllWidgetsOfClass because GetMinimapState, GetMinimapTexture and GetObjectiveLocator are URRHUDWidget members (task-094-10 could move them to a subsystem or the panel base).

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class, OnLocatorTargetChanged and the binding hooks.
- [[sys-hud-minimap]] — the minimap data and getters it displays.
