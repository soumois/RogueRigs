---
id: wbp-intermission
description: "WBP_Intermission, the end-of-level stats screen (Total Duskite line, kills per robot type, damage per weapon, Next level button); open for its tree and bindings."
parent: blueprints-ui-index
kind: doc
---
# WBP_Intermission

- asset_path: /Game/RogueRigs/UI/WBP_Intermission
- parent_class: URRIntermissionWidget (/Script/RogueRigs.RRIntermissionWidget, [[sys-run-transition]])
- purpose: full-screen stats screen over the paused game after every extraction (task-317-04): finished level, next level, one Total Duskite line (secured ore balance held after the extraction, purchases and rerolls subtracted, task-391-04), robots destroyed per type, damage per weapon, and the Next level button. The shop moved to [[wbp-shop-screen]]. On level 4 (bRunWon) the button reads "Cash out" and leads to the run-won screen. Created and removed by BP_RRPlayerController (IntermissionWidgetClass).
- components: CanvasPanel Root, direct children only, z order bottom to top (task-381-02, rebuilt to the level-end mockup): Backdrop (Border, black alpha 0.85, the paused level shows through); TitleBarL, TitleBarR (Image T_Shop_TitleBar, right one render scale X -1); SubtitleLineL, SubtitleLineR (Image, 2 px teal 0,0.85,1 alpha 0.8); Frame (Border, T_LevelEnd_Frame); OreRow (HorizontalBox, not a variable, task-399-01) holding in order OreIcon (Image T_Icon_Ore, 88 x 88), OreLabel (Roboto Bold 32, outline 2; design-time Text "Total Duskite:", was "Total Ore:", task-392-04; runtime text from screen_texts.json) and OreValue (Roboto Bold 48, outline 2); DividerH, DividerV (Images); KillsLine, DamageLine (Image 2 px grey 0.75 alpha 0.6); KillsRows, DamageRows (Overlay each holding KillsList / DamageList left and KillsValues / DamageValues right-justified, 24, outline 2); KillsLabel, DamageLabel (28); Title (48, outline 3); Subtitle; ContinueButton > ContinueText (Roboto Bold 28, dark brown, ToUpper). Variables: Title, Subtitle, OreLabel, OreValue, KillsLabel, KillsList, KillsValues, DamageLabel, DamageList, DamageValues, ContinueButton, ContinueText, Strings KillsStr, DamageStr, KillsValStr, DamageValStr. Removed: Content, RecapPanel, RecapBox, StatsRow, KillsBox, DamageBox, OreTexts.
- anchor_rule (task-381-02; fractions measured by eye from the mockup, the owner will probably retouch them; the owner edits by hand; rule [[umg-layout]]). Each Root child (OreRow holds the ore band widgets), anchors min-max / alignment / offsets (L,T,R,B when stretched, else x,y,width,height):
  - Backdrop: (0,0)-(1,1), offsets 0.
  - TitleBarL: anchor (0.5,0.15), align (1,0.5), (-300,0,356,48). TitleBarR: align (0,0.5), (300,0,356,48). Title: (0.5,0.15), align (0.5,0.5), auto. Subtitle: (0.5,0.21), align (0.5,0.5), auto.
  - SubtitleLineL: (0.5,0.21), align (1,0.5), (-300,0,140,2). SubtitleLineR: align (0,0.5), (300,0,140,2).
  - Frame: (0.22,0.242)-(0.78,0.799), offsets 0; Box margins L/R 0.0955, T/B 0.1878 (image 942 x 458).
  - Task-383-02 moved the ore band and every widget below it up by 0.057 (values below are after the move). Task-399-01 (Q1 = A, decided 2026-10-30, values provisional): OreRow, direct child of Root at the former OreIcon index (z order unchanged: above Frame, below DividerH), anchors (0.5,0.31)-(0.5,0.31), align (0.5,0.5), offsets 0, auto size. Slots in order OreIcon, OreLabel, OreValue: horizontal Left, vertical Center, size Automatic; padding OreIcon right 12, OreValue left 16, OreLabel none. Before: OreIcon (0.40,0.31) 88 x 88, OreLabel (0.43,0.31), OreValue (0.525,0.31) as separate canvas slots, which overlapped once task-392-04 lengthened the label. Exception to [[umg-layout]] (a HorizontalBox instead of independent anchors); the rule node is unchanged.
  - DividerH: (0.2273,0.3296)-(0.7727,0.3896); Box margins L 0.069, R 0.066 (984 x 62). DividerV: (0.4845,0.368)-(0.5155,0.775); Box margins T 0.057, B 0.058 (58 x 984).
  - KillsLine: (0.267,0.4637)-(0.472,0.4637), align (0,0.5). DamageLine: (0.528,0.4637)-(0.733,0.4637), align (0,0.5).
  - KillsRows: (0.267,0.488)-(0.472,0.765). DamageRows: (0.528,0.488)-(0.733,0.765). Offsets 0.
  - KillsLabel: (0.267,0.424), align (0,0.5), auto. DamageLabel: (0.528,0.424), same.
  - ContinueButton: (0.5,0.861), align (0.5,0.5), (0,0,416,106); T_Shop_ButtonContinue Box margin 0.155, tints 1.0 / 1.3 hover / 0.7 pressed / 0.35 disabled. ContinueText was 36, 28 after the fit fix (892ae8c1).
