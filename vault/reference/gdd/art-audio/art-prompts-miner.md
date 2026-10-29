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

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[art-prompts-rigs]] — the first miner sheet and the T-pose turnaround this one replaces for 3D.
- [[art-prompts-drone]] — the turnaround format this prompt follows.
- [[miner]] — the character spec.
- [[asset-pipeline]] — turnaround step before the mesh.
