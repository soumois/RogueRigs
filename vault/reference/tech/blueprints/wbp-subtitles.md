---
id: wbp-subtitles
description: "WBP_Subtitles, the one-line company subtitle widget placed by WBP_HUD and driven by the C++ subtitle events; open for its tree and graph."
parent: blueprints-index
kind: doc
---
# WBP_Subtitles

- asset_path: /Game/RogueRigs/UI/WBP_Subtitles
- parent_class: UserWidget (no C++ class: it binds the delegates of [[sys-subtitles]] itself)
- purpose: shows the current voice line subtitle ("Speaker: line", already composed in C++); a sub-widget of [[wbp-hud]], which only places it.
- components: CanvasPanel Root; SubtitleText (TextBlock, variable, Roboto Bold 26, white, outline 3 black, centred, wrapped, Collapsed by default), anchored to fill the root (anchors 0,0 to 1,1, zero offsets) so the wrap width is the instance width set by WBP_HUD (900).
- exposed_defaults: none.
- events: EventConstruct assigns OnSubtitleShown (custom event OnSubtitleShown_Event_1 with SpeakerId, Speaker, Text, Subtitle, Duration) and OnSubtitleCleared (OnSubtitleCleared_Event_1) of URRSubtitleSubsystem (GetWorldSubsystem); OnSubtitleShown sets SubtitleText to Subtitle and Visibility HitTestInvisible (also on a replace); OnSubtitleCleared sets Collapsed; EventDestruct unbinds all events from both delegates (this widget is their only listener). No Tick, no timer: the C++ owns the timing. The "_1" suffix is the editor naming from authoring through MCP.
- placement (task-094-05): in WBP_HUD the instance bottom edge is now 140 px above the screen bottom (top offset -140, was -50), so it clears the centred weapons row ([[wbp-weapons]], bottom edge 54) and the XP bar; still 900 x 80, anchors 0.5/1, alignment 0.5/1.
- logic_in_bp: display only (show / hide on C++ events).
- assets: none.
- owner_agent: agent-bp
- verified: compiled with warnings as errors, saved; the graph was read back. Not seen on screen or in PIE (nobody sees the screen).

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it (bottom centre, 140 above the bottom) as a child widget instance.
- [[sys-subtitles]] — the subsystem and events it binds.
