---
id: bp-aim-laser-material
description: "M_AimLaser (task-415-01): translucent unlit two-sided aim laser material, UV layout, parameters and defaults, core/glow/tip-fade Custom node. Open to tune the laser look."
parent: blueprints-fx-index
kind: doc
---
# M_AimLaser

- asset_path: /Game/RogueRigs/FX/M_AimLaser (Material, commit 1700f6ff). Replaces M_AimChevrons, deleted in 8326a0a8.
- purpose: the miner's aim laser ([[aim-indicator]]), drawn on a stretched engine Plane by [[sys-aim-indicator]].
- settings: Surface domain, Translucent, Unlit, two sided, depth test on (the line is hidden by walls). Additive is one blendMode change if Translucent hides the line over muzzle flashes.
- uv: UV.x runs 0 at the Muzzle to 1 at the tip, UV.y across the width.
- graph: one Custom HLSL node computes the opacity: bright core with one-pixel AA, glow = `GlowOpacity` x (1 - d)^2 beyond the core, tip fade on UV.x. `Color` x `EmissiveGain` feeds EmissiveColor.
- parameters and defaults (provisional, owner 2026-11-02): `Color` linear (1.0, 0.84, 0.40), `EmissiveGain` 1.0, `Opacity` 1.0, `CoreFrac` 0.16, `GlowOpacity` 0.3, `TipFadeFrac` 0.15 (last 15 % fades to the tip).
- c++: the component pushes the six values to a dynamic instance every `BeginPlay` (defaults live on the component, see [[sys-aim-indicator]]); editing the material defaults changes nothing while the component pushes.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-aim-indicator]] — the component that places the Plane and drives the parameters.
- [[aim-indicator]] — the GDD feature.
- [[bp-blob-shadow-material]] — sibling unlit translucent Plane material.
