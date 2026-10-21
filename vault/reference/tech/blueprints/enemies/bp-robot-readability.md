---
id: bp-robot-readability
description: "Robot readability (task-266-01): brightness, saturation, roughness, cool rim on the chaser materials; shooter and heavy colours lowered. Open to tune burn."
parent: blueprints-enemies-index
kind: doc
---
# Robot readability parameters (provisional, owner 2026-10-20, owner tunes by eye)

Why: the yellow chaser burnt under the old sun and bloom; the warm player light (60 cd, [[lights-dark-ambient]]) would burn a bright yellow again close to the miner. Goal: keep the company yellow ([[color-code]]) but darker, less saturated, less shiny, with a faint cool rim to separate from the floor. No bloom, light or exposure change.

- assets: `/Game/RogueRigs/Art/Chaser/M_Chaser_Meshy_Anim` (BP_Chaser) and `/Game/RogueRigs/Art/ShieldChaser/M_ShieldChaser_Anim` (BP_ShieldChaser instances Body, BodyPlain, Shield, Emblem, Hidden). The two masters are separate copies ([[chaser-meshy-pipeline]]), so each carries the same added nodes: tune a value on the master default (all instances follow) or override it on one instance.
- parameters (group Readability, scalar and vector, none uses Custom Primitive Data):
  - `BodyBrightness` 0.7: multiplier on the base colour RGB.
  - `BodySaturation` 0.85: lerp from luminance (0) to the texture colour (1); luminance weights 0.299, 0.587, 0.114.
  - `BodyRoughness` 0.75: roughness = max(0.6, BodyRoughness); the old fixed roughness was 0.6, so a floor of 0.6 would change nothing: default raised to 0.75 (deviation from the briefed 0.6).
  - `RimColor` (0.25, 0.35, 0.8), `RimAmount` 0.35, `RimPower` 3: Fresnel (base reflect fraction 0, exponent RimPower) x RimColor x RimAmount, added to the emissive. Peak emissive at the silhouette about 0.13 luminance, far below bloom range.
- mask: brightness and saturation are blended by 1 - the existing red-eye mask (`saturate((R - 2 max(G, B)) x 3)`), and the rim is multiplied by the same 1 - mask, so the red eye texels keep the original colour and GlowAmount 25 (untouched). The yellow logo plate cannot be separated from the yellow body by hue (same colour), so it is darkened and desaturated like the body.
- MI_ShieldChaser_Emblem: overrides BodyBrightness 1, BodySaturation 1, RimAmount 0 (emblem unchanged). MI_ShieldChaser_Shield keeps the master defaults (not judged).
- animation: ClipId (CPD 0), AnimStart (CPD 1), RunRate, ClipOffset, Visible, GlowAmount and the WPO node untouched.
- other robots (greybox, instances of M_GreyBox, roughness already 0.8): MI_GB_Shooter Color (0.45, 1, 0.05) to (0.3, 0.68, 0.035) (green channel was 1.0); MI_GB_Heavy Color (0.15, 0.35, 0.7) to (0.12, 0.28, 0.56) (blue 0.7). MI_GB_Kamikaze (0.12, 0.13, 0.15) fine, MI_GB_Core orange, MI_GB_Eye, MI_GB_Chaser (yellow logo plates and hazard stripes) left as they are.
- how to tune: open the master or the instance in the editor, group Readability. Still burnt: lower BodyBrightness or raise BodyRoughness. Flat or muddy: raise BodySaturation, RimAmount. Rim too glowing: lower RimAmount.
- unverified: no render was looked at (no screenshots); values were read back only. Look of the rim, the mask edge on the eye, the burn at 60 cd close up and the shield instance are all unseen. The shader compiled without error (recompile raised nothing); instruction count not read.

## Relations
- [[blueprints-enemies-index]] — parent index.
- [[bp-chaser]] — uses M_Chaser_Meshy_Anim.
- [[bp-shield-chaser]] — uses the M_ShieldChaser_Anim instances.
- [[bp-shooter]] — MI_GB_Shooter colour lowered.
- [[bp-heavy]] — MI_GB_Heavy colour lowered.
- [[chaser-meshy-pipeline]] — the materials and their animation parameters.
- [[color-code]] — chaser = company yellow, red eyes.
