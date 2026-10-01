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
- Health and explosion values: open.
- Asset: modeled by hand.
- Milestone: M1.

## Relations
- [[terrain-index]] — parent index.
