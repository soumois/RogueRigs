---
id: wbp-shop-screen
description: "WBP_ShopScreen, the cards shop on its own opaque black screen after the stats screen; open for its tree and bindings."
parent: blueprints-ui-index
kind: doc
---
# WBP_ShopScreen

- asset_path: /Game/RogueRigs/UI/WBP_ShopScreen
- parent_class: URRShopScreenWidget (/Script/RogueRigs.RRShopScreenWidget, abstract, [[sys-run-transition]], task-317-01)
- purpose: full-screen shop shown after the stats screen on levels 1 to 3 ([[sys-run-shop]]): title "SPARE PARTS", 5 priced stat cards, ore balance, paid reroll and the Continue button. Created by BP_RRPlayerController (ShopScreenWidgetClass, Z-order 50) on OnShopScreenStarted, removed on OnLoadingStarted. Built in task-317-03 by duplicating [[wbp-intermission]] and removing the recap part (changed to the stats screen in task-317-04).
- components: CanvasPanel Root > Backdrop (Border, black, alpha 1, opaque) and Content (VerticalBox, anchors 0.5,0.5, alignment 0.5,0.5, auto size) holding, top to bottom: Title (TextBlock, 48, outline 3), ShopPanel (Border, padding 16/10) > ShopBox (VerticalBox) = CardsRow (HorizontalBox) of ShopCard0..4 (WBP_ShopCard, [[wbp-shop-card]], SlotIndex 0..4) + BalanceRow (OreIcon T_Icon_Ore + OreBalanceText + BalanceSpacer + RerollButton > RerollRow = RerollText + RerollOre + RerollCost); then ContinueButton > ContinueText. Variables: Title, ShopCard0..4, OreIcon, OreBalanceText, RerollButton, RerollText, RerollOre, RerollCost, ContinueButton, ContinueText. The recap panel, subtitle and "Salvage Stall" label of WBP_Intermission are removed.
- exposed_defaults: none. Tree default of Title is "SPARE PARTS".
- events: EventConstruct assigns ContinueButton OnClicked to custom event ContinueClicked (calls the C++ `Continue()` of URRShopScreenWidget) and RerollButton OnClicked to RerollClicked (calls RequestReroll on GetShop), then calls RefreshShop and LoadShopTexts once. OnShopRefreshed (C++ event, fired on every OnShopChanged) calls RefreshShop. RefreshShop: OreBalanceText = GetOreBalance, RerollCost = GetRerollCost, RerollButton SetIsEnabled(CanReroll). LoadShopTexts: Title = GetScreenTitle("shop_screen"); from GetScreenText("intermission_recap"): ContinueText = Labels[1], RerollText = Labels[5] ([[sys-text-content]]). The cards refresh and buy by themselves.
- logic_in_bp: display only (text, button state from C++ booleans, two calls to C++: Continue, RequestReroll).
- assets: none new (T_Icon_Ore).
- owner_agent: agent-bp
- verified: task-317-03: compiled (CompileWidgetBlueprint true, compile_blueprint warnings as errors, no error) and saved; Backdrop BrushColor a = 1 read back; graphs read back; ShopScreenWidgetClass on BP_RRPlayerController read back. Not run in PIE, nothing seen: layout fit, click behaviour, refresh, that the Self calls (RefreshShop, LoadShopTexts; printed by MCP as Class|WBPIntermission|...) bind to this widget. LoadShopTexts reads GetScreenText twice (a Break node fed by a second call) as WBP_Intermission did.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[wbp-intermission]] — the stats screen it was duplicated from (shop panel removed there in task-317-04).
- [[wbp-shop-card]] — the five cards it hosts.
- [[sys-run-transition]] — the C++ base, events and Continue.
- [[sys-run-shop]] — the shop API it reads.
- [[sys-text-content]] — the texts from screen_texts.json.
- [[bp-rr-player-controller]] — holds ShopScreenWidgetClass and creates the widget.
