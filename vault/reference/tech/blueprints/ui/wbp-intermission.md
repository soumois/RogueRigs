---
id: wbp-intermission
description: "WBP_Intermission, the end-of-level stats screen (recap, kills per robot type, damage per weapon, Next level button), texts from screen_texts.json; open for its tree and bindings."
parent: blueprints-ui-index
kind: doc
---
# WBP_Intermission

- asset_path: /Game/RogueRigs/UI/WBP_Intermission
- parent_class: URRIntermissionWidget (/Script/RogueRigs.RRIntermissionWidget, [[sys-run-transition]])
- purpose: full-screen stats screen over the paused game after every extraction (task-317-04): finished level, next level, ore secured this level and run, robots destroyed per type, damage per weapon, and the Next level button. The shop moved to [[wbp-shop-screen]]. On level 4 (bRunWon) the button reads "Cash out" and leads to the run-won screen. Created and removed by BP_RRPlayerController (IntermissionWidgetClass).
- components: CanvasPanel Root > Backdrop (Border, black alpha 0.85, unchanged: the paused level shows through) and Content (VerticalBox, anchors 0.5,0.5, alignment 0.5,0.5, auto size) holding: Title (48, outline 3), Subtitle (24 teal), RecapPanel (Border, padding 28/8) > RecapBox = OreLevelText, OreRunText (26, outline 2), StatsRow (HorizontalBox, slot padding top 16 bottom 6, centred) of KillsBox and DamageBox (VerticalBox, slot padding 40 each side, top aligned) each = Label (KillsLabel, DamageLabel: 28, grey 0.9, outline 2) + List (KillsList, DamageList: 24, outline 2, left, one line per entry "Name: value"); then ContinueButton (slot padding top 20) > ContinueText (32). Font Roboto Bold. Variables: Title, Subtitle, OreLevelText, OreRunText, KillsLabel, KillsList, DamageLabel, DamageList, ContinueButton, ContinueText, plus String KillsStr and DamageStr (loop accumulators). Removed: ShopPanel, five ShopCards, balance row, reroll button, RecapReserve. Estimated height about 600 of 1080 with 8 lines per list.
- exposed_defaults: none. Tree defaults are English placeholders.
- events: EventConstruct assigns ContinueButton OnClicked to a custom event that calls the C++ `NextLevel()`. OnRecapShown(InRecap): breaks the recap struct and GetScreenText("intermission_recap") ([[sys-text-content]]); OreLevelText and OreRunText from Lines[0..1] ({ORE}), KillsLabel = Labels[6], DamageLabel = Labels[7]. If bRunWon: GetScreenText("intermission_run_won") gives Title, Subtitle and ContinueText = Labels[0]. Else Title (Replace {N} by FinishedLevelIndex), Subtitle ({NEXT}, {TOTAL}) and ContinueText = Labels[8]. Two ForEach loops over KillsByRobotType and DamageByWeapon append "DisplayName: Count" and "DisplayName: Round(Damage)" plus a newline to KillsStr and DamageStr, then set the list texts.
- logic_in_bp: display only (text formatting, string accumulation for the lists, one call to C++: NextLevel).
- assets: none.
- owner_agent: agent-bp
- verified: task-317-04: CompileWidgetBlueprint true, compile_blueprint with warnings as errors no error, no LogBlueprint error, saved; tree read back with GetWidgetDescription, graph pins read back (Break outputs connected to the expected consumers, newline pin A = real newline). Not run in PIE, nothing seen: layout fit at 1080p, the list texts with real data, the button leading to the shop screen or the run-won screen, an empty list shows nothing. Earlier: task-316-04 shop version, task-210-03 English texts, task-206-04 first version.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-run-transition]] — the C++ events, the recap struct and NextLevel.
- [[sys-text-content]] — the text subsystem and screen_texts.json.
- [[bp-rr-player-controller]] — holds IntermissionWidgetClass and creates the widget.
- [[wbp-shop-screen]] — the shop screen the Next level button leads to (levels 1 to 3).
- [[wbp-alert-objectives]] — font and palette reference.
