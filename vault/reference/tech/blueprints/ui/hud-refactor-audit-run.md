---
id: hud-refactor-audit-run
description: "HUD audit table part 2: level-up cards, alert bar and icons, horde banner, objective, hack, shuttle, end screen, minimap, locator arrow, events and placement."
parent: hud-refactor-audit
kind: note
---
# HUD audit, part 2
Fields as in [[hud-refactor-audit-core]]. Only the minimap and the locator are read in the BP tick; the alert icons are recomputed each frame in C++ (RefreshAlertBarEvents).

- Level-up cards: UpgradeScreen (z 10, hidden) > Backdrop, Panel > CardsRow of CardSize0..2 (260x230) with CardBorder, CardName, CardTarget, CardValue, CardDesc (flavor or weapon description), RerollButton | OnUpgradeOfferShown(Cards, bCanReroll) calls SetCard0..2, StyleCards (ApplyCardFlavors last); OnUpgradeOfferClosed; clicks call ChooseUpgrade, RequestReroll | none | exists: URRLevelUpComponent OnUpgradeOfferShown, OnUpgradeOfferClosed | no | anchors 0,0-1,1, offsets 0, z 10; Panel centred, auto size | WBP_UpgradeChoice.
- Alert bar and label: AlertBar (-300,48, 600x36, top-centre anchor), AlertLabel (-310,44, right aligned, auto) | OnAlertChanged(Alert, bPaused) | none | exists: URRRunFlowSubsystem::OnAlertChanged | no | as listed | WBP_AlertObjectives.
- Alert icons: AlertIconConsole, Chest, Shuttle (-316,82), AlertIconHorde0..3 (-316,0), z 3, hidden | OnAlertEventsChanged(Events) calls PlaceIcon (translation Position x 600) | none in BP (C++ NativeTick recomputes the list, calls the event on change) | missing: URRRunFlowSubsystem OnAlertBarEventsChanged | no (a pending horde position drifts while the bar fills; C++ tolerance 0.002) | as listed | WBP_AlertObjectives.
- Horde banner: HordeBanner (0.5/0.5, 0,-220, z 5, hidden) | OnHordeIncoming(SecondsUntil), Delay | none | exists: ARREnemySpawner::OnHordeIncoming(HordeIndex, SecondsUntil), reached only through the HUD BindToSpawner; missing: run-flow relay | no | as listed | WBP_AlertObjectives.
- Objective and hack texts: ObjectiveText (-400,140, 800x27), HackBar (-150,174, 300x14), var bHackDone | OnObjectiveChanged(bActive, State, RocksLeft, RocksTotal), OnObjectiveProgressChanged(Progress, bHacking), OnExtractionReady | none | exists on URRRunFlowSubsystem: the same three | no | top-centre anchor, hidden | WBP_AlertObjectives.
- Shuttle text: ShuttleText (-300,196, 600x40, hidden) | OnShuttleCountdown(Remaining, ShuttleLocation) | none | exists: URRRunFlowSubsystem::OnShuttleCountdown (fires every countdown frame; the text only needs whole seconds) | no | as listed | WBP_AlertObjectives.
- End screen: EndScreen (z 20, hidden), EndTitle, RestartButton | OnRunEnded(bSuccess), RestartLevel | none | exists: URRRunFlowSubsystem::OnRunEnded | no | anchors 0,0-1,1, z 20 | WBP_AlertObjectives.
- Minimap and markers: MinimapImage (200x200 at 20,20, z 6, M_Minimap), MarkerConsole, MarkerChest, MarkerShuttle (z 7), MarkerMiner (z 8), 10x10 at 120,120 | OnMinimapTextureReady once; UpdateMinimap each tick | yes: GetMinimapState (twice), 2 material parameter sets, 3 PlaceMarker | texture: exists (the C++ HUD creates it); marker presence: OnConsoleDropped, OnBonusChestDropped, OnShuttleLanded exist | yes: the disc scrolls with the miner (C++ Minimap->Tick also per frame) | as listed | WBP_MinimapLocator.
- Locator arrow and distance: LocatorArrow (Image 64x64, alignment 0.5, z 5, M_LocatorArrow), LocatorDistance (text, z 5) | UpdateLocator_0 each tick | yes: GetObjectiveLocator (target, edge position, angle, projection) | the target change is event-able (missing: OnLocatorTargetChanged) | yes while a target exists (camera and miner move) | anchor 0,0, RenderTranslation = EdgePosition | WBP_MinimapLocator.
- Subtitles: Subtitles (WBP_Subtitles) | its own C++ delegates | none | exists: URRSubtitleSubsystem | no | anchor 0.5/1, 0,-50, 900x80 | already a sub-widget.

## Relations
- [[hud-refactor-audit]] — parent, summary and sub-widget list.
- [[hud-refactor-audit-core]] — part 1.
- [[hud-refactor-missing-events]] — the "missing" items.
