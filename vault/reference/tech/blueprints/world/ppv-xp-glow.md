---
id: ppv-xp-glow
description: "PPV_XPGlow, the PostProcessVolume of L_ArenaM1 that sets the bloom (halo) explicitly; open to tune the glow of the XP pickups and every emissive element."
parent: blueprints-world-index
kind: doc
---
# PPV_XPGlow (arena bloom volume)

- asset_path: placed actor in the persistent level /Game/RogueRigs/Maps/L_ArenaM1 (class PostProcessVolume, not a Blueprint; external actor package under Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/4/WO/), label PPV_XPGlow, at the origin (task-142-01, provisional, owner 2026-10-13, to test).
- general: bUnbound (Infinite Extent) true, Priority 0, bEnabled true, BlendWeight 1.
- overrides (only these three are on, read back): bOverride_BloomMethod with BloomMethod = Standard (Gaussian blur, BM_SOG); bOverride_BloomIntensity with BloomIntensity 1.3 (owner 2026-10-19, set by the owner in the editor, task-236; the level read 1.8 before, the 1.0 of task-142-01 had been raised since; engine default 0.675); bOverride_BloomThreshold with BloomThreshold -1.0 (engine default, bloom applies to the whole image). Exposure override since task-268-01 (provisional, owner 2026-10-21, owner tunes by eye): bOverride_AutoExposureMinBrightness and bOverride_AutoExposureMaxBrightness on, both 1.8 since task-272-03 (EV100; read back 1.8; revert 1.3, the value of task-268-01), so the eye adaptation is frozen and the dark ambient stays dark (before: -10 and 20, auto exposure brightened the dark scene and over-bloomed the XP). autoExposureMethod stays AEM_Histogram, AutoExposureBias stays 1. To tune: Details, Post Process Settings, Lens, Exposure, Min and Max EV100 (same value in both; +1 halves the brightness). Revert: untick both boxes. Details in [[lights-dark-ambient]]. No Lumen or colour grading override. Before this volume the level had none and used the engine defaults.
- blendables: Settings.WeightedBlendables holds one entry since task-272-08 (provisional, owner 2026-10-21): /Game/RogueRigs/Materials/PostProcess/M_PP_PlayerLightMask, weight 1.0 (read back; the array was empty before). Neutral until the collection is tuned (MPC MaskEnabled 0), so the look does not change yet; details in [[mpp-player-light-mask]]. Other settings read back unchanged (bloom 1.3 / -1, exposure min = max 1.8, bias 1). Revert: remove the array entry (Details, Post Process Settings, Rendering Features, Post Process Materials).
- tune: select PPV_XPGlow in the Outliner, Details panel, Post Process Settings, Lens, Bloom: change Intensity (higher = wider and brighter halo), Threshold (-1 = everything blooms; a positive value limits the halo to pixels brighter than it), Method. Tick the box in front of a field to override it, untick to go back to the engine default. Save the level (Ctrl+S) so the external actor file is written.
- scope: the volume is global to the level (Infinite Extent): it also changes the halo of the ore crystals and of every emissive material, not only the XP pickups ([[bp-pickup-xp]], [[color-code]]).
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-pickup-xp]] — the XP pickups whose halo depends on this volume.
- [[color-code]] — the colours of the glows.
- [[lights-dark-ambient]] — the dark ambient the fixed exposure serves.
- [[mpp-player-light-mask]] — the post-process material carried as a blendable (task-272-08).
