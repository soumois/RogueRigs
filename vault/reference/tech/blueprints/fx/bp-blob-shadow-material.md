---
id: bp-blob-shadow-material
description: "M_BlobShadow and MI_BlobShadow (task-264-01): unlit translucent radial dark disc, the fake blob shadow on a Plane under actors; parameters, usage."
parent: blueprints-fx-index
kind: doc
---
# M_BlobShadow and MI_BlobShadow

- asset_path: /Game/RogueRigs/FX/BlobShadow/M_BlobShadow (Material) and /Game/RogueRigs/FX/BlobShadow/MI_BlobShadow (MaterialInstanceConstant, no override: defaults). The C++ references the instance MI_BlobShadow.
- purpose: owner 2026-10-20: the only cast shadow is the player's light ([[lights-no-shadows]]); everything else gets a simple fake round shadow on the floor.
- settings: Unlit, Translucent, two sided, fully rough, `bDisableDepthTest` false (the disc is hidden by walls and props), `bUseTranslucencyVertexFog` false, no lighting, no WPO, no texture, no scene texture read. Translucent rather than Masked dither: a soft gradient edge needs real alpha, and a dither under TSR would shimmer. The disc is a handful of pixels per actor, so the blend cost is small. Cast shadow is off at the component (the material has no cast flag for translucent surfaces beyond the component).
- graph: Distance(TexCoord[0], (0.5, 0.5)) x 2, OneMinus, Saturate, Power(Exp = Softness), x Opacity into Opacity; Tint (vector parameter) into EmissiveColor. Result: alpha 1 x Opacity at the UV centre, 0 at radius 0.5 (the plane edge; the corners are also 0).
- parameters and defaults: Opacity 0.45, Softness 1.6 (power on the radial falloff: 1 linear, higher gives a tighter core and softer edge), Tint (0.02, 0, 0.04) dark violet-black.
- usage: engine /Engine/BasicShapes/Plane (100 x 100 uu, UV 0..1) lying flat just above the floor (a few uu), scale X = Y = footprint diameter / 100. Component: no collision, no cast shadow, no lighting; set the primitive's Translucency Sort Priority low on the component (the material has no such property in this engine version, so draw order under effects is a component setting).
- what 264-02 expects: a static mesh component with the Plane and MI_BlobShadow, `CastShadow` false, collision off, scale from the actor footprint, kept at floor height under the actor, in the robot base class, the miner and the drone. Several of them can share the instance; per-actor variation (opacity) can use a dynamic instance or custom primitive data if needed (not wired).
- c++ (task-264-02): used by `URRBlobShadowComponent` ([[sys-blob-shadow]]) on robots, the miner and the drone; instance defaults only, no dynamic instance, no custom primitive data.
- verified (MCP read-back): recompile without error; blend Translucent, shading model Unlit, two sided true, depth test not disabled, fully rough; wiring of Opacity (Multiply of Power and Opacity) and EmissiveColor (Tint); parameter list Softness, Opacity, Tint on the instance with defaults 0.45, 1.6, (0.02, 0, 0.04, 1); both assets saved.
- unverified: the on-screen look (no capture was taken), the falloff shape at the chosen Softness, the draw order against the floor and the effects, the opacity against the darkest floor tiles.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[lights-no-shadows]] — why a fake blob shadow replaces cast shadows.
- [[readability-rules]] — the dark disc must not hide the silhouettes or the telegraphs.
- [[sys-blob-shadow]] — the C++ component that draws it.
- [[perf-robot-components-audit]] — the cost budget of per-robot components.
