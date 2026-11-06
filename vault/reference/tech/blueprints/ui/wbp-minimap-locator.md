---
id: wbp-minimap-locator
description: "WBP_MinimapLocator, HUD panel: round minimap in its riveted frame, markers, locator arrow with distance; open for tree, layout and graphs."
parent: blueprints-ui-index
kind: doc
---
# WBP_MinimapLocator

- asset_path: /Game/RogueRigs/UI/WBP_MinimapLocator
- parent_class: URRHUDPanelWidget (/Script/RogueRigs.RRHUDPanelWidget, [[sys-hud-events]])
- purpose: shows the minimap disc, the console / chest / shuttle / miner markers and the off-screen locator arrow with its distance in metres; a sub-widget of [[wbp-hud]], which only places it (variable MinimapLocator, anchors 0,0 to 1,1, zero offsets, z-order 6, HitTestInvisible).
- components: CanvasPanel Root > MinimapImage (Image 200 x 200 at 72,63, z 6, brush material M_Minimap), MarkerConsole (teal 0/0.85/1), MarkerChest (gold 1/0.78/0.1), MarkerShuttle (white), MarkerMiner (teal), all 10 x 10 at 172,163, alignment 0.5, z 7 (miner 8), RadarFrame (Image, not a variable, HitTestInvisible, brush T_HUD_RadarFrame, z 6, last child so it paints over the disc edge and under the markers), the three objective markers Collapsed; LocatorArrow (Image 64 x 64, alignment 0.5, z 5, material M_LocatorArrow, Collapsed), LocatorDistance (TextBlock 24 bold, outline 2, auto size, alignment 0.5, z 5, Collapsed). Values copied from WBP_HUD (task-094-08); all are variables. Variables MinimapMID, bMinimapReady, bArrowPulsing.
- layout (task-457-02, reference screen 1920 x 1080; every slot anchors 0,0 / 0,0, size mode fixed offsets, owner retouches by hand per [[umg-layout]]):

| widget | alignment | offsets (left, top, width, height) | z |
|---|---|---|---|
| RadarFrame | 0,0 | 16, 16, 524, 297 (sprite 1024 x 580 at 0.512) | 6 |
| MinimapImage | 0,0 | 72, 63, 200, 200 (was 20,20) | 6 |
| Markers (4) | 0.5,0.5 | 172, 163, 10, 10 (was 120,120; translations relative, graphs unchanged) | 7 / 8 |
| LocatorArrow, LocatorDistance | 0.5,0.5 | 0, 0 (moved by EdgePosition) | 5 |

  The ring hole is centred at 172,163, diameter about 196, so the ring covers the 200 disc edge by 2 px; clamped markers stay above the ring. The frame's attached plate (inside x 312..491, y 78..159) holds the ore count of [[wbp-ore-counter]]; moving RadarFrame means moving the disc, the markers and the two ore slots by the same amount.
- exposed_defaults: none.
- events: hook BindToHUD(HUD, PreviousHUD) (task-103-02): calls UnbindMinimapHUD(PreviousHUD) (UnbindAll on the three delegates, only if valid), then for a valid HUD assigns OnMinimapTextureReady, OnMinimapViewChanged and OnLocatorViewChanged to custom events (OnMinimapTextureReady_Event_0, OnMinimapViewChanged_Event_0, OnLocatorViewChanged_Event_0, which call SetupMinimap(Texture), UpdateMinimapView(State), UpdateLocatorView(bHasLocator, Locator)) and calls each handler once with GetMinimapTexture, GetMinimapState (if true) and GetObjectiveLocator. Destruct clears the pulse timer and unbinds the HUD. No Construct, no BindToRunFlow override: OnLocatorViewChanged also fires on target changes and carries the distance inputs, so the run flow binding was removed.
- functions: SetupMinimap(Texture): if valid, MID from the image brush, parameter MinimapTex, SetBrushResourceObject, bMinimapReady. UpdateMinimapView(State): if bMinimapReady, MinerUV and ViewRadiusUV (ByInfo setters) and the console / chest / shuttle markers (visible with RenderTranslation = GetMinimapOffset(clamped) x 100, else Collapsed). UpdateLocatorView(bHasLocator, Locator): hide when bHasLocator is false, Target None or bOnScreen; else arrow translation = EdgePosition, angle = ArrowAngle, distance text round(distance to the pawn / 100) + " m" at EdgePosition y +-50, teal for Console, white for Shuttle, starts the pulse timer if not running. PulseLocatorArrow: arrow scale 1 + 0.15 sin(360 x game time). HideLocator: collapses both and stops the timer. UnbindMinimapHUD(Hud). Removed: TickMinimapLocator, FindHUD, HUDRef, bHasTarget.
- tick: nothing runs while the miner is idle and no target is shown. The only per-frame work is the looping 0.016 s timer PulseLocatorArrow, started when the arrow is shown off screen and cleared on HideLocator or Destruct. The pulse stays a timer because it is a render-transform scale, not a material parameter, so M_LocatorArrow Time cannot do it.
- logic_in_bp: display only.
- assets: M_Minimap, M_LocatorArrow, T_HUD_RadarFrame (/Game/RogueRigs/UI/HUD/T_HUD_RadarFrame, 1024 x 580 RGBA, UI group, no mips; source `Art/UI/HUD/T_HUD_RadarFrame.png`, owner-retained candidate of [[art-prompts-hud-unified-mockup]]).
- opacity: M_Minimap (task-184-01) outputs opacity = circle mask only (saturate((0.5 - length(UV - 0.5)) x 100)): 1 inside the circle whatever the texel alpha, 0 outside; colour is black outside the texture bounds and where the texel alpha is 0. Unseen cells are opaque black (MinimapConfig.UnseenColor 0,0,0,255 on WBP_HUD). MinimapImage tint, renderOpacity and WBP_MinimapLocator colorAndOpacity read back 1. Compiled and saved; not seen on screen.
- owner_agent: agent-bp
- verified: task-457-02: compiled with warnings as errors, saved; PIE L_ArenaM1 no LogScript error; frame look not seen on screen (Q26), offline composite only. Earlier: compiled with warnings as errors, saved (task-103-02). Not seen in PIE: delegate assignment, the first-call texture hookup, the markers and the arrow pulse are unverified. Binding uses Assign nodes (AssignOn...), whose event signatures come from the delegates; the unused Bind Event / Create Event route failed on const-ref struct payloads.

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class, OnLocatorTargetChanged and the binding hooks.
- [[sys-hud-minimap]] — the minimap data and getters it displays.
- [[wbp-ore-counter]] — its icon and count sit in this frame's plate.
- [[art-prompts-hud-unified-mockup]] — the frame sprite and its source.
