---
id: wbp-xp-status
description: "WBP_XPStatus, HUD panel for the XP bar, level text and LEVEL UP flash, driven by the miner progression events; open for its tree and graphs."
parent: blueprints-index
kind: doc
---
# WBP_XPStatus

- asset_path: /Game/RogueRigs/UI/WBP_XPStatus
- parent_class: URRHUDPanelWidget (/Script/RogueRigs.RRHUDPanelWidget, [[sys-hud-events]])
- purpose: shows the XP bar, the level and the LEVEL UP flash; a sub-widget of [[wbp-hud]], which only places it (variable XPStatus, child index 2 after HealthStatus, HitTestInvisible, anchors 0,0 to 1,1, zero offsets: it fills the screen so the moved elements keep their old absolute positions).
- components: CanvasPanel Root; XPBar (ProgressBar, cyan fill 0/0.85/1, anchors 0,1 to 1,1, offsets left 90, top -36, right 20, 16 tall); LevelText (TextBlock "Lv 1", Roboto Bold 22, white, shadow 1,1, anchor bottom-left, 20,-42, auto size); LevelUpText (TextBlock "LEVEL UP", Roboto Bold 56, cyan 0/0.85/1, anchor 0.5/0.5, alignment 0.5/0.5, 0,-120, auto size, Collapsed). All three are variables, same values as they had in WBP_HUD.
- variables: none besides the widgets.
- events: hook BindToMiner(Miner, PreviousMiner): UnbindXP(Previous), then for a valid miner assigns the progression component's OnXPChanged and OnLevelUp to the handlers and calls ShowXP(GetXP, GetXPToNextLevel, GetLevel) to read the current values; null miner shows 0 / 0 and "Lv 1". Handlers (editor names with the "_2" suffix from MCP authoring): OnXPChanged_Event_2(Current, Needed, Level) calls ShowXP; OnLevelUp_Event_2 sets LevelUpText Visible and starts a one-shot 1 s timer HideLevelUp (a second level-up inside 1 s restarts it). Destruct unbinds the miner (UnbindXP of GetMiner) and clears the timer.
- functions: ShowXP(Current, Needed, Level): XPBar percent = Current / Needed through a Branch (percent 0 when Needed <= 0; task-104-01: the earlier Select node evaluated the division on both inputs and logged Divide by zero), LevelText "Lv " + Level. UnbindXP(M): unbinds all OnXPChanged and OnLevelUp listeners of a valid miner's progression. HideLevelUp: LevelUpText Collapsed. (UnbindXP is not named UnbindMiner: a call by that name resolved to the WBP_HealthStatus function of the same name through MCP.)
- tick: none, no timer except the one-shot HideLevelUp (replaces the Delay of the old OnLevelUp handler in WBP_HUD).
- logic_in_bp: display only.
- assets: none.
- owner_agent: agent-bp
- verified: task-104-01 guard compiled 0 errors, saved, graph read back. Earlier: compiled with warnings as errors, saved, graphs read back. Not seen on screen or in PIE; the binding and the flash at run time are unverified. The C++ source has no lookup of XPBar, LevelText or LevelUpText, so nothing breaks.

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class and the binding hooks.
- [[sys-progression]] — the OnXPChanged and OnLevelUp events and the getters it reads.
