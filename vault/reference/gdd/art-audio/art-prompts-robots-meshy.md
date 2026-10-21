---
id: art-prompts-robots-meshy
description: "Meshy prompt chains for the shooter, heavy and kamikaze: turnaround sheet plus four view prompts each, attachments, design choices. Not run yet."
parent: art-prompts
kind: note
---
# Shooter, heavy, kamikaze — Meshy prompt chains (not run yet)

Written 2026-10-21 on the model of the chaser chain ([[chaser-meshy-pipeline]], `Art/Source/chaser/prompts/` then `Art/Source/chaser-meshy/prompts/`). No image generated, no Meshy credit spent.

## Files
- `Art/Source/shooter-meshy/prompts/`, `Art/Source/heavy-meshy/prompts/`, `Art/Source/kamikaze-meshy/prompts/`: each holds `sheet.txt`, `front.txt`, `left.txt`, `right.txt`, `back.txt` (verbatim, English).

## Chain (ChatGPT / GPT Image 2)
1. `sheet.txt`: Image 1 = the robot cropped from `vault/assets/enemies-lineup.jpg` ([[art-prompts-robots]]), Image 2 = `vault/assets/chaser-turnaround.png` (style and simplification reference). Output: turnaround sheet, to be validated by the owner and saved in `vault/assets/`.
2. `front.txt`: Image 1 = front crop of the sheet, Image 2 = the sheet.
3. `left.txt`, `back.txt`: Image 1 = the generated front, Image 2 = the sheet, Image 3 = the crop of that view.
4. `right.txt`: same, Image 3 = the side crop mirrored horizontally (heavy, kamikaze). Shooter: Image 3 = the right side crop, NOT mirrored.
5. The four views are the Meshy multi-view input.

## Design choices in the prompts (proposals, owner to confirm)
- Shooter: asymmetric (cannon forearm on the right, [[enemy-shooter]]), so its sheet has four views (front, left, right, back) and its right view is not a mirror.
- Heavy: shoulders at most two thirds of the height, so about 160 cm wide at twice the chaser height, for the 80 uu capsule ([[enemy-heavy]]); hazard stripes on the top faces for the top-down camera; no back weak point (wishlist).
- Kamikaze: no legs, hover pose; orange core in an open rib cage, visible from every side and from above ([[enemy-kamikaze]]); short fins under the body.
- The parts of the heavy and the kamikaze are `open` in their specs: the volumes named here (fists, fins, rib cage) are proposals.
- Every effect is excluded from the images (targeting laser, projectile, thruster flame, explosion): Meshy would model it.
- Background light grey, as in the chaser view prompts (the locked turnaround phrase says white).

## Relations
- [[art-prompts]] — parent: method and locked phrases.
- [[art-prompts-robots]] — the lineup these sheets start from.
- [[chaser-meshy-pipeline]] — the chain these prompts copy, and the steps after Meshy.
- [[enemy-shooter]] — shooter spec.
- [[enemy-heavy]] — heavy spec.
- [[enemy-kamikaze]] — kamikaze spec.
