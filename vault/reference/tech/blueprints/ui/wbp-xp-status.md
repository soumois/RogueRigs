---
id: wbp-xp-status
description: "WBP_XPStatus, HUD panel for the XP bar and level text, driven by the miner progression XP event; open for its tree and graphs."
parent: blueprints-ui-index
kind: doc
---
# WBP_XPStatus

- asset_path: /Game/RogueRigs/UI/WBP_XPStatus
- parent_class: URRHUDPanelWidget (/Script/RogueRigs.RRHUDPanelWidget, [[sys-hud-events]])
- purpose: shows the XP bar and the level; a sub-widget of [[wbp-hud]], which only places it (variable XPStatus, child index 2 after HealthStatus, HitTestInvisible, anchors 0,0 to 1,1, zero offsets: it fills the screen so the moved elements keep their old absolute positions). It shows no level-up text: since task-450-01 the only LEVEL UP text is the gold one in the world ([[sys-level-up]]).
- components: CanvasPanel Root; XPBar (ProgressBar, cyan fill 0/0.85/1, anchors 0,1 to 1,1, offsets left 90, top -36, right 20, 16 tall); LevelText (TextBlock "Lv 1", Roboto Bold 22, white, shadow 1,1, anchor bottom-left, 20,-42, auto size). Both are variables.
- variables: none besides the widgets.
- events: hook BindToMiner(Miner, PreviousMiner): UnbindXP(Previous), then for a valid miner assigns the progression component's OnXPChanged to OnXPChanged_Event_2 and calls ShowXP(GetXP, GetXPToNextLevel, GetLevel) to read the current values; null miner shows 0 / 0 and "Lv 1". Handler (editor name with the "_2" suffix from MCP authoring): OnXPChanged_Event_2(Current, Needed, Level) calls ShowXP. The level shown follows OnXPChanged, which carries the level. Destruct unbinds the miner (UnbindXP of GetMiner). Two empty stray custom events OnXPChanged_Event and OnXPChanged_Event_0 remain, unbound.
- functions: ShowXP(Current, Needed, Level): XPBar percent = Current / Needed through a Branch (percent 0 when Needed <= 0; task-104-01: the earlier Select node evaluated the division on both inputs and logged Divide by zero), LevelText "Lv " + Level. UnbindXP(M): for a valid miner, unbinds only this widget's own handler from its progression: UnbindEvent from OnXPChanged fed by a Create Event node bound to OnXPChanged_Event_2 (task-319-02; the earlier ClearDelegate nodes removed every listener, including the level-up flow's, when the HUD was destroyed). (UnbindXP is not named UnbindMiner: a call by that name resolved to the WBP_HealthStatus function of the same name through MCP.)
- removed (task-450-01, owner 2026-11-05): the cyan LevelUpText TextBlock ("LEVEL UP", Roboto Bold 56), its OnLevelUp binding and handler, the one-shot HideLevelUp timer and function, the Destruct timer clear and the OnLevelUp UnbindEvent.
- tick: none, no timer.
- logic_in_bp: display only.
- assets: none.
- owner_agent: agent-bp
- verified: task-450-01: compiled with no error, saved, graphs and widget tree read back; PIE with two GrantXP cheats (level 1 to 9, then 13, two card picks) logged no Accessed None nor script error. Not seen on screen: the owner checks the look in game. Earlier: task-319-02 UnbindXP rewrite and task-104-01 guard, compiled and read back. The C++ source has no lookup of XPBar or LevelText.

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class and the binding hooks.
- [[sys-progression]] — the OnXPChanged event and the getters it reads.
- [[sys-level-up]] — the gold LEVEL UP text in the world, the only level-up text.
