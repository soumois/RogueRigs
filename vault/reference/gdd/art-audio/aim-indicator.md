---
id: aim-indicator
description: "Aim indicator: lime green laser from the rifle muzzle along the aim, above the floor, no length limit, stopping at the first robot, barrel or rock. Open for aim feedback."
parent: art-audio-index
kind: spec
status: provisional
---
# Aim indicator

- What (owner 2026-11-02, [[task-415-aim-laser]], replaces the chevrons of [[task-328-aim-chevrons]] to task-331): a thin straight laser line from the shown rifle Muzzle of the [[miner]] along the aim direction, horizontal at Muzzle height, above the floor and parallel to it, never piercing it (owner correction 2026-11-02).
- Why: shows where the auto-fire goes (P2) and gives a stick player, who has no cursor, an aim reference (P1, P2, [[gdd-pillars]]). The 2026-10-27 "not a laser" choice is reversed by the owner (2026-11-02); the laser is never red, the colour of enemy fire and of the shooter targeting laser ([[readability-rules]]).
- Colour (provisional, owner 2026-11-06, [[task-469-aim-laser-green]], "Prend la couleur comme référence"): the bright lime green of the owner capture `Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp`, pale yellow-green core and lime glow, sampled from that image; only the colour changed, not the look (was white-gold sRGB (255, 236, 170)). Never red, orange, steel blue or a rarity colour; the acid green Shooter must still be spotted at once ([[color-code]]).
- Weapon-agnostic (owner 2026-10-27): direction only. No spread widening, no pulse per shot, no length tied to a weapon range. Weapon-specific feedback may come later as a separate layer; not part of this node.
- Length (provisional, owner 2026-11-06, [[task-468-aim-laser-unlimited-length]], "On va arrêter de limiter la ligne d'AIM en longueur. Juste cela s'arrête lorsque cela touche un mur ou un robot."): no maximum length; from the Muzzle to the first robot (its shield included), barrel or rock on the aim line, whichever is nearer; rock means breakable blocks and the walls and cliffs of the arena border alike, and the line is never drawn over a wall; pickups, turrets, drone, console and extractor do not stop it; when nothing is hit it runs past the screen edge (was at most 600 uu, [[task-423-miner-aim-laser-range-fade]]) (miner cannot shoot through rock, [[destruction-rules]]).
- Look (provisional, owner 2026-11-02): bright core 3 uu wide plus a soft glow of the same colour about 8 uu on each side at low opacity; uniform brightness along the line; no fade at the tip: the line ends sharp on the wall, robot or barrel it touches (provisional, owner 2026-11-06, [[task-468-aim-laser-unlimited-length]]; was the last 150 uu fading, [[task-423-miner-aim-laser-range-fade]]); unlit.
- Reticle: none at the laser end; in the reference the hex brackets are the player cursor, not part of the laser (owner 2026-11-02).
- Behaviour: pure visual, no collision, no gameplay effect; never places the [[drone]].
- Scope: the miner only; turrets have no aim laser (owner 2026-11-02). The chevron values and fixes (tasks 328 to 331, [[task-329-aim-chevrons-fade]], [[task-330-aim-chevrons-sharp]]) are in git history; the chevron code and M_AimChevrons are removed.
- Light mask: no compensation; the far part of the laser fades with the screen darkening around the miner ([[mpp-player-light-mask]]; owner 2026-10-27, "Laisser s'éteindre").
- Visibility: shown whenever the miner is alive, mouse and stick alike; hidden at death (owner 2026-10-27, default accepted).
- Reference images (in `Art/Source/aim-indicator/`, placed by task-328-01, owner 2026-10-27): `mockup-chevrons.png` (the chevron mockup the owner validated); `base-screenshot.png` (owner screenshot of the build the mockup was drawn on); `mockup-chevrons.ps1` (script that drew the mockup); `owner-screenshot.webp` (original owner image, source of the base screenshot); `owner-test-2026-10-27.webp` (owner screenshot of the task-328 build, 2026-10-27: five equally bright chevrons, blurry edges; source of [[task-329-aim-chevrons-fade]] and [[task-330-aim-chevrons-sharp]]); `owner-test-2026-10-27-b.webp` (owner screenshot of the task-329/330 build, 2026-10-27: sharp edges, beige chevrons, fade barely visible; source of [[task-331-aim-chevrons-fade-measured]]). The mockup values are approximate, read off a drawing, not game values. Laser reference (owner 2026-11-02): `laser-reference-2026-11-02/` (f01 to f08 at 4 fps and `sheet.png`, 00:03 to 00:05 of an owner co-op top-down video: thin bright straight lasers, one colour per player).
- Later, not in scope: turret placement preview at the aimed spot.
- Milestone: M3 (juice).

## Relations
- [[art-audio-index]] — parent index.
- [[miner]] — the aim it shows.
- [[color-code]] — colour table, aim laser row.
- [[readability-rules]] — colours it must avoid.
- [[sys-aim-indicator]] — the C++ component and material contract that implement it.
- [[ground-telegraph]] — projected-decal technique the former chevrons used.
- [[task-415-aim-laser]] — replacement of the chevrons by the aim laser (owner 2026-11-02).
- [[task-329-aim-chevrons-fade]] — fix of the missing distance fade (owner test 2026-10-27).
- [[task-330-aim-chevrons-sharp]] — fix of the blurry chevron edges (owner test 2026-10-27).
