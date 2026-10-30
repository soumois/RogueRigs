---
id: art-prompts-miner
description: "ChatGPT turnaround prompt of the miner (A-pose, front, side, back) for the owner's own 3D mesh, with images to attach. Open for miner art."
parent: art-prompts
kind: note
---
# Miner — turnaround prompt for ChatGPT (A-pose)

The owner generates the sheet in ChatGPT and makes the 3D mesh himself (owner 2026-10-29, task-364). No Kie.ai run, no crew-generated image. The validated sheet goes to `vault/assets/` when the owner says so.

## Decisions (owner "ok", 2026-10-29, provisional)
- Pose: A-pose, arms about 45 degrees down (not the T-pose of [[art-prompts-rigs]], kept as history).
- Images to attach in ChatGPT, keep the same design: `vault/assets/miner-turnaround.jpg` and `vault/assets/rigs-lineup.jpg`.
- Prompt text: as drafted, no change.
- Animation and weapon holding: not planned as a task now.

## Prompt (verbatim)
```
Character turnaround sheet for 3D modeling of a stylized space miner, the playable hero of a top-down game, same character design as the attached validated miner: front view, side view (exact 90 degrees) and back view, all three at the same size, same angle and same framing, full body, subject centred with a margin, standing in a neutral A-pose (arms straight, about 45 degrees down from the shoulders, palms facing down, fingers together, thumbs slightly apart), legs straight and slightly apart, feet flat, no stand, no ground.
Style: stylized flat-shaded low-poly game asset, chunky blocky volumes with large flat facets, facets clearly visible, no smoothing, no textures, clean hard-surface look.
Design: a sturdy human miner in a full suit of rigid plate armor, slightly chunky proportions with a slightly large head so it reads from a steep top-down camera. A closed helmet with a wide amber visor. Rigid plates on the chest, shoulders, upper arms, forearms, thighs and shins, with clearly separated joints at the neck, shoulders, elbows, wrists, hips, knees and ankles (dark segmented joint covers) so the body can be rigged and bent. Big armored gloves with separated fingers, able to grip a rifle. A compact salvaged backpack bolted to the back and a small pouch on the belt. A clearly readable front and back.
Colors and wear: patched, mismatched white and teal plates with rust chips, bolts, rivets and welded repairs, one plate visibly replaced by a different teal plate, a repaired and improvised salvaged look. The amber visor is the only warm light; no red, no yellow, no green, no blue, no violet.
Background and light: plain white background, even neutral lighting, no shadows, no weapon, nothing in the hands, no effects, no text, no UI, no logo.
```

## Rigged mesh (owner 2026-10-30)
The owner delivered the rigged miner made from the turnaround sheet (Meshy auto-rig, no credit spent by the crew; task-382).

Owner words (2026-10-30, French): "Le personnage riggé".

- Source file: `C:/Users/soumo/Downloads/Meshy_AI_Character_output.fbx` (12,468,364 bytes, SHA-256 f929daa5bef58f556567dbc447b3169aa9f88bfec1ce2a7378e2650a3fc4d22f), binary FBX 7.4.
- Saved files in `Art/Source/miner-meshy/`: `Meshy_AI_Character_output.fbx` (byte-identical to the download), `texture_0.png` (base colour) and `normal.png` (both 2048 x 2048, extracted from the FBX), `miner_meshy_identify.report.json`. Script `Tools/art/blender/miner_identify.py` (headless Blender 5.2, no mesh edit).
- Measured: one mesh `output_unwrapped`, 25,744 triangles, 40,681 vertices; bounding box 185.53 x 46.54 x 170.00 cm (X x Y x Z, A-pose, height 170 cm); one material `BakedMaterial`; one UV layer `UVMap`.
- Skeleton: armature `SmartRigArmature`, 66 bones, single root `mixamorig:Hips` (Mixamo names, five fingers per hand with four bones each, legs to `Toe_End`, plus a `headfront` bone). Skin: 52 vertex groups, 0 vertices without weight, 4 influences at most, one Armature modifier.
- Animation: no action in the file (rig only, no clip).
- Turnaround sheet: not in the repo (owner Q3 default, 2026-10-30), to save when the owner sends it.
- Decided 2026-10-30 (owner, task-386): triangle budget 25,744 as delivered (provisional); animation route Mixamo clips in place with a new ABP_Miner; Unreal import and replacement of the Quinn mannequin on BP_Miner in task-386. Open: weapon held and its hand socket.

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[art-prompts-rigs]] — the first miner sheet and the T-pose turnaround this one replaces for 3D.
- [[art-prompts-drone]] — the turnaround format this prompt follows.
- [[miner]] — the character spec.
- [[asset-pipeline]] — turnaround step before the mesh.
