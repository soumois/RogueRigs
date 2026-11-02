---
id: readability-rules
description: "Rules that keep the arena readable in every biome: brightness tiers, cracks mean damaged rock, enemy-only colors, UI-only rarity colors."
parent: art-audio-index
kind: rule
applies_to: art, vfx, level-design, image prompts
---
# Readability rules

- Terrain separates by brightness: breakable rock is lighter; ore glows.
- Cracks always mean "damaged, digs faster" (owner 2026-10-17, was "breakable"): intact breakable rock is smooth, cracks then tilted plates show lost health or weaker rock.
- Acid green and steel blue are reserved for enemies in every biome; terrain never uses enemy colors.
- Red means enemy fire (shots, targeting lasers: moving lines) or healing (heal ore and pickups: static on the floor). The miner aim laser is white-gold, never red (owner 2026-11-02, [[aim-indicator]]).
- Orange means "explodes": barrels and the kamikaze's core.
- Rarity colors appear only in the interface, never on an object in the arena.
- The shooter must be spotted in under a second.

- UI language (owner 2026-10-06 third test): every on-screen text is in English for now (HUD, cards, weapon names and descriptions, banners, end screens).

- Weapon slots (owner 2026-10-06 fourth test): under each owned weapon, ammo shows as a bar of the remaining percentage above 7 rounds, as one block per round at 7 or fewer; reloading shows a red bar filling over the reload time; turret charges show as blocks, and a recharging charge shows as a red bar in the leftmost empty block. Slots not owned yet show a padlock.

- Damage vignette (owner 2026-10-08): the red flash stays at full strength for at least 0.15 s before fading, so a long frame never hides the first hit.

- Minimap (owner 2026-10-08): 200 px circle at the top left, centred on the miner, about 40 cells of radius; shows rock, floor and heal veins for cells already seen only; markers for the miner, the console (teal), the bonus chest (gold) and the shuttle (white); no enemies.
- Alert bar (owner 2026-10-08): twice as wide and twice as tall as before, its event icons scaled with it.

- Minimap reveal (owner 2026-10-09, provisional radius): cells are revealed in a circle of 18 cells around the miner (about the short half of the screen), the same in every direction, not the camera trapezoid.
- Minimap opacity (owner 2026-10-15, provisional): the disc is fully opaque; cells not seen yet are drawn black (0, 0, 0), so nothing of the game shows through it.
- Alert bar label (owner 2026-10-15): no ALERT text next to the bar, no pause indicator.

- Ground circles (owner 2026-10-12): zones the player must stay in are dotted circles on the ground (gold while a condition is missing, white-blue when active) with a sharp solid white circle growing to the edge for progress; danger zones (barrel, heavy slam) are orange, the colour of "explodes"; red stays reserved for enemy fire and healing ([[ground-telegraph]]).
- Weapon slots (owner 2026-10-12): an owned slot shows the weapon icon large without the name, the ammo or reload bar under it; a locked slot shows the padlock.

## Relations
- [[art-audio-index]] — parent index.
- [[color-code]] — the current palette.
