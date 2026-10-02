---
id: wbp-hud
description: "WBP_HUD, widget Blueprint of URRHUDWidget: health bar, health text, Game Over text; open for its tree and the two event graphs."
parent: blueprints-index
kind: doc
---
# WBP_HUD

- asset_path: /Game/RogueRigs/UI/WBP_HUD
- parent_class: URRHUDWidget (/Script/RogueRigs.RRHUDWidget)
- purpose: in-run HUD display; the data comes from C++ ([[sys-hud]]).
- components: CanvasPanel Root; HealthBar (ProgressBar, top-left 20,20, 280 x 28, red fill 0.9/0.05/0.05, percent 1); HealthText (TextBlock under the bar, "0 / 0"); GameOverText (TextBlock "GAME OVER", size 72, red, centred, Visibility Collapsed). All three are variables.
- exposed_defaults: none.
- events: OnHealthChanged(Current, Max) sets the bar percent to Current / Max (0 when Max <= 0) and the text to "round(Current) / round(Max)"; OnGameOver sets GameOverText Visible. Display only, no gameplay logic.
- logic_in_bp: display only (two event graphs above)
- assets: none.
- owner_agent: agent-bp
- verified: compiled; graph read back through read_graph_dsl; runtime display not yet viewed in PIE.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-hud]] — the C++ base class and events.
- [[bp-rr-player-controller]] — creates it through HUDWidgetClass.
