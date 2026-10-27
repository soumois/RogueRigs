---
id: sys-aim-indicator
description: "Aim chevron decal component of the miner: API, M_AimChevrons parameter contract and asset settings, hiding rules, stop-at-rock query. Open to change the aim indicator."
parent: systems-combat-index
kind: doc
---
# Aim indicator

- responsibility: draw the [[aim-indicator]]: a decal projected straight down at the miner's feet whose material draws a row of white-gold chevrons along the aim, stopping at the first rock block. Pure visual: no collision, no gameplay effect, no link to any weapon, turret placement or the drone (task-328-03).
- main_classes: `Source/RogueRigs/AimIndicator/RRAimIndicatorComponent.h/.cpp`: `URRAimIndicatorComponent` (derives from `UDecalComponent`, Blueprint-spawnable). Default subobject `AimIndicator` of `ARRMiner`, attached to the capsule; `ARRMiner::GetAimIndicator()` (BlueprintPure). Absolute transform, rotation built from axes (not a pitch -90 rotator).
- entry_points: `RefreshIndicator()` (called each tick; hides when the owner is not a living `ARRMiner` or there is no material, else places, writes `VisibleLength` and shows), `HideIndicator()`, `IsIndicatorShown()`, `GetFixedLength()`, `GetVisibleLength()`, `GetIndicatorMaterialInstance()`. Static `FName` parameter names, one per material parameter below. `BeginPlay` sizes the box, loads the material, creates the dynamic instance, writes the parameters, warms the material up through `ARRDecalWarmUp::WarmUp` ([[sys-ground-telegraph]]) and places the decal once.
- placement: ticks at `TG_PostPhysics` (after the miner turned and moved); located at the miner's feet, local +Z (material forward, UV.x) along `ARRMiner::GetAimDirection`, projection axis straight down. The box starts at the miner (UV.x = 0) and runs 2 x HalfLength forward; lateral UV.y is centred at 0.5.
- stop-at-rock: `VisibleLength` = min(fixed length, `ARRArenaGrid::GetClearDistance(miner location, aim, fixed length)`), the distance to the first block cell on the aim line, any block type (ore and barrels included). No grid: the fixed length. The material skips a chevron whose far tip `FirstOffset + i x Spacing` is beyond `VisibleLength` ([[sys-grid]]).
- fixed length: 416 uu = `FirstOffset + (ChevronCount - 1) x Spacing` (120 + 4 x 74). It is the cpp agent's derivation from the row, not an owner-decided value.
- material contract (`/Game/RogueRigs/FX/M_AimChevrons`, task-328-02, bp commits e8204d64 and d2b9c630): Deferred Decal, Translucent, Unlit, decal response None; one Custom HLSL node. Chevron i is drawn at opacity Decay^i. Emissive is plain `Color`, no brightness compensation for [[mpp-player-light-mask]] (owner: let it fade). Parameters (name, default):
  - `ChevronCount` 5 (loop capped at 16), `FirstOffset` 120 uu, `Spacing` 74 uu, `OpeningDeg` 60 (full angle between the arms), `ArmLength` 40 uu, `LineWidth` 12 uu, `Decay` 0.78, `Color` linear (1.0, 0.84, 0.40), `Opacity` 1.
  - `HalfLength` 500 uu (box half length), `VisibleLength` 1000 uu (default; written each tick by C++).
  - `GlowWidth` 10, `GlowOpacity` 0.35: material defaults, not pushed by C++.
  - `HalfWidth` 64 uu: extra parameter, not in the task contract; the material needs the box width in uu.
- data: component defaults (`EditDefaultsOnly`) equal the pushed values: ChevronCount 5 (1 to 16), FirstOffset 120, Spacing 74, OpeningDeg 60, ArmLength 40, LineWidth 12, Decay 0.78, Color, Opacity 1, HalfLength 500, HalfWidth 64, `ProjectionHalfDepth` 256. `DefaultMaterialPath` (config `[/Script/RogueRigs.RRAimIndicatorComponent]`, default the asset above), used when `DecalMaterial` is not set. Values are provisional ([[aim-indicator]]).
- hiding: `ARRMiner::HandleDeath` calls `HideIndicator()`; the tick keeps it hidden while the miner `IsDead`. Shown whenever the miner is alive, mouse and stick aim alike.
- depends_on: [[sys-player]] (miner, aim direction), [[sys-grid]] (`GetClearDistance`), [[sys-ground-telegraph]] (`ARRDecalWarmUp`).
- implements: [[aim-indicator]].
- gotchas: a missing material asset logs one warning, nothing is drawn and the tick stops. Look and orientation of the material were not verified on screen (no capture). A decal is a scene component: no collision or navigation. The row stops at any block, not only rock. Tests: 271 pass, 2 old unrelated failures at the build of task-328-03.

## Relations
- [[systems-combat-index]] — parent index.
- [[aim-indicator]] — the GDD feature it implements.
- [[sys-player]] — the miner that owns the component and hides it at death.
- [[sys-ground-telegraph]] — same decal technique and the warm-up it reuses.
- [[sys-grid]] — block cell query that stops the row.
- [[mpp-player-light-mask]] — darkening the far chevrons fade with.
