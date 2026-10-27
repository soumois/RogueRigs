---
id: sys-aim-indicator
description: "Aim chevron decal component of the miner: API, M_AimChevrons parameter contract and asset settings, hiding rules, stop-at-rock query. Open to change the aim indicator."
parent: systems-combat-index
kind: doc
---
# Aim indicator

- responsibility: draw the [[aim-indicator]]: a decal projected straight down at the miner's feet whose material draws a row of white-gold chevrons along the aim, stopping at the first rock block. Pure visual.
- main_classes: `Source/RogueRigs/AimIndicator/RRAimIndicatorComponent.h/.cpp`: `URRAimIndicatorComponent` (derives from `UDecalComponent`, Blueprint-spawnable). Default subobject `AimIndicator` of `ARRMiner`, attached to the capsule; `ARRMiner::GetAimIndicator()` (BlueprintPure).
- entry_points: `RefreshIndicator()` (each tick: hides without a living `ARRMiner` or material, else places, writes `VisibleLength`, shows), `HideIndicator()`, `IsIndicatorShown()`, `GetFixedLength()`, `GetVisibleLength()`, `GetIndicatorMaterialInstance()`. `BeginPlay` loads the material, creates the dynamic instance, writes the parameters and warms up via `ARRDecalWarmUp::WarmUp` ([[sys-ground-telegraph]]).
- placement: ticks at `TG_PostPhysics` (after the miner turned and moved); at the miner's feet, local +Z (UV.x) along `ARRMiner::GetAimDirection`, projected straight down. The box starts at the miner and runs 2 x HalfLength forward.
- stop-at-rock: `VisibleLength` = min(fixed length, `ARRArenaGrid::GetClearDistance(miner location, aim, fixed length)`), the distance to the first block cell on the aim line, any block type. No grid: the fixed length. The material skips a chevron whose far tip `FirstOffset + i x Spacing` is beyond `VisibleLength` ([[sys-grid]]).
- fixed length: 416 uu = `FirstOffset + (ChevronCount - 1) x Spacing` (120 + 4 x 74), owner-approved.
- material contract (`/Game/RogueRigs/FX/M_AimChevrons`, bp commits e8204d64, d2b9c630, 8513d873, 5e935801): Deferred Decal, Translucent, Unlit, decal response None; one Custom HLSL node. Chevron i has weight a = pow(max(Dec,0), FG x pow(i, FE)) (chevron 0 always 1), carried on `MP_Opacity` via `Multiply_0` (Custom x `Opacity`); accumulation `res = max(res, ln * a)`. `MP_EmissiveColor` = `Multiply_1` (`Color` RGB x `EmissiveGain`). Edges: one-pixel smoothstep AA as in [[sys-ground-telegraph]], no glow term. No brightness compensation for [[mpp-player-light-mask]].
- fade cause (task-331, measured): the material already drew opacity Decay^i = 1 / 0.78 / 0.61 / 0.475 / 0.37 (PIE read-back). The post-process (tonemapping, sRGB, exposure -0.94 EV100 of `PPV_XPGlow`; fitted model, not measured directly) compressed it to C_i/C_0 = 1.00 / 0.99 / 0.93 / 0.84 / 0.70. Decal emissive is drawn `SrcAlpha, One` (scaled by opacity, floor not darkened): not weighted twice. Fix: `FadeGain` 5.15 and `FadeExp` 0.55 give weights 1 / 0.278 / 0.154 / 0.096 / 0.064; `EmissiveGain` 1.0. After capture: 1.00 / 0.88 / 0.71 / 0.51 / 0.36 against 1 / 0.78 / 0.61 / 0.47 / 0.37; chevrons 1 and 2 exceed the 0.78^i+0.08 limit by about 0.02; owner accepted it and the cream/beige look.
- parameters: `ChevronCount` 5, `FirstOffset` 120 uu, `Spacing` 74 uu, `OpeningDeg` 60, `ArmLength` 40, `LineWidth` 12, `Decay` 0.78, `Color` linear (1.0, 0.84, 0.40), `Opacity` 1, `HalfLength` 500, `HalfWidth` 64, `VisibleLength` 1000, `FadeGain` 5.15, `FadeExp` 0.55, `EmissiveGain` 1.0. `GlowWidth` 10 and `GlowOpacity` 0.35 are unused (still wired to the Custom node). Component defaults (`EditDefaultsOnly`) equal the pushed values, plus `ProjectionHalfDepth` 256; `DefaultMaterialPath` (config) is used when `DecalMaterial` is unset. Values are provisional ([[aim-indicator]]).
- where to tune (`ApplyParameters` pushes every parameter except the last three; editing those in the material changes nothing):

  | knob | where | pushed by C++ | raising it |
  |---|---|---|---|
  | `Decay` | `BP_Miner` `AimIndicator` defaults, or header | yes | weaker fade |
  | `FadeGain` | material default | no | steeper fade |
  | `FadeExp` | material default | no | far chevrons dimmer |
  | `EmissiveGain` | material default | no | all chevrons brighter |
  | `Color`, `Opacity` (<= 1) | `BP_Miner` class defaults | yes | hue; Opacity scales all chevrons |
  | length | `FirstOffset`, `Spacing`, `ChevronCount` in `BP_Miner` | yes | `HalfLength` 500 must cover it |

- hiding: `ARRMiner::HandleDeath` calls `HideIndicator()`; the tick keeps it hidden while the miner `IsDead`. Shown whenever the miner is alive, mouse and stick aim alike.
- depends_on: [[sys-player]] (miner, aim direction), [[sys-grid]] (`GetClearDistance`), [[sys-ground-telegraph]] (`ARRDecalWarmUp`).
- implements: [[aim-indicator]].
- gotchas: a missing material asset logs one warning, nothing is drawn and the tick stops. The row stops at any block, not only rock.

## Relations
- [[systems-combat-index]] — parent index.
- [[aim-indicator]] — the GDD feature it implements.
- [[sys-player]] — the miner that owns the component and hides it at death.
- [[sys-ground-telegraph]] — same decal technique and the warm-up it reuses.
- [[sys-grid]] — block cell query that stops the row.
- [[mpp-player-light-mask]] — darkening the far chevrons fade with.