- exposed_defaults: none. Tree defaults are English placeholders.
- events: EventConstruct assigns ContinueButton OnClicked to a custom event that calls the C++ `NextLevel()`; two empty custom events OnClicked_Event_0 and _2 are harmless leftovers. OnRecapShown(InRecap): breaks the recap struct and GetScreenText("intermission_recap") ([[sys-text-content]]); OreLabel = Lines[0] ("Total Duskite:", ToString/ToText/SetText), OreValue = RunSecuredOre of the recap struct (the balance held, not a since-run-start total) (ToString/ToText/SetText); Lines[1] is no longer read; KillsLabel = Labels[6], DamageLabel = Labels[7]. If bRunWon: GetScreenText("intermission_run_won") gives Title, Subtitle and ContinueText = Labels[0]. Else Title (Replace {N} by FinishedLevelIndex), Subtitle ({NEXT}, {TOTAL}) and ContinueText = Labels[8]. Two-part lists (task-381-02): the ForEach loops over KillsByRobotType and DamageByWeapon append "Name
" to KillsStr / DamageStr and "count
" / "Round(damage)
" to KillsValStr / DamageValStr; after the loops SetText on the four list texts; both value strings are cleared at event end. The button text is upper case (ToUpper); texts unchanged.
- logic_in_bp: display only (text formatting, string accumulation for the lists, one call to C++: NextLevel).
- assets: textures in /Game/RogueRigs/UI/LevelEnd (task-381-01, commit 3a46713a; group UI, NoMipmaps, sRGB, TC_EditorIcon, NeverStream): T_LevelEnd_Frame, T_LevelEnd_DividerH, T_LevelEnd_DividerV (Box margins in anchor_rule). Reused: T_Shop_TitleBar, T_Shop_ButtonContinue (/Game/RogueRigs/UI/Shop, [[wbp-shop-screen]]), T_Icon_Ore.
- ui_check (task-392-05): other WBP_* under /Game/RogueRigs/UI scanned for player-visible "Ore"; two left, outside the task and reported to the owner: ShopCard ShortLabel "Short on ore" ([[wbp-shop-screen]]) and the EndOreText designer default "Ore secured: 0" in [[wbp-alert-objectives]] (the latter now documented there).
- owner_agent: agent-bp
- verified (task-399): the owner validated the result on screen on 2026-10-30 ("Ok je valide ce que j'ai vu à l'écran"); capture run (Saved/LevelEndScreen/task-399/comparison_sheet.png, not committed) showed the number right of the label at 1920 x 1080, 1280 x 720 and 2560 x 1080; 1920 x 1200 not verified. Earlier, no capture run for task-383-02: the owner validated the screen visually on 2026-10-30 ("Ok je valide l'écran de fin, plus besoin de test"); the BP compiled clean with warnings as errors. Earlier (task-381-03 capture, Saved/LevelEndScreen/task-381/): layout seen at 1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080 and level 4 at 1920 x 1080. Open defect (seen, not fixed): on level 4 the longer subtitle crosses the two fixed-width subtitle lines. Not seen by the crew: render after task-383, label and value spacing at other resolutions, Bold typeface in game, level 4 CASH OUT, hover and pressed tints, a click, a multi-weapon damage list. Earlier: task-317-05, task-317-04.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-run-transition]] — the C++ events, the recap struct and NextLevel.
- [[sys-text-content]] — the text subsystem and screen_texts.json.
- [[bp-rr-player-controller]] — holds IntermissionWidgetClass and creates the widget.
- [[wbp-shop-screen]] — the shop screen the Next level button leads to (levels 1 to 3).
- [[umg-layout]] — the layout rule the anchor_rule follows.
- [[task-381-level-end-screen-rebuild]] — the rebuild to the level-end mockup.
- [[task-383-level-end-total-ore]] — the single Total Ore line and the moved anchors.
- [[task-399-total-duskite-number-overlap]] — the OreRow fix for the label and number overlap.
- [[intermission]] — the gdd look of this screen.
- [[wbp-alert-objectives]] — font and palette reference.
