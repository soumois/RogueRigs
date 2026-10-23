---
id: objective-console
description: "Mandatory objective: a command console dropped near the miner, marked rocks to clear, then a drone hack that unpauses the alert bar."
parent: run-index
kind: spec
status: approved
---
# Objective: command console

- Triggered by an alert tier; at least one per level ([[level-flow]]).
- The console is dropped outside the miner's view radius, but not at the far end of the level.
- The rocks around it are marked and must be destroyed before hacking. Only the drone and explosions break rock ([[destruction-rules]]), so clearing them costs digging time.
- Hack rules:
  - the drone hacks only while the miner stays inside the console zone;
  - if the miner leaves, the drone follows him and hacking stops; progress drains slowly;
  - the console shows its gauge, so the player sees why progress stops.
- The drone hacks one target at a time and cannot dig and hack at once.
- While the console is not hacked, the alert bar is paused.
- Values (provisional, owner 2026-10-03): dropped 36 to 60 cells (67 uu) from the miner; marked rocks within 6 cells, pulsing white outline; hack 16 s (owner 2026-10-07 fifth test: 20 % shorter, was 20 s); zone radius 9 cells; progress drains 5 %/s while the miner is out. Cell counts converted to 67 uu cells in task-029-04, same world distances.
- Landing clearance (provisional, owner after handover-2026-10-06): the console lands only where its whole 2 × 2-cell footprint is open floor, so its mesh never overlaps rock.
- M1: one console, dropped when the alert bar reaches 40 % ([[level-flow]]).
- Lore (provisional): the Rigs' ship detects an entry point into the claim's network and drops the console there as an access point ([[lore]]).
- Milestone: M1.

- Locator (owner 2026-10-06 third test): while the console is off screen, an arrow on the screen edge points to it; the same arrow points to the shuttle during extraction ([[extraction]]).

- Locator look (owner 2026-10-07 fifth test, provisional): a real arrow image about 64 px, pulsing, with the distance in metres next to it.

- Zone look (owner 2026-10-12, rendering only, the rules above are unchanged): the hack zone is drawn on the ground as a sharp circle of the zone radius (dotted gold while marked rocks remain, dotted white-blue once they are all destroyed and hacking can start) and a second sharp solid white circle grows from the console to the edge of the zone: when it reaches the zone circle the hack is at 100 %; it shrinks while the miner is out of the zone as the progress drains. No diffuse growing disc ([[ground-telegraph]]).

- Console rules (owner 2026-10-13, provisional; replace the marked-rock radius, zone radius and hack time above): the hack zone radius is 7 cells (67 uu), every rock inside the zone, ore included, is marked and must be destroyed before the hack can start (the marking radius equals the zone radius, so the zone never contains an unmarked rock); hack time 12 s (replaced by 10 s below); the drain while the miner is out stays 5 %/s. The bonus chest is unchanged ([[objective-bonus-chest]]).

- Zone fill (owner 2026-10-13, rendering only): the inside of the zone circle stays transparent; the growing circle is a disc filled with a transparent colour, so the progress reads as a filled area ([[ground-telegraph]]).

- Hack time (owner 2026-10-23, provisional): 10 s, console only (was 12 s); the bonus chest is unchanged ([[objective-bonus-chest]]).

- Marked rocks per drop (owner 2026-10-23, provisional): the console lands only where its zone holds between 10 and 40 markable rocks (ore included); when no spot fits, it lands where the count is nearest that range. Replaces the plain "prefer spots with rocks" choice. The bonus chest is unchanged ([[objective-bonus-chest]]).

## Relations
- [[run-index]] — parent index.
- [[drone]] — the drone's hack state.
- [[objective-bonus-chest]] — the optional hack objective.
