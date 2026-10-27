---
id: sys-aim-indicator
description: "Aim chevron decal component of the miner: API, M_AimChevrons parameter contract and asset settings, hiding rules, stop-at-rock query. Open to change the aim indicator."
parent: systems-combat-index
kind: doc
---
# Aim indicator

- responsibility: draw the [[aim-indicator]]: a decal projected straight down at the miner's feet whose material draws a row of white-gold chevrons along the aim, stopping at the first rock block. Pure visual: no collision, no gameplay effect.
- main_classes: `Source/RogueRigs/AimIndicator/RRAimIndicatorComponent.h/.cpp`: `URRAimIndicatorComponent` (derives from `UDecalComponent`, Blueprint-spawnable). Default subobject `AimIndicator` of `ARRMiner`, attached to the capsule; `ARRMiner::GetAimIndicator()` (BlueprintPure). Absolute transform, rotation built from axes (not a pitch -90 rotator).
- entry_points: `RefreshIndicator()` (each tick: hides without a living `ARRMiner` or material, else places, writes `VisibleLength`, shows), `HideIndicator()`, `IsIndicatorShown()`, `GetFixedLength()`, `GetVisibleLength()`, `GetIndicatorMaterialInstance()`. `BeginPlay` loads the material, creates the dynamic instance, writes the parameters and warms up via `ARRDecalWarmUp::WarmUp` ([[sys-ground-telegraph]]).
- placement: ticks at `TG_PostPhysics` (after the miner turned and moved); at the miner's feet, local +Z (UV.x) along `ARRMiner::GetAimDirection`, projected straight down. The box starts at the miner and runs 2 x HalfLength forward.
- stop-at-rock: `VisibleLength` = min(fixed length, `ARRArenaGrid::GetClearDistance(miner location, aim, fixed length)`), the distance to the first block cell on the aim line, any block type (ore and barrels included). No grid: the fixed length. The material skips a chevron whose far tip `FirstOffset + i x Spacing` is beyond `VisibleLength` ([[sys-grid]]).
- fixed length: 416 uu = `FirstOffset + (ChevronCount - 1) x Spacing` (120 + 4 x 74). Owner's value (2026-10-27, "La longueur est bonne"), no longer only a derivation.
- material contract (`/Game/RogueRigs/FX/M_AimChevrons`, task-328-02, bp commits e8204d64 and d2b9c630): Deferred Decal, Translucent, Unlit, decal response None; one Custom HLSL node. The HLSL weights chevron i by Decay^i (`a *= Dec` per chevron), carried on `MP_Opacity` via `Multiply_0` (A = Custom, B = `Opacity`). `MP_EmissiveColor` = `Multiply_1` (A = `Color` RGB, B = `EmissiveGain`), about (0.5, 0.42, 0.2): same hue at half intensity (task-329-02, bp commit 30ab5343). Cause fixed (probable, from the owner's screenshot, not measured): fixed exposure -0.94 EV100 (`PPV_XPGlow`) plus tonemapping pushed full-strength emissive near white, so the 78/61/47/37 % chevrons looked almost as bright as the first. No brightness compensation for [[mpp-player-light-mask]] (owner: let it fade). Parameters (name, default):
  - `ChevronCount` 5 (loop capped at 16), `FirstOffset` 120 uu, `Spacing` 74 uu, `OpeningDeg` 60 (full angle between the arms), `ArmLength` 40 uu, `LineWidth` 12 uu, `Decay` 0.78, `Color` linear (1.0, 0.84, 0.40), `Opacity` 1.
  - `HalfLength` 500 uu (box half length), `VisibleLength` 1000 uu (default; written each tick by C++).
  - `GlowWidth` 10, `GlowOpacity` 0.35: material defaults, not pushed by C++.
  - `HalfWidth` 64 uu: extra parameter; the material needs the box width in uu.
  - `EmissiveGain` 0.5: provisional, material default, not pushed by C++.
- data: component defaults (`EditDefaultsOnly`) equal the pushed values: ChevronCount 5 (1 to 16), FirstOffset 120, Spacing 74, OpeningDeg 60, ArmLength 40, LineWidth 12, Decay 0.78, Color, Opacity 1, HalfLength 500, HalfWidth 64, `ProjectionHalfDepth` 256. `DefaultMaterialPath` (config `[/Script/RogueRigs.RRAimIndicatorComponent]`, default the asset above), used when `DecalMaterial` is not set. Values are provisional ([[aim-indicator]]).
- hiding: `ARRMiner::HandleDeath` calls `HideIndicator()`; the tick keeps it hidden while the miner `IsDead`. Shown whenever the miner is alive, mouse and stick aim alike.
- depends_on: [[sys-player]] (miner, aim direction), [[sys-grid]] (`GetClearDistance`), [[sys-ground-telegraph]] (`ARRDecalWarmUp`).
- implements: [[aim-indicator]].
- gotchas: a missing material asset logs one warning, nothing is drawn and the tick stops. The owner saw the fade barely visible; the `EmissiveGain` 0.5 fix is provisional, not verified on screen, awaiting his judgement in the open editor.  The row stops at any block, not only rock.

## Relations
- [[systems-combat-index]] — parent index.
- [[aim-indicator]] — the GDD feature it implements.
- [[sys-player]] — the miner that owns the component and hides it at death.
- [[sys-ground-telegraph]] — same decal technique and the warm-up it reuses.
- [[sys-grid]] — block cell query that stops the row.
- [[mpp-player-light-mask]] — darkening the far chevrons fade with.
