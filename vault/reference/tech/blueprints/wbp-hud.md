---
id: wbp-hud
description: "WBP_HUD, widget Blueprint of URRHUDWidget: health, XP bar with level, LEVEL UP and Game Over texts; open for its tree and event graphs."
parent: blueprints-index
kind: doc
---
# WBP_HUD

- asset_path: /Game/RogueRigs/UI/WBP_HUD
- parent_class: URRHUDWidget (/Script/RogueRigs.RRHUDWidget)
- purpose: in-run HUD display; the data comes from C++ ([[sys-hud]]).
- components: CanvasPanel Root; HealthBar (ProgressBar, top-left 20,20, 280 x 28, red fill 0.9/0.05/0.05, percent 1); HealthText (TextBlock under the bar, "0 / 0"); GameOverText (TextBlock "GAME OVER", size 72, red, centred, Visibility Collapsed); XPBar (ProgressBar, cyan fill 0/0.85/1, percent 0, anchored bottom and stretched across the width: left 90, right 20, top -36, 16 uu tall); LevelText (TextBlock "Lv 1", size 22, bottom-left at 20, -42, auto size); LevelUpText (TextBlock "LEVEL UP", size 56, cyan, centred a little above the screen centre (0, -120), Visibility Collapsed). All are variables except Root.
- exposed_defaults: none.
- events: OnHealthChanged(Current, Max) sets the bar percent to Current / Max (0 when Max <= 0) and the text to "round(Current) / round(Max)"; OnXPChanged(Current, Needed, Level) sets XPBar percent to Current / Needed (0 when Needed <= 0) and LevelText to "Lv " + Level; OnLevelUp(NewLevel) sets LevelUpText Visible, a Delay of 1 s, then Collapsed (a second level-up inside 1 s hides it early); OnGameOver sets GameOverText Visible. Display only, no gameplay logic.
- logic_in_bp: display only (four event graphs above)
- assets: none.
- owner_agent: agent-bp
- verified: compiled (warnings as errors); graphs written with write_graph_dsl; PIE of 12 s on L_ArenaM1 ran with no divide-by-zero or script warning in the log (task-012-03); display not viewed on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-hud]] — the C++ base class and events.
- [[bp-rr-player-controller]] — creates it through HUDWidgetClass.
