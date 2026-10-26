---
id: wbp-intermission
description: "WBP_Intermission, recap and shop screen between run levels (compact recap, 5 WBP_ShopCard, ore balance, reroll, Continue), texts from screen_texts.json; open for its tree and bindings."
parent: blueprints-ui-index
kind: doc
---
# WBP_Intermission

- asset_path: /Game/RogueRigs/UI/WBP_Intermission
- parent_class: URRIntermissionWidget (/Script/RogueRigs.RRIntermissionWidget, [[sys-run-transition]])
- purpose: full-screen screen over the paused game after an extraction on levels 1 to 3: compact recap (finished level, next level, ore secured this level and run), the intermission shop ([[sys-run-shop]], task-316-04: 5 priced stat cards, ore balance, paid reroll) and the Continue button. Created and removed by BP_RRPlayerController (IntermissionWidgetClass).
- components: CanvasPanel Root > Backdrop (Border, black alpha 0.85) and Content (VerticalBox, anchors 0.5,0.5, alignment 0.5,0.5, auto size) holding, top to bottom for 1920 x 1080: Title (48, outline 3), Subtitle (24 teal, padding top 2 bottom 10), RecapPanel (Border, padding 28/8) > RecapBox with OreLevelText and OreRunText (26, outline 2) and RecapReserve (Collapsed, kept), ShopPanel (same border, padding 16/10, slot padding 12) > ShopBox (VerticalBox) = ShopLabel (28, 0.9 grey, title "Salvage Stall"), CardsRow (HorizontalBox, centred, top 8) of ShopCard0..4 (WBP_ShopCard, [[wbp-shop-card]], SlotIndex 0..4, slot padding 8 left and right; 5 x 346 = 1730 px), BalanceRow (HorizontalBox, padding 8/12/8) = OreIcon (Image 40 x 40, T_Icon_Ore) + OreBalanceText (36) + BalanceSpacer (SizeBox, slot Fill) + RerollButton > RerollRow (padding 24/6) = RerollText (24, "Reroll stock") + RerollOre (Image 32 x 32, T_Icon_Ore, padding left 12 right 6) + RerollCost (24, default 15), as the reroll button of [[wbp-upgrade-choice]]; then ContinueButton > ContinueText (32). Font Roboto Bold. Variables: Title, Subtitle, OreLevelText, OreRunText, ShopLabel, ContinueText, ContinueButton, ShopCard0..4, OreIcon, OreBalanceText, RerollButton, RerollText, RerollOre, RerollCost. Tree defaults are the English strings. Estimated height about 960 of 1080.
- exposed_defaults: none (SlotIndex is set on each placed ShopCard).
- events: OnRecapShown(InRecap) as before (recap texts and ShopLabel = Labels[0], ContinueText = Labels[1]). OnShopRefreshed (C++ event, fired by URRIntermissionWidget on every OnShopChanged) calls RefreshShop. EventConstruct assigns ContinueButton OnClicked to OnClicked_Event (C++ Continue()) and RerollButton OnClicked to RerollClicked_Event (calls RequestReroll on GetShop), then calls RefreshShop once (the offer exists) and LoadShopTexts. RefreshShop: shop = GetShop; OreBalanceText = ToText(GetOreBalance); RerollCost = ToText(GetRerollCost); RerollButton SetIsEnabled(CanReroll), so a disabled button greys text, icon and cost together. LoadShopTexts: GetScreenText("intermission_recap"), RerollText = Labels[5]. The cards refresh and buy by themselves; the parent never calls BuySlot. A few empty custom events (OnClicked_Event_0..3) remain, harmless.
- logic_in_bp: display only (text formatting, button state from C++ booleans, two calls to C++: Continue, RequestReroll).
- assets: none.
- owner_agent: agent-bp
- verified: task-316-04: compiled with warnings as errors (compile_blueprint and CompileWidgetBlueprint) and saved; tree, properties, RefreshShop and LoadShopTexts pins, EventGraph (Construct chain, delegates, OnShopRefreshed, reroll event) read back. Not run in PIE, nothing seen: layout fit at 1080p, card look in the row, balance and reroll updates, greyed reroll, click behaviour. Earlier: task-210-03 English texts, task-206-04 first version.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-run-transition]] — the C++ events, the recap struct and Continue.
- [[sys-text-content]] — the text subsystem and screen_texts.json.
- [[bp-rr-player-controller]] — holds IntermissionWidgetClass and creates the widget.
- [[wbp-shop-card]] — the five cards it hosts.
- [[sys-run-shop]] — the shop API it reads.
- [[wbp-upgrade-choice]] — reroll button look copied.
- [[wbp-alert-objectives]] — font and palette reference.
