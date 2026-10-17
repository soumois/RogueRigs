---
id: wbp-loading
description: "WBP_Loading, full-screen opaque loading screen shown between levels (Chargement, Niveau N, stage line), bound to the C++ loading events; open for its tree and graph."
parent: blueprints-ui-index
kind: doc
---
# WBP_Loading

- asset_path: /Game/RogueRigs/UI/WBP_Loading
- parent_class: URRLoadingScreenWidget (/Script/RogueRigs.RRLoadingScreenWidget, [[sys-run-transition]])
- purpose: hides the map being built during a level change: opaque dark screen, "Chargement", "Niveau N" and a stage line. Created by the controller (class in LoadingScreenWidgetClass of [[bp-rr-player-controller]]) when loading starts, again on the new world's BeginPlay, removed on OnLevelReady.
- components: Border Backdrop (root, brush colour 0.02/0.025/0.035 alpha 1, fills the screen) > VerticalBox Column (centred both ways) > TitleText "Chargement" (Roboto Bold 64, white, outline 2), LevelText "Niveau 1" (Bold 36, orange 1/0.35/0.1, outline 2, top padding 16), StageText (Bold 22, grey 0.7, outline 2, top padding 16, initial "Préparation du terrain"). All three texts are variables, centre-justified and aligned. Same font and colour language as [[wbp-alert-objectives]]. No animation.
- exposed_defaults: none.
- events: OnLoadingShown(LevelIndex, LayoutName): LevelText = "Niveau " + LevelIndex (LayoutName unused). OnLoadingStageChanged(Stage): switch on ERRRunLoadingStage: WaitingForLayout "Préparation du terrain", OpeningMap "Chargement de la carte", PreparingLevel "Préparation du niveau"; None leaves the text unchanged.
- logic_in_bp: display only.
- assets: none.
- owner_agent: agent-bp
- verified: task-206-05: tree read back, graph read back, compiled with warnings as errors, saved; LoadingScreenWidgetClass read back on the controller default. Not seen on screen or in PIE; not checked that the Backdrop covers the viewport when added to the viewport.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-run-transition]] — the C++ base class, loading events and stages.
- [[bp-rr-player-controller]] — creates it through LoadingScreenWidgetClass.
- [[wbp-alert-objectives]] — font and colour reference.
