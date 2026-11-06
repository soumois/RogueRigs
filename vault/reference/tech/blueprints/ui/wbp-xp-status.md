---
id: wbp-xp-status
description: "WBP_XPStatus, HUD panel for the XP bar and level text, driven by the miner progression XP event; open for its tree and graphs."
parent: blueprints-ui-index
kind: doc
---
# WBP_XPStatus

- asset_path: /Game/RogueRigs/UI/WBP_XPStatus
- parent_class: URRXPStatusWidget (/Script/RogueRigs.RRXPStatusWidget, [[sys-hud-xp-bar]]; reparented by task-456-02 from URRHUDPanelWidget, [[sys-hud-events]]), which drives XPBar and XPGhostBar
- purpose: shows the XP bar and the level; a sub-widget of [[wbp-hud]], which only places it (variable XPStatus, child index 2 after HealthStatus, HitTestInvisible, anchors 0,0 to 1,1, zero offsets: it fills the screen so the moved elements keep their old absolute positions). It shows no level-up text: since task-450-01 the only LEVEL UP text is the gold one in the world ([[sys-level-up]]).
- components: CanvasPanel Root > XPBarArea (CanvasPanel) > XPGhostBar (ProgressBar, variable, HitTestInvisible, fill linear 0.578/0.922/1 = #c8f6ff pale cyan, also set by C++; background the default ProgressBar_Background with white tint, so it draws the empty bar), XPBar (ProgressBar, cyan fill 0/0.85/1, variable; background tint alpha 0 since task-456-02 so the ghost shows behind where the fill is empty), then XPTick20, XPTick40, XPTick60, XPTick80 (Images, default white brush tinted white A 0.75, HitTestInvisible); Root > LevelText (TextBlock "Lv 1", Roboto Bold 22, white, shadow 1,1, variable). Draw order in XPBarArea: XPGhostBar, XPBar, ticks: the ghost is behind the fill (as the health chunk, [[wbp-health-status]]) and the ticks above both.
- ticks (task-455-01, Q31 owner 2026-11-05, provisional): 4 thin light lines cut the bar into 5 equal parts at 20, 40, 60, 80 % whatever the level; they are anchored to fractions of XPBarArea, so they follow the bar width at every resolution. Look from the retained mockup Art/Source/hud-unified/20261105-152627-hud-unified-1.png (thin light vertical marks, shorter than the bar), positions set exactly in UMG (the mockup spacing is wrong).
- layout (per [[umg-layout]]; alignment 0,0 everywhere):

| Widget | Parent | Anchors min / max | Offsets (L, T, R, B) | Size mode |
|---|---|---|---|---|
| XPBarArea | Root | 0,1 / 1,1 | 162, -44, 45, 22 (task-467; was 122, -36, 26, 16) | width stretches with the screen, 22 tall (the rail window, y 1036..1058 at 1080p) |
| XPGhostBar | XPBarArea | 0,0 / 1,1 | 0, 0, 0, 0 | fills XPBarArea |
| XPBar | XPBarArea | 0,0 / 1,1 | 0, 0, 0, 0 | fills XPBarArea |
| XPTick20 / 40 / 60 / 80 | XPBarArea | x 0.2 / 0.4 / 0.6 / 0.8, y 0 to 1 | -1.5, 3, 3, 3 (task-467; was -1, 3, 2, 3) | 3 wide (centred on the fraction), height stretches with 3 inset top and bottom; tint cream 1 / 0.95 / 0.85, alpha 1 (was white 0.75) |
| XPFrame | Root | 0,1 / 1,1 | 19, -62, 19, 58 (task-467; was 103, -47, 7, 37) | width stretches, 58 tall (x 19..1901, y 1018..1076 at 1080p) |
| LevelCap | Root | 0,1 / 0,1 | 12, -63, 150, 58 (task-467; was 8, -50, 108, 44) | 150 x 58, auto size off |
| LevelText | Root | 0,1 / 0,1, alignment 0.5,0.5 | 87, -34 (task-467; was 62, -28) | auto size, centred on LevelCap |

- rail (task-467-04, commit 75272a91, current; owner 2026-11-06, target Art/Source/hud-unified/20261105-152627-hud-unified-1.png): XPFrame brush T_HUD_XPRail (840 x 58 texture) Draw As Box, ImageSize 840 x 58, margins L/R 0.031, T/B 0.31, tint white: a thin straight riveted rail with square ends and a transparent window over the bar. LevelCap brush T_HUD_LevelPlate (216 x 55) Box, ImageSize 216 x 55, margins 0.065 / 0.22, tint white: framed plate with small corner rivets. Box borders draw at the texture pixel size in game, so both sprites are cut at their 1080p size ([[art-prompts-hud-unified-mockup]]). Graphs unchanged. Verified: -game captures at 1920 x 1080 (t467_04, t467_06_final): rail along the bottom edge, Lv 1 legible in its plate, four ticks clearly visible, after GrantXP the ghost and fill inside the rail, no script error. The card flight to the XP bar was not caught on a frame.
- frame (task-461-03, commit 81462e37, superseded by task-467 above; owner 2026-11-05 image 2, Q37): Root children in order XPBarArea, XPFrame, LevelCap, LevelText. XPFrame (Image, variable, HitTestInvisible): /Game/RogueRigs/UI/HUD/T_HUD_XPFrame Draw As Box, ImageSize 384 x 37, margins L/R 0.06, T/B 0.3, tint 0.8 grey (the critic found the sprite teal lighter than the panel): riveted rails above and below the bar, bolted end caps, transparent track window over the bar. LevelCap (Image, variable, HitTestInvisible): T_HUD_LevelCap Box, ImageSize 126 x 44, margins 0.2 / 0.3, tint 0.8: framed plate drawn over the frame start, LevelText ("Lv N", justification centre) on it. XPGhostBar background tint #0B0C14 (linear 0.0033465 / 0.0036765 / 0.0069954, was white): the empty track is dark as in the mockup; ghost fill, XPBar and ticks unchanged. Owner retouch points: XPFrame and LevelCap slots and ImageSize, XPBarArea offsets. Verified: compiled, saved; PIE GrantXP 3 and 40, two card picks, no runtime error. Look not seen (A6: the owner checks in game); "Lv 10" and above may be tight in the 108 wide cap.
- variables: none besides the widgets.
- events: hook BindToMiner(Miner, PreviousMiner): UnbindXP(Previous), then for a valid miner assigns the progression component's OnXPChanged to OnXPChanged_Event_2 and calls ShowXP(GetXP, GetXPToNextLevel, GetLevel) to read the current values; null miner shows 0 / 0 and "Lv 1". Handler (editor name with the "_2" suffix from MCP authoring): OnXPChanged_Event_2(Current, Needed, Level) calls ShowXP. The C++ parent binds the same progression natively for the bars ([[sys-hud-xp-bar]]). The level shown follows OnXPChanged, which carries the level. Destruct unbinds the miner (UnbindXP of GetMiner). Two empty stray custom events OnXPChanged_Event and OnXPChanged_Event_0 remain, unbound.
- functions: ShowXP(Current, Needed, Level): LevelText "Lv " + Level only (task-456-02 removed the XPBar SetPercent branch and 25 stray nodes: the C++ parent owns both bar percents; Current and Needed stay as unused inputs). UnbindXP(M): for a valid miner, unbinds only this widget's own handler from its progression: UnbindEvent from OnXPChanged fed by a Create Event node bound to OnXPChanged_Event_2 (task-319-02; the earlier ClearDelegate nodes removed every listener, including the level-up flow's, when the HUD was destroyed). (UnbindXP is not named UnbindMiner: a call by that name resolved to the WBP_HealthStatus function of the same name through MCP.)
- removed (task-450-01, owner 2026-11-05): the cyan LevelUpText TextBlock ("LEVEL UP", Roboto Bold 56), its OnLevelUp binding and handler, the one-shot HideLevelUp timer and function, the Destruct timer clear and the OnLevelUp UnbindEvent.
- tick: none in the Blueprint; the C++ parent arms a next-tick update only while the ghost or fill moves ([[sys-hud-xp-bar]]).
- logic_in_bp: display only.
- assets: T_HUD_XPRail, T_HUD_LevelPlate (task-467, [[art-prompts-hud-unified-mockup]]); T_HUD_XPFrame and T_HUD_LevelCap (task-461) no longer used.
- owner_agent: agent-bp
- verified: task-456-02 (commit 441acaae): reparented, compiled true, saved; tree read back (XPBarArea: XPGhostBar, XPBar, XPTick20..80), XPGhostBar slot and XPBar background alpha read back, ShowXP DSL read back (one SetText chain, 6 nodes). PIE L_ArenaM1 with GrantXP 3 (level 1, 3 / 4) then 40 (level 3, 19 / 25) and two card picks: no Accessed None nor script error. Ghost and catch-up not seen on screen (Q26: no capture): the owner checks the look in game. Earlier, task-455-01 (commit 5e101e15): compiled with no error, saved, widget tree read back with the values above; graphs untouched; PIE L_ArenaM1 with GrantXP 3 then 40 (level 1 to 3) logged no Accessed None nor script error. The ticks are not seen on screen (Q26: no capture): the owner checks the look in game. Earlier, task-450-01: compiled with no error, saved, graphs and widget tree read back; PIE with two GrantXP cheats (level 1 to 9, then 13, two card picks) logged no Accessed None nor script error. Not seen on screen: the owner checks the look in game. Earlier: task-319-02 UnbindXP rewrite and task-104-01 guard, compiled and read back. The C++ source has no lookup of XPBar or LevelText.

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class and the binding hooks.
- [[sys-progression]] — the OnXPChanged event and the getters it reads.
- [[sys-level-up]] — the gold LEVEL UP text in the world, the only level-up text.
- [[umg-layout]] — the layout rule its anchors table follows.
- [[sys-hud-xp-bar]] — the C++ parent that drives the fill and the ghost.
- [[art-prompts-hud-unified-mockup]] — the XP frame and level cap sprites (task-461).
