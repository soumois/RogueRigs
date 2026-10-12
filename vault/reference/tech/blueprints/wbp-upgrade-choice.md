---
id: wbp-upgrade-choice
description: "WBP_UpgradeChoice, HUD panel for the level-up cards and the reroll, driven by the level-up component offer events; open for its tree and graphs."
parent: blueprints-index
kind: doc
---
# WBP_UpgradeChoice

- asset_path: /Game/RogueRigs/UI/WBP_UpgradeChoice
- parent_class: URRHUDPanelWidget (/Script/RogueRigs.RRHUDPanelWidget, [[sys-hud-events]])
- purpose: shows the three upgrade cards and the reroll button; a sub-widget of [[wbp-hud]], which only places it (variable UpgradeChoice, last child of Root, anchors 0,0 to 1,1, zero offsets, z-order 10, Visibility SelfHitTestInvisible: the instance itself takes no clicks, its Backdrop and buttons do, as UpgradeScreen did).
- components: CanvasPanel Root > UpgradeScreen (CanvasPanel, full screen, Collapsed until an offer) = Backdrop (Border, black alpha 0.7) + Panel (VerticalBox, centred, auto size): TitleText "Choose an upgrade" (36), CardsRow (HorizontalBox) of three SizeBox CardSize0..2 (260 x 230, padding 12 left and right) > CardBorder<i> (Border, padding 6, tier colour) > CardButton<i> > CardBox<i> (VerticalBox) = CardName (26), CardTarget (18), CardValue (30), CardDesc (16, Collapsed), all Roboto Bold, centred, wrapped; then RerollButton > RerollText "Reroll" (24). Same values as in WBP_HUD (task-094-06); every card widget, RerollButton and UpgradeScreen are variables.
- events: hook BindToMiner(Miner, PreviousMiner): UnbindCards(Previous); valid miner: assigns the level-up component's OnUpgradeOfferShown and OnUpgradeOfferClosed to custom events (editor names OnUpgradeOfferShown_Event_0, OnUpgradeOfferClosed_Event_0) and, when IsAwaitingChoice, calls ShowCards(GetOfferCards, CanReroll) (bind-time read); otherwise CloseCards. Null miner: CloseCards. Offer shown calls ShowCards; closed calls CloseCards. Construct assigns the OnClicked of CardButton0..2 and RerollButton to custom events (OnClicked_Event .. _2) that call ChooseCard(0..2) and RerollCards (the old component-bound OnClicked events could not be written through the DSL). Destruct calls UnbindCards(GetMiner).
- functions: ShowCards(Cards, bCanReroll): UpgradeScreen Visible, RerollButton enabled = bCanReroll, then per index 0..2 SetCard + StyleCard when the card exists else CardSize Collapsed. SetCard(Card, Size, Border, Name, Target, Value): CardSize Visible, texts from DisplayName / TargetText / ValueText, border colour from Tier (grey 0.55, green 0.2/0.8/0.25, blue 0.2/0.45/1, purple 0.65/0.25/0.9, yellow 1/0.85/0.1). StyleCard(Card, Border, Desc, Value): StatUpgrade = Desc Collapsed, Value Visible, then ShowCardFlavor; NewWeapon = teal-white border (0, 0.9, 0.85), Desc shows DescriptionText, Value Collapsed. ShowCardFlavor(Card, Desc): for a StatUpgrade sets Desc to CardFlavorText and makes it Visible when non-empty. CardFlavorText(Description): the text after the first newline (ParseIntoArray with one real newline; empty when none). CloseCards: UpgradeScreen Collapsed. ChooseCard(Index) / RerollCards: GetMiner > GetLevelUp > ChooseUpgrade / RequestReroll. UnbindCards(M): unbinds all offer listeners of a valid miner's level-up component. The 14 old functions (SetCard0..2, StyleCard0..2, StyleCards, ApplyCardFlavors, GetCardFlavor, ShowFlavor) were merged into these and renamed (a call named GetCardFlavor or ShowFlavor resolved to the WBP_HUD function through MCP).
- selection input: mouse clicks on the buttons only (as before, no key binding); the click calls the miner's URRLevelUpComponent directly instead of URRHUDWidget::ChooseUpgrade / RequestReroll, which still exist in C++.
- tick: none, no timer.
- logic_in_bp: display only.
- assets: none.
- owner_agent: agent-bp
- verified: compiled with warnings as errors, saved, graphs read back, placement read back in WBP_HUD. Not seen on screen or in PIE; binding, clicks, the new-weapon and flavor rendering are unverified. Breaks (for task-094-10): the C++ tests look the widgets up by name in the HUD's own tree (GetWidgetFromName on the HUD) and no longer find them: RRIntegrationTests.cpp (UpgradeScreen, RerollButton, CardName0..2 around lines 1428-1436, 1550, 1691, 1877, 1917) and RRWeaponsIntegrationTests.cpp (UpgradeScreen, CardName<i>, CardDesc<i> around lines 347-399, including Integration.WeaponCardEquipsShotgun). The lookup must go through the UpgradeChoice child widget's own tree. The C++ HUD still binds OnUpgradeOfferShown / Closed and calls its (now unimplemented) Blueprint events.

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class and the binding hooks.
- [[sys-level-up]] — the offer events, ChooseUpgrade and RequestReroll it calls.
