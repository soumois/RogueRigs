---
id: readability-rules
description: "Rules that keep the arena readable in every biome: brightness tiers, cracks mean breakable, enemy-only colors, UI-only rarity colors."
parent: art-audio-index
kind: rule
applies_to: art, vfx, level-design, image prompts
---
# Readability rules

- Terrain separates by brightness: breakable rock is lighter; ore glows.
- Cracks always mean "breakable".
- Acid green and steel blue are reserved for enemies in every biome; terrain never uses enemy colors.
- Red means enemy fire (shots, targeting lasers: moving lines) or healing (heal ore and pickups: static on the floor).
- Orange means "explodes": barrels and the kamikaze's core.
- Rarity colors appear only in the interface, never on an object in the arena.
- The shooter must be spotted in under a second.

- UI language (owner 2026-10-06 third test): every on-screen text is in English for now (HUD, cards, weapon names and descriptions, banners, end screens).

- Weapon slots (owner 2026-10-06 fourth test): under each owned weapon, ammo shows as a bar of the remaining percentage above 7 rounds, as one block per round at 7 or fewer; reloading shows a red bar filling over the reload time; turret charges show as blocks, and a recharging charge shows as a red bar in the leftmost empty block. Slots not owned yet show a padlock.

- Damage vignette (owner 2026-10-08): the red flash stays at full strength for at least 0.15 s before fading, so a long frame never hides the first hit.

## Relations
- [[art-audio-index]] — parent index.
- [[color-code]] — the current palette.
