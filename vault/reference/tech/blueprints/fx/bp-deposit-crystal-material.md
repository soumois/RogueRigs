---
id: bp-deposit-crystal-material
description: "M_DepositCrystalGlow (task-262-01): crystal material of the Duskite and heal deposits with height gradient and Fresnel rim, capped emission; formula, parameters, tuning."
parent: blueprints-fx-index
kind: doc
---
# M_DepositCrystalGlow

- asset_path: /Game/RogueRigs/Art/OreDeposit/M_DepositCrystalGlow (Material, Default Lit, opaque, bUsedWithInstancedStaticMeshes true, no texture)
- purpose: owner screenshot 2026-10-20: the Duskite crystals looked flat white-pink. Cause: emission far above 1 (peak (6.6, 1.2, 12)) saturated after the tone mapper and bloom, so the lighting had no hold. This material keeps the glow but caps it and shapes it, so the facets read without cast shadows ([[lights-no-shadows]]).
- origin: a duplicate of M_XPNutGlow ([[bp-pickup-xp]], untouched, still used by the ore pickups, XP and every other glow). The pulse nodes are unchanged copies, so the desync of task-257 works: sine argument (turns) = Time x (PulseFrequency x (PerInstanceCustomData 5 x PulseRate)) + dot(ObjectPositionWS, (0.0137, 0.0211, 0)) + (PerInstanceCustomData 4 + PulsePhase). Neutral inputs (data absent: phase 0, rate 1) give exactly the M_XPNutGlow expression. Same nodes as there (Multiply_1, Multiply_5, Multiply_4, Add_0, Add_2, Add_3, Sine_0, Multiply_2, Add_1).
- emission: Multiply_3 (GlowColor x GlowAmount) x (1 + PulseAmount x sin) as before, then x Add_5 = Lerp(GradientBase, 1, saturate((Z + 5) / GradientHeight)) + Fresnel(FresnelPower) x FresnelAmount (Multiply_7 into EmissiveColor). Z = (WorldPosition - ObjectPositionWS).z (Subtract_0, ComponentMask B, Add_4 +5 for the base at -5 uu, Divide_0, Saturate_0, LinearInterpolate_0; Fresnel_0, Multiply_6, Add_5). Base colour = Color (lit by the sun and sky, Roughness 0.4, default specular).
- parameters and defaults: GlowAmount 2, PulseAmount 0.5, PulseFrequency 0.8333 (1.2 s), GradientBase 0.35 (emission fraction at the base), GradientHeight 70 uu, FresnelPower 3, FresnelAmount 0.5, Roughness 0.4, Color and GlowColor (per instance), PulsePhase (CPD 1) and PulseRate (CPD 2) kept for static meshes.
- instances: MI_DuskiteDeposit_Crystal (/Game/RogueRigs/Art/OreChunk/, reparented from M_XPNutGlow): Color (0.25, 0.04, 0.55), GlowColor (0.55, 0.1, 1.0), GlowAmount 2 (was 6.86), PulseAmount 0.5 (was 0.75), Roughness 0.4; on the crystal slot of SM_DuskiteDeposit_01..03. MI_HealDepositCrystal (/Game/RogueRigs/Art/OreDeposit/, new): Color (0.5, 0, 0.025), GlowColor (1, 0, 0.05), GlowAmount 1.5, PulseAmount 0.5; on the crystal slot (index 1) of SM_HealDeposit_01..03 (was MI_HealCrystal_Bright). MI_HealCrystal_Bright and _Dark stay unchanged because BP_ArenaGrid still uses them as HealOreCrystalMaterial (rock crystal clusters); the heal deposit keeps its own instance so those do not change.
- why these numbers: peak emission before = (6.6, 1.2, 12). Now at the tip with the highest pulse: Duskite 2 x 1.5 x (1 + 0.5 rim) x (0.55, 0.1, 1) = about (2.5, 0.45, 4.5), rim peak only, most of the surface (gradient 0.35 to 1, no rim) is (0.4 to 1.6, 0.07 to 0.3, 0.7 to 3). Luminance up to about 1.2, mean well under 1: it stays saturated violet (G stays far below R and B) and the lit base colour adds the facet shading. Heal uses 1.5 because a very bright pure red skews orange after the tone mapper (task-230-25), with the small blue part kept.
- tune: open MI_DuskiteDeposit_Crystal or MI_HealDepositCrystal. Still too flat: lower GradientBase (more base-to-tip contrast), raise FresnelAmount or lower FresnelPower (wider rim), lower GlowAmount. Too dull: raise GlowAmount (bloom needs emission above about 1 on the tips) or PulseAmount. Colour: Color for the lit body, GlowColor for the glow hue. To change the gradient scale: GradientHeight in uu (the meshes are up to about 70 uu high).
- assumptions: instances only yaw-rotate and scale uniformly, so world Z minus the instance origin is the object height (a non-uniform scale or a tilt shifts the gradient). ObjectPositionWS is the instance origin on instanced meshes.
- verified (MCP read-back): recompile without error; parameter list GlowAmount, Roughness, PulseFrequency, PulseAmount, PulsePhase, PulseRate, GradientHeight, GradientBase, FresnelPower, FresnelAmount, Color, GlowColor; instance values and parents; the slot assignments of the six meshes; wiring of the emissive output. unverified: the on-screen look, the Fresnel on the real normals, the bloom halo at the new level, the pulse desync at runtime (nobody saw a render); NumCustomDataFloats stays set by the C++ (RRArenaGrid, at least 6), no grid asset changed.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-pickup-xp]] — M_XPNutGlow, the material it was copied from, and the pulse desync inputs.
- [[bp-pickup-ore]] — MI_DuskiteDeposit_Crystal, the Duskite instance.
- [[bp-pickup-heal]] — the heal deposit instance.
- [[bp-arena-grid]] — the grid that places the deposit instanced meshes.
- [[sys-grid-deposits]] — writes the pulse custom data 4 and 5.
- [[lights-no-shadows]] — the shadows are off, the volume must come from the material.
- [[color-code]] — violet and red.
