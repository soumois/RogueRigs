---
id: wbp-intermission
description: "WBP_Intermission, placeholder recap screen shown between run levels (title, ore recap, reserved shop area, Continue button); open for its tree and bindings."
parent: blueprints-ui-index
kind: doc
---
# WBP_Intermission

- asset_path: /Game/RogueRigs/UI/WBP_Intermission
- parent_class: URRIntermissionWidget (/Script/RogueRigs.RRIntermissionWidget, [[sys-run-transition]])
- purpose: full-screen recap over the paused game after an extraction on levels 1 to 3: finished level, next level, ore secured this level and in the run, a reserved shop area and the Continue button. Created and removed by BP_RRPlayerController (IntermissionWidgetClass).
- components: CanvasPanel Root > Backdrop (Border, full screen, BrushColor black alpha 0.85) and Content (VerticalBox, anchors 0.5,0.5, alignment 0.5,0.5, auto size) holding: Title (64, white, outline 3, centred), Subtitle (30, teal 0/0.85/1, centred, padding top 8 bottom 32), RecapPanel (Border 0.08/0.09/0.11 alpha 0.9, padding 32/20) > RecapBox (VerticalBox) with OreLevelText and OreRunText (36, white, outline 2) and RecapReserve (SizeBox 640 x 80, empty: room for the later DPS per weapon and robots killed, no fake data), ShopPanel (same border, padding 32/56, vertical padding 24) > ShopLabel ("Boutique — bientôt", 24, grey 0.6, no items), ContinueButton (centred) > ContinueText ("Continuer", 32 white). Font Roboto Bold everywhere, as WBP_AlertObjectives; the end screen of that widget uses white and the teal 0/0.85/1 (ShuttleText). Variables: Title, Subtitle, OreLevelText, OreRunText, ContinueButton. Editor texts are placeholders replaced by OnRecapShown.
- exposed_defaults: none.
- events: OnRecapShown(InRecap) (C++ BlueprintImplementableEvent) breaks the recap and sets Title "Niveau N terminé" (FinishedLevelIndex), Subtitle "Niveau suivant : NextLevelIndex / LevelCount", OreLevelText "Minerai sécurisé ce niveau : OreSecuredThisLevel", OreRunText "Minerai du run : RunSecuredOre". EventConstruct assigns ContinueButton OnClicked to custom event OnClicked_Event, which calls the C++ Continue() on self (return value unused).
- logic_in_bp: display only (text formatting, one call to C++).
- assets: none.
- owner_agent: agent-bp
- verified: task-206-04: compiled with warnings as errors, saved; tree, properties and graph read back (Break node outputs wired). Not seen on screen or in PIE: layout, text fit, click and the Continue flow are unverified. NextLayoutName is not shown.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-run-transition]] — the C++ events, the recap struct and Continue.
- [[bp-rr-player-controller]] — holds IntermissionWidgetClass and creates the widget.
- [[wbp-alert-objectives]] — font and palette reference.
