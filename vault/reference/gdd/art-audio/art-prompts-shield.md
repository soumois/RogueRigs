---
id: art-prompts-shield
description: "Shield chaser riot shield: validated sheet, prompt chain, Meshy view prompts, raw Meshy FBX location, open texture steps. Open for shield art."
parent: art-prompts
kind: note
---
# Shield chaser riot shield — prompt chain and files

Owner decisions (2026-10-17): the shield chaser carries a physical riot shield, not an energy shell; the robot is the chaser mesh unchanged plus the shield, with new clips (shield held in front while running, shield bash). Inspired by a Warhammer 40k boarding shield, without its weapon slot and without its fist emblem (third-party IP); the centre carries the Kessler-Voss emblem ([[kessler-voss-logo]]). Colours in [[color-code]].

## Files
- Validated sheet (front, side, back): `vault/assets/shield-turnaround.webp`, generated in ChatGPT by the owner from the prompt below.
- Meshy view prompts (front, back, left, right; front asked without emblem): `Art/Source/shield-meshy/prompts/`.
- The four generated views, Meshy input (front without emblem): `vault/assets/shield-meshy-{front,left,back,right}.webp` (left = front face to the left of the image).
- Raw Meshy mesh, untextured: `Art/Source/shield-meshy/meshy-raw/Meshy_AI__1017052835_model-edit.fbx` (copied from the owner's Downloads).

## Sheet prompt (verbatim, attachments: `chaser-turnaround.png` and the emblem)
```
Same art style as the attached yellow security robot sheet. 3D source sheet on a plain white background, even lighting, no shadows: front, side and back orthographic views of one heavy riot shield carried by this robot. A tall rectangular slab of thick armour plate, about two thirds of the robot's height and slightly wider than its torso, with the top-right and bottom-left corners cut at 45 degrees. Solid plate: no hole, no cut-out, no weapon slot. Front face: flat bright industrial yellow, a raised inner border panel, a few chunky round bolts, and the attached charcoal asteroid emblem in the centre. Both cut corners and the top edge carry a band of yellow and black diagonal hazard stripes. Back: dark gunmetal frame with a thick grip bar and a forearm bracket. Clean corporate equipment: no rust, no chipped paint, no scratches. Flat-shaded low-poly, large visible facets, rigid mechanical parts only, no text, no UI.
```
The result showed a dark blob in place of the emblem; the owner asked for the real emblem, applied in the texture rather than regenerated.

## Animation reference prompt (not run yet)
```
Keep this exact shield and the attached robot unchanged. Character sheet on a plain light grey background, three-quarter top view, two poses of the robot holding the shield on its left forearm: 1) running, shield held in front of its torso, tilted slightly back; 2) shield bash, body leaning forward, shield thrust forward at arm's length. Flat-shaded low-poly, no effects, no text.
```

## Next steps (open)
- Texture in Blender from the views: flat yellow front, stripes, gunmetal back; emblem as a decal from the emblem-only logo.
- Readability: the camera sees mostly the top edge (A12), hence the stripes on the top edge.
- Gameplay: whether the shield blocks only from the front is open; today it is a 30-point pool ([[enemy-chaser]]).

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[enemy-chaser]] — the shield variant this shield belongs to.
- [[color-code]] — shield colours.
- [[kessler-voss-logo]] — the emblem on its face.
