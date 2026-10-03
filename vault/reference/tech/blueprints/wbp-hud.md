---
id: wbp-hud
description: "WBP_HUD, widget Blueprint of URRHUDWidget: health, XP bar, LEVEL UP, Game Over and the upgrade card screen; open for its tree and graphs."
parent: blueprints-index
kind: doc
---
# WBP_HUD

- asset_path: /Game/RogueRigs/UI/WBP_HUD
- parent_class: URRHUDWidget (/Script/RogueRigs.RRHUDWidget)
- purpose: in-run HUD display; the data comes from C++ ([[sys-hud]]).
- components: CanvasPanel Root; HealthBar (ProgressBar, top-left 20,20, 280 x 28, red fill 0.9/0.05/0.05, percent 1); HealthText (TextBlock under the bar, "0 / 0"); GameOverText (TextBlock "GAME OVER", size 72, red, centred, Visibility Collapsed); XPBar (ProgressBar, cyan fill 0/0.85/1, percent 0, anchored bottom and stretched across the width: left 90, right 20, top -36, 16 uu tall); LevelText (TextBlock "Lv 1", size 22, bottom-left at 20, -42, auto size); LevelUpText (TextBlock "LEVEL UP", size 56, cyan, centred a little above the screen centre (0, -120), Visibility Collapsed). UpgradeScreen (CanvasPanel, full screen, z-order 10, Visibility Collapsed) = Backdrop (Border, black alpha 0.7) + Panel (VerticalBox, centred, auto size): TitleText "Choisis une amélioration" (size 36), CardsRow (HorizontalBox) of three SizeBox 260 x 230 (CardSize0..2) each holding CardBorder (Border, tier colour, padding 6) > CardButton (Button) > CardBox (VerticalBox) with CardName (26), CardTarget (18), CardValue (30) TextBlocks (centred, wrapped), then RerollButton with RerollText "Relancer". Card, border, size and text widgets, RerollButton and UpgradeScreen are variables. The upgrade UI lives in this widget (a separate WBP_UpgradeChoice could not be called from the HUD graph through MCP and was deleted).
- Other: All other named widgets are variables except Root.
- exposed_defaults: none.
- events: OnHealthChanged(Current, Max) sets the bar percent to Current / Max (0 when Max <= 0) and the text to "round(Current) / round(Max)"; OnXPChanged(Current, Needed, Level) sets XPBar percent to Current / Needed (0 when Needed <= 0) and LevelText to "Lv " + Level; OnLevelUp(NewLevel) sets LevelUpText Visible, a Delay of 1 s, then Collapsed (a second level-up inside 1 s hides it early); OnGameOver sets GameOverText Visible; OnUpgradeOfferShown(Cards, bCanReroll) sets UpgradeScreen Visible, RerollButton enabled = bCanReroll, then functions SetCard0..2(Cards): if Cards has that index, CardSize Visible, the three texts from DisplayName / TargetText / ValueText and a switch on Tier sets CardBorder colour (grey 0.55 grey, green 0.2/0.8/0.25, blue 0.2/0.45/1, purple 0.65/0.25/0.9, yellow 1/0.85/0.1), else CardSize Collapsed; OnUpgradeOfferClosed sets UpgradeScreen Collapsed; OnClicked of CardButton0..2 calls ChooseUpgrade(0..2), of RerollButton RequestReroll. Display only, no gameplay logic (the single reroll rule is in C++, [[sys-level-up]]).
- logic_in_bp: display only (four event graphs above)
- assets: none.
- owner_agent: agent-bp
- verified: task-014-04 PIE: cards shown and game frozen, reroll once then button disabled, card click applied the upgrade and resumed the game; compiled (warnings as errors); graphs written with write_graph_dsl; PIE of 12 s on L_ArenaM1 ran with no divide-by-zero or script warning in the log (task-012-03); display not viewed on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-hud]] — the C++ base class and events.
- [[sys-level-up]] — the offer events and calls the card screen uses.
- [[bp-rr-player-controller]] — creates it through HUDWidgetClass.
