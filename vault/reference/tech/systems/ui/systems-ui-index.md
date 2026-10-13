---
id: systems-ui-index
description: "HUD, minimap, pressure events, subtitles, text content: player-facing display systems."
parent: systems-index
kind: note
---
# C++ systems — ui

HUD, minimap, pressure events, subtitles, text content: player-facing display systems.

## Relations
- [[systems-index]] — parent index.
- [[sys-hud]] — HUD widget C++ base: health, XP, level-up, upgrade choice and game-over display events, created by the player controller.
- [[sys-hud-events]] — system
- [[sys-hud-minimap]] — system
- [[sys-hud-pressure]] — system
- [[sys-subtitles]] — subtitle subsystem: one voice line at a time, speaker prefix, C++ timer, company alert hooks, widget delegates.
- [[sys-text-content]] — runtime loader of the A4 text files: upgrade flavor, alert lines, barks, fallback, hooks.
