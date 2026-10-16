---
id: barrel
description: "Orange explosive barrel placed on the grid: has health, explodes when destroyed, damaging characters and terrain."
parent: terrain-index
kind: spec
status: approved
---
# Barrel

- Orange industrial barrel, placed as a grid cell (type `barrel`). Orange means "explodes" ([[readability-rules]]).
- On destruction it plays a particle burst and explodes ([[explosions]]).
- It is one of the three block-destruction sources ([[destruction-rules]]).
- It has health. Shots and explosions damage it (provisional); it explodes when its health reaches 0.
- Values (provisional, owner 2026-10-02): health 20 (two rifle bullets), explosion radius 350 uu, character damage 40; about 16 barrels in arena_m1, in walls along paths, some in pairs for chains.
- Asset: modeled by hand.
- Milestone: M1.

- Owner rules 2026-10-12 (replace the explosion radius and the instant explosion above): the explosion radius is doubled, 350 to 700 uu (character damage 40 unchanged); when its health reaches 0 it does not explode at once: it explodes 1.5 s later, whatever killed it (a barrel hit by another explosion too, so chains become slow and readable); from 0 health a danger zone appears on the ground at the explosion radius (orange, "explodes") and a second circle grows inside it until it reaches the edge of the zone at the moment of the explosion, the same ground-circle logic as the capture zones ([[ground-telegraph]]).

- Barrel count (owner 2026-10-13, provisional): 25 % more barrels than the first value: about 20 in `arena_m1` instead of 16, and the same ratio on larger layouts. GDD only: the current map is not changed.

- Fuse and explosion feedback (owner 2026-10-16, provisional; Code Architect run 2): while the danger circle grows the barrel looks unstable: it shakes more and more (render only) and its body blinks orange-white faster and faster until the explosion. At the explosion a particle burst (flash, orange fireball, sparks, a little smoke) scaled to the radius plays on top of the grey-box sphere of [[explosions]], and an orange shockwave ring grows on the ground from the centre to the explosion radius, about as long as the sphere. Built as a barrel-only explosion visual that the kamikaze may reuse later ([[enemy-kamikaze]] unchanged for now).

## Relations
- [[terrain-index]] — parent index.
- [[explosions]] — the explosion rules and the grey-box sphere the burst plays on.
- [[enemy-kamikaze]] — may reuse the barrel explosion visual later.
- [[barrel-asset]] — the asset as built (Blender script, decals, wear) and its Unreal integration.
