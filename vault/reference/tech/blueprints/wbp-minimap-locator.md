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
- components: CanvasPanel Root > MinimapImage (Image 200 x 200 at 20,20, z 6, brush material M_Minimap), MarkerConsole (teal 0/0.85/1), MarkerChest (gold 1/0.78/0.1), MarkerShuttle (white), MarkerMiner (teal), all 10 x 10 at 120,120, alignment 0.5, z 7 (miner 8), the three objective markers Collapsed; LocatorArrow (Image 64 x 64, alignment 0.5, z 5, material M_LocatorArrow, Collapsed), LocatorDistance (TextBlock 24 bold, outline 2, auto size, alignment 0.5, z 5, Collapsed). Values copied from WBP_HUD (task-094-08); all are variables. Variables MinimapMID, bMinimapReady, bArrowPulsing.
- exposed_defaults: none.
- events: hook BindToHUD(HUD, PreviousHUD) (task-103-02): calls UnbindMinimapHUD(PreviousHUD) (UnbindAll on the three delegates, only if valid), then for a valid HUD assigns OnMinimapTextureReady, OnMinimapViewChanged and OnLocatorViewChanged to custom events (OnMinimapTextureReady_Event_0, OnMinimapViewChanged_Event_0, OnLocatorViewChanged_Event_0, which call SetupMinimap(Texture), UpdateMinimapView(State), UpdateLocatorView(bHasLocator, Locator)) and calls each handler once with GetMinimapTexture, GetMinimapState (if true) and GetObjectiveLocator. Destruct clears the pulse timer and unbinds the HUD. No Construct, no BindToRunFlow override: OnLocatorViewChanged also fires on target changes and carries the distance inputs, so the run flow binding was removed.
- functions: SetupMinimap(Texture): if valid, MID from the image brush, parameter MinimapTex, SetBrushResourceObject, bMinimapReady. UpdateMinimapView(State): if bMinimapReady, MinerUV and ViewRadiusUV (ByInfo setters) and the console / chest / shuttle markers (visible with RenderTranslation = GetMinimapOffset(clamped) x 100, else Collapsed). UpdateLocatorView(bHasLocator, Locator): hide when bHasLocator is false, Target None or bOnScreen; else arrow translation = EdgePosition, angle = ArrowAngle, distance text round(distance to the pawn / 100) + " m" at EdgePosition y +-50, teal for Console, white for Shuttle, starts the pulse timer if not running. PulseLocatorArrow: arrow scale 1 + 0.15 sin(360 x game time). HideLocator: collapses both and stops the timer. UnbindMinimapHUD(Hud). Removed: TickMinimapLocator, FindHUD, HUDRef, bHasTarget.
- tick: nothing runs while the miner is idle and no target is shown. The only per-frame work is the looping 0.016 s timer PulseLocatorArrow, started when the arrow is shown off screen and cleared on HideLocator or Destruct. The pulse stays a timer because it is a render-transform scale, not a material parameter, so M_LocatorArrow Time cannot do it.
- logic_in_bp: display only.
- assets: M_Minimap, M_LocatorArrow.
- owner_agent: agent-bp
- verified: compiled with warnings as errors, saved (task-103-02). Not seen in PIE: delegate assignment, the first-call texture hookup, the markers and the arrow pulse are unverified. Binding uses Assign nodes (AssignOn...), whose event signatures come from the delegates; the unused Bind Event / Create Event route failed on const-ref struct payloads.

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class, OnLocatorTargetChanged and the binding hooks.
- [[sys-hud-minimap]] — the minimap data and getters it displays.
