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
- Status: values provisional (owner 2026-10-12).

## Relations
- [[art-audio-index]] — parent index.
- [[readability-rules]] — colour meanings it follows.
- [[objective-console]] — hack zone using it.
- [[barrel]] — danger zone using it.
- [[enemy-heavy]] — slam signal using it.
