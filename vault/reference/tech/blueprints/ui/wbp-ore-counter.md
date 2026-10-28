---
id: wbp-ore-counter
description: "WBP_OreCounter, HUD panel: run total ore (carried + secured) as icon and number right of the minimap, bound to OnOreChanged; open for tree and binding."
parent: blueprints-ui-index
kind: doc
---
# WBP_OreCounter

- asset_path: /Game/RogueRigs/UI/WBP_OreCounter
- parent_class: URRHUDPanelWidget (/Script/RogueRigs.RRHUDPanelWidget, [[sys-hud-events]])
- purpose: shows the run total ore, carried this level plus secured (what the miner can spend, task-340-02), as "icon then number", no plate, at the top of the HUD right of the round minimap; a sub-widget of [[wbp-hud]], which only places it (variable OreCounter, anchors 0,0 to 1,1, zero offsets, z-order 6, HitTestInvisible). Task-183-04, counting rules in [[sys-objectives-ore-counter]].
- components: CanvasPanel Root > OreIcon (Image 56 x 56 at 236,30, z 1, brush /Game/RogueRigs/UI/Icons/T_Icon_Ore, variable), OreText (TextBlock, Roboto Bold 32, white, outline 3 black, auto size, left 300, top 58 with alignment 0,0.5 so it is centred on the icon row, text "0", variable). Reference screen 1920 x 1080: icon x 236..292, y 30..86; the minimap disc is x 20..220, y 20..220, the health bar y 232..260 and the alert bar x 660..1260, y 48..84, so no overlap.
- exposed_defaults: none.
- events: hook BindToRunFlow(RunFlow, PreviousRunFlow): valid RunFlow: assigns OnOreChanged to a custom event (OnOreChanged_Event_3, params CarriedOre, SecuredOre) and calls ShowOre(GetSpendableOre) once (bind-time value); then, if PreviousRunFlow is valid, unbinds all OnOreChanged from it (same on the not-valid branch). Destruct unbinds all OnOreChanged from GetRunFlow. The custom event calls ShowOre(CarriedOre + SecuredOre). The success screen reads the secured ore separately in [[wbp-alert-objectives]]. Unbind is "unbind all" (Assign nodes, like [[wbp-minimap-locator]]).
- functions: ShowOre(Total int): SetText(OreText, ToText(Total)).
- tick: none, no timer, no polling; the Tick event node the editor adds was deleted.
- logic_in_bp: display only.
- assets: T_Icon_Ore (/Game/RogueRigs/UI/Icons/T_Icon_Ore, 256 x 256 RGBA, UI texture group, no mips, sRGB). Source `Art/UI/Icons/T_Icon_Ore.png`, keyed from the owner-chosen `vault/assets/ore-hud-icon.png` (crystal on #00FF00, no alpha) with the project cut-out tool `Tools/art/ui_icon_prepare.js` run in a green-key variant (corner-colour background, distance key plus a green-excess key, unmix of the background from edge pixels and green despill G -= max(0, G - max(R, B)), then trim and area resample to 256 with a 4 percent margin). The variant (R, B and G swapped in the despill tests) lived in the session scratch folder; the committed tool is the magenta one. Checked: corners alpha 0, no pixel with alpha over 10 near #00FF00, no green excess over 24.
- owner_agent: agent-bp
- verified: compiled with warnings as errors, saved. PIE (task-183-04, Slate snapshot, not an image): after `GrantOre 10` the OreText showed "10" with the icon at the expected position (layout scale 0.857 in the PIE window, positions matched the slot values times that scale). -game capture task-340-03: level 2 HUD counter 7 after 7 ore secured on level 1, 0 after `rr.Run.Restart` (contact sheet in `Saved/OreRunCarry/`, not committed). Not seen as a render: icon crispness and the keyed edges on screen; the text outline look.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class and the binding hooks.
- [[sys-objectives-ore-counter]] — the OnOreChanged event and the getters it uses.
- [[wbp-alert-objectives]] — shows the secured ore on the success screen.
