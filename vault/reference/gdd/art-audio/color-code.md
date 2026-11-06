---
id: color-code
description: "Color of every element: rock, ore, heal ore, barrel, pickups, player side, shots, each robot. Open before any asset, VFX or prompt."
parent: art-audio-index
kind: spec
status: approved
---
# Color code

| Element | Color |
|---|---|
| Breakable rock | light grey tinted violet; intact rock has a smooth top with no cracks; cracks, then tilted plates, show damage (owner 2026-10-17, task-219, [[rock-look-reference]]) |
| Ore rock (Duskite) | same base + glowing violet crystals (kept, owner 2026-10-10) |
| Barrel | orange |
| Heal ore and heal pickups | red |
| Ore pickup (Duskite chunk) | glowing Duskite violet crystal cluster, like the HUD ore icon, with a violet trail (owner 2026-10-15, provisional until modelled, task-189); the owner judges it distinct enough from the lilac core of the big XP chip |
| XP pickups | glowing cyan, shaped as a hex nut (owner 2026-10-13, provisional until modelled; resource pickups other than ore keep this look); the big XP block (many pieces merged) is a data chip whose glowing core is bright magenta-pink with a glow of the same hue (owner 2026-10-13, was white), on a dark gunmetal board; implemented as MI_PickupXPBig: base (1, 0.15, 0.75), emissive (1, 0.2, 0.8) x 4, with magenta trail NS_XPTrailBig and sparkle NS_PickupSparkleBig; the big XP chip core now glows lilac-violet, GlowColor about (0.8, 0.25, 1.0), provisional, owner 2026-10-13, while NS_PickupSparkleBig, NS_XPTrailBig and MI_PickupXPBig stay magenta (a mismatch for the owner to decide) |
| Miner, turrets, drone | white + teal + rust |
| Drone camera eye | glows orange (1, 0.45, 0.08), a detail on a white + teal + rust element (owner 2026-10-21, "c'est juste visuel", task-279, provisional intensity) |
| Player laser shots (burst rifle, turrets), their impact sparks, the burst rifle and turret muzzle flashes | the green of the owner reference `Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp`: bright saturated green with several tones, pale white-green hot core, bright green body, darker green edge and glow, never one flat colour; never the Shooter acid green (owner 2026-11-05, "La couleur verte sur l'image semble bien pour l'instant", provisional, [[task-464-player-shot-colour]]; was white-gold) |
| Shotgun pellets (projectiles), their impact sparks and muzzle flash, mining laser | white-gold, the normal bullet look (shotgun: owner 2026-11-05, "ce sont des projectiles donc garde l'effet bullet normal", [[task-464-player-shot-colour]]; impact sparks: owner 2026-11-05, [[task-432-player-shot-impact-sparks]]) |
| Aim laser | bright lime green of the owner capture `Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp`, pale yellow-green core, lime glow; the acid green Shooter must still be spotted at once (owner 2026-11-06, "Prend la couleur comme référence", provisional, [[task-469-aim-laser-green]], [[aim-indicator]]; was white-gold) |
| Player shot impacts (where a player shot hits a robot, rock, barrel or ends) | flat cartoon impact burst after the owner reference `Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png`: bright core, radial star of sharp spikes, soft glow, small solid and outlined triangles flying out; white for projectiles (shotgun pellets), keeping the warm gold-orange accents and glow of the reference; green for lasers (burst rifle, turrets), the varied green of the player laser row (pale white-green core and spikes, bright green spikes, dark green glow); small and short so the robot and the damage number stay readable (owner 2026-11-05, "blanc pour projectile, vert pour le laser", provisional, [[task-465-shot-impact-burst]]; replaces the impact sparks of [[task-432-player-shot-impact-sparks]] and [[task-464-player-shot-colour]]) |
| Command console, shuttle | white + teal (player side) |
| Console marked rocks | pulsing white outline |
| Enemy shots, targeting laser | red (moving lines) |
| All robots | red eyes + yellow logo plate |
| Chaser | company yellow |
| Kamikaze | dark gunmetal + pulsing orange core |
| Chaser shield | physical riot shield: charcoal #33363B plate front and back, yellow and black diagonal stripes on the cut corners and over the top edge, dark gunmetal grip and bolts, yellow Kessler-Voss logo (KV and facets only, no plate) on the front; its fragments use the same texture (owner 2026-10-17, provisional: charcoal chosen over yellow and light grey so the shield stands out from the yellow chaser and the floor, task-225, task-226; [[art-prompts-shield]]) |
| Shooter | acid green |
| Heavy | steel blue + yellow/black hazard stripes |
| Normal / critical damage number | white / larger white-gold |
| Void space background | clouds in saturated indigo, electric violet and magenta (the floor's violet-magenta family, saturation raised), plus deep ultramarine blue as second hue, mainly in the deep layer (saturated, never grey steel blue); stars white with a slight warm or cool shift; never acid green, steel blue, red, orange, cyan, teal, salmon or pink (owner 2026-10-27, provisional, [[task-332-space-background]]; B3 of [[task-335-space-texture-layers]] replaces "low saturation"; pink cores in the brightest cloud centres tolerated by the owner 2026-10-27, deviation kept on purpose, task-335-02) |
| Background asteroids (void) | breakable rock family, light grey tinted violet, flat, no crystals, darker than the arena rocks so they read as background, not ground (owner 2026-10-27, provisional, [[task-334-near-asteroids]]) |
| Heal number ("+N" on the miner) | green, linear (0.1, 1, 0.2), the heal ring green; a small detail, so the Shooter keeps acid green as main colour (owner 2026-10-27, provisional, [[task-324-heal-numbers]], [[damage-numbers]]) |
| Level-up text and floor wave | gold, linear (1, 0.72, 0.15), a brief event, not a rarity colour (owner 2026-11-05, provisional, [[task-441-level-up-effect]]) |
| Ore number (Duskite icon + "+N" on the miner) | icon in its own violet colours (the HUD ore icon), "+N" white like a normal damage number (owner 2026-11-04, provisional, [[task-430-ore-pickup-number]], [[damage-numbers]]) |

Exception (owner 2026-10-11): the explosion warning sign on the barrel is a yellow triangle with a black border and a black burst, chosen by the owner after a reference sign; the yellow and black hazard stripes stay reserved for the heavy ([[barrel-asset]]).

Exception "console state lamps" (owner 2026-10-23, provisional): the command console orange lamps turn red (Locked), orange (Idle), green (Hacking, Complete) and its screen black or green; a small detail on a white + teal element, allowed despite red and green being enemy colours ([[objective-console]]).

- Colour principle (owner 2026-10-13): an element is coded by its main colour, the colour that dominates it, not by a detail. A small detail may reuse a colour that belongs to another element (the barrel is orange with a yellow sign, and yellow stays the chaser colour). The same colour may appear as a detail on any element.
- Not rigid (owner 2026-11-05): this table applies as [[readability-rules]] says, "Colour rules are not rigid"; it must not go against what a player expects, and explosion effects may hold red.
- Big XP block (owner 2026-10-13, replaces the white block of 2026-10-12): bright magenta-pink with a glow of the same hue, scale 1.8 unchanged; magenta is not the main colour of any other element.

## Relations
- [[art-audio-index]] — parent index.
- [[readability-rules]] — why these colors.
- [[barrel-asset]] — the barrel and its yellow and black warning sign.
- [[objective-console]] — the console state lamps exception.
- [[damage-numbers]] — damage and heal number rules.
