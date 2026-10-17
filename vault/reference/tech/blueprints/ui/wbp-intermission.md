---
id: wbp-intermission
description: "WBP_Intermission, recap screen shown between run levels (title, ore recap, shop area, Continue), English texts from screen_texts.json; open for its tree and bindings."
parent: blueprints-ui-index
kind: doc
---
# WBP_Intermission

- asset_path: /Game/RogueRigs/UI/WBP_Intermission
- parent_class: URRIntermissionWidget (/Script/RogueRigs.RRIntermissionWidget, [[sys-run-transition]])
- purpose: full-screen recap over the paused game after an extraction on levels 1 to 3: finished level, next level, ore secured this level and in the run, a reserved shop area and the Continue button. Created and removed by BP_RRPlayerController (IntermissionWidgetClass).
- components: CanvasPanel Root > Backdrop (Border, full screen, BrushColor black alpha 0.85) and Content (VerticalBox, anchors 0.5,0.5, alignment 0.5,0.5, auto size) holding: Title (64, white, outline 3, centred), Subtitle (30, teal 0/0.85/1, centred, padding top 8 bottom 32), RecapPanel (Border 0.08/0.09/0.11 alpha 0.9, padding 32/20) > RecapBox (VerticalBox) with OreLevelText and OreRunText (36, white, outline 2) and RecapReserve (SizeBox 640 x 80, empty: room for the later DPS per weapon and robots killed, no fake data), ShopPanel (same border, padding 32/56, vertical padding 24) > ShopLabel ("Shop opens later", 24, grey 0.6, no items), ContinueButton (centred) > ContinueText ("Continue", 32 white). Font Roboto Bold everywhere, as WBP_AlertObjectives; the end screen of that widget uses white and the teal 0/0.85/1 (ShuttleText). Variables: Title, Subtitle, OreLevelText, OreRunText, ShopLabel, ContinueText, ContinueButton. Tree defaults are the English strings ("LEVEL 1 CLEARED", "Next claim: level 2 of 4", "Ore secured this level: 0", "Ore secured this run: 0"), shown in the designer and kept when the text file is missing.
- exposed_defaults: none.
- events: OnRecapShown(InRecap) (C++ BlueprintImplementableEvent) breaks the recap, gets URRTextContentSubsystem (Get World Subsystem) and calls GetScreenText("intermission_recap", [[sys-text-content]]). If it returns true (Branch; false keeps the tree defaults): Title = Title with "{N}" replaced by FinishedLevelIndex; Subtitle = Subtitle with "{NEXT}" by NextLevelIndex and "{TOTAL}" by LevelCount; OreLevelText = Lines[0] with "{ORE}" by OreSecuredThisLevel; OreRunText = Lines[1] with "{ORE}" by RunSecuredOre; ShopLabel = Labels[0]; ContinueText = Labels[1]. Each replace is a literal String Replace on ToString(Text), then ToText(String). EventConstruct assigns ContinueButton OnClicked to custom event OnClicked_Event, which calls the C++ Continue() on self (return value unused). An empty stray custom event OnClicked_Event_3 remains, harmless.
- logic_in_bp: display only (text formatting, one call to C++).
- assets: none.
- owner_agent: agent-bp
- verified: task-210-03: compiled with warnings as errors, saved; tree defaults and node wiring (pins of each Replace, array index, break outputs, exec chain) read back. Not run in PIE and not seen on screen: the runtime texts, layout and text fit are unverified. NextLayoutName is not shown. Earlier: task-206-04 built the French version.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-run-transition]] — the C++ events, the recap struct and Continue.
- [[sys-text-content]] — the text subsystem and screen_texts.json.
- [[bp-rr-player-controller]] — holds IntermissionWidgetClass and creates the widget.
- [[wbp-alert-objectives]] — font and palette reference.
