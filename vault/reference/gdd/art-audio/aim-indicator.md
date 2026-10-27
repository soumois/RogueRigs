---
id: aim-indicator
description: "Aim indicator: white-gold ground chevrons in front of the miner showing aim direction only, weapon-agnostic, stopping at rock. Open for aim feedback."
parent: art-audio-index
kind: spec
status: provisional
---
# Aim indicator

- What (owner 2026-10-27, "piste 1", [[task-328-aim-chevrons]]): a row of chevrons ("› › ›") on the ground in front of the [[miner]], pointing in the aim direction, opacity decreasing with distance. Projected on the floor like the [[ground-telegraph]] (follows the floor, flattened by the camera perspective).
- Why: shows where the auto-fire goes (P2) and gives a stick player, who has no cursor, an aim reference (P1, P2, [[gdd-pillars]]). Not a laser: red is enemy fire and the shooter targeting laser ([[readability-rules]]), and the owner finds a laser too classic.
- Colour: white-gold, the player shots colour ([[color-code]]). Never red, orange, acid green or steel blue, no rarity colour, never a dotted circle (dotted circles are zone telegraphs).
- Weapon-agnostic (owner 2026-10-27): direction only. No spread widening, no pulse per shot, no length tied to a weapon range. Weapon-specific feedback may come later as a separate layer; not part of this node.
- Length: fixed. Stops at the first rock block on the aim line (miner cannot shoot through rock, [[destruction-rules]]; owner 2026-10-27, default accepted).
- Behaviour: pure visual, no collision, no gameplay effect; never places the [[drone]].
- Values (provisional, owner 2026-10-27, read from the owner-seen mockup): 5 chevrons; first at 120 uu from the miner; spacing 74 uu; opening about 60 degrees; arm length 40 uu; line width 12 uu; opacity 1 for the first, x0.78 per chevron; colour sRGB (255, 236, 170), linear about (1.0, 0.84, 0.40), with a soft glow.
- Light mask: no compensation; the far chevrons fade with the screen darkening around the miner ([[mpp-player-light-mask]]; owner 2026-10-27, "Laisser s'éteindre").
- Visibility: shown whenever the miner is alive, mouse and stick alike; hidden at death (owner 2026-10-27, default accepted).
- Reference images (in `Art/Source/aim-indicator/`, placed by task-328-01, owner 2026-10-27): `mockup-chevrons.png` (the chevron mockup the owner validated); `base-screenshot.png` (owner screenshot of the build the mockup was drawn on); `mockup-chevrons.ps1` (script that drew the mockup); `owner-screenshot.webp` (original owner image, source of the base screenshot). The mockup values are approximate, read off a drawing, not game values.
- Later, not in scope: turret placement preview at the aimed spot.
- Milestone: M3 (juice).

## Relations
- [[art-audio-index]] — parent index.
- [[miner]] — the aim it shows.
- [[color-code]] — white-gold player side colour.
- [[readability-rules]] — colours it must avoid.
- [[sys-aim-indicator]] — the C++ component and material contract that implement it.
- [[ground-telegraph]] — same projected-decal technique, different shape and meaning.
