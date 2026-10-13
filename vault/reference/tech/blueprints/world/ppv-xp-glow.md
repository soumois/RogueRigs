---
id: ppv-xp-glow
description: "PPV_XPGlow, the PostProcessVolume of L_ArenaM1 that sets the bloom (halo) explicitly; open to tune the glow of the XP pickups and every emissive element."
parent: blueprints-world-index
kind: doc
---
# PPV_XPGlow (arena bloom volume)

- asset_path: placed actor in the persistent level /Game/RogueRigs/Maps/L_ArenaM1 (class PostProcessVolume, not a Blueprint; external actor package under Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/4/WO/), label PPV_XPGlow, at the origin (task-142-01, provisional, owner 2026-10-13, to test).
- general: bUnbound (Infinite Extent) true, Priority 0, bEnabled true, BlendWeight 1.
- overrides (only these three are on, read back): bOverride_BloomMethod with BloomMethod = Standard (Gaussian blur, BM_SOG); bOverride_BloomIntensity with BloomIntensity 1.0 (engine default 0.675); bOverride_BloomThreshold with BloomThreshold -1.0 (engine default, bloom applies to the whole image). No exposure, Lumen or colour grading override. Before this volume the level had none and used the engine defaults.
- tune: select PPV_XPGlow in the Outliner, Details panel, Post Process Settings, Lens, Bloom: change Intensity (higher = wider and brighter halo), Threshold (-1 = everything blooms; a positive value limits the halo to pixels brighter than it), Method. Tick the box in front of a field to override it, untick to go back to the engine default. Save the level (Ctrl+S) so the external actor file is written.
- scope: the volume is global to the level (Infinite Extent): it also changes the halo of the ore crystals and of every emissive material, not only the XP pickups ([[bp-pickup-xp]], [[color-code]]).
- owner_agent: agent-bp

## Relations
- [[blueprints-world-index]] — parent index.
- [[bp-pickup-xp]] — the XP pickups whose halo depends on this volume.
- [[color-code]] — the colours of the glows.
