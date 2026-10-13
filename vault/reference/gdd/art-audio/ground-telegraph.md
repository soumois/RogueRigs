---
id: ground-telegraph
description: "Ground circles under capture zones and danger zones (barrel, heavy slam): a growing circle reaches the zone edge at 100 % or at the hit."
parent: art-audio-index
kind: spec
status: provisional
---
# Ground telegraph

- What: a flat marker drawn on the ground (a projected decal: it follows the floor and the top of rocks) made of a zone circle of a given radius and a second circle that grows from the centre to the edge of the zone. When the growing circle touches the zone circle the thing is complete (100 % of a hack, the explosion, the slam). Owner 2026-10-12, inspired by the reference game (dotted outer circle with a solid white progress circle).
- Uses: console and bonus chest hack zones ([[objective-console]], [[objective-bonus-chest]]), the barrel fuse ([[barrel]]), the heavy slam ([[enemy-heavy]]).
- Look: circles are sharp (no diffuse disc). Zone circle dotted; progress circle solid white. Colours: gold dotted while a hack is blocked by marked rocks, white-blue dotted when active; orange for danger ([[readability-rules]]).
- Behaviour: no collision; ignores robots, the miner and the player weapons; it never changes gameplay (rules stay in the objective, barrel or robot); it disappears with its owner, at once when the owner dies or is destroyed.
- Final values (task-112-02): lines 12 uu wide (constant in world units, whatever the zone radius), sharp with one-pixel anti-aliasing; zone circle 48 dashes, 55 % drawn, for all modes including danger; progress circle solid white, radius = zone radius x progress (0..1), invisible at 0; no disc fill. Fill added by task-118-01 (disc of radius = zone radius x progress, FillOpacity 0.25, see Fill below). Parameters Radius, Progress, Opacity 0..1, FillOpacity, see [[sys-ground-telegraph]].
- Status: values provisional (owner 2026-10-12).

- Fill (owner 2026-10-13, replaces "no disc fill" above and in the final values): the inside of the zone circle stays transparent and the growing progress circle is a disc filled with a transparent colour of its mode (white-blue for hacks, orange for danger), with its solid edge kept. At progress 0 nothing is filled; a danger zone is never fully filled before the effect is complete. Applies to the console, chest, barrel and heavy zones alike.
- Visibility: a telegraph exists only while its owner is in the matching state (hack zone while the objective is active, barrel from 0 health, heavy slam only during the telegraph); a robot running toward the miner shows none (owner 2026-10-13, bug report).

## Relations
- [[art-audio-index]] — parent index.
- [[readability-rules]] — colour meanings it follows.
- [[objective-console]] — hack zone using it.
- [[barrel]] — danger zone using it.
- [[enemy-heavy]] — slam signal using it.
