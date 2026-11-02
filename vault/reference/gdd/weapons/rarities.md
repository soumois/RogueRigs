---
id: rarities
description: "Five upgrade rarity tiers (grey to yellow): fixed value per tier, luck-weighted draw, UI-only colors. Open for card generation or balance."
parent: weapons-index
kind: spec
status: approved
---
# Rarities

- Tiers, low to high: grey, green, blue, purple, yellow. Effects are always yellow.
- Rarity colors appear only in the interface (upgrade cards, HUD), never on an object in the arena ([[readability-rules]]).
- Each (stat, target, tier) has one fixed value, the same in every run. Values: open.
- Draw (provisional): each tier has a base weight; luck raises the weight of higher tiers: weight = base weight × (1 + k × luck)^rank, with rank 0 (grey) to 4 (yellow), then normalized. Placeholders: base weights and k in `UpgradeRarities.csv`, columns `BaseWeight` and `LuckFactor`, one row per tier (`Grey`, `Green`, `Blue`, `Purple`, `Yellow`).
- A bonus chest adds a temporary luck bonus to its draw ([[objective-bonus-chest]]).
- Milestone: M1 (owner 2026-10-03, moved from M2).

## Relations
- [[weapons-index]] — parent index.
- [[upgrades]] — what a tier scales.
- [[player-stats]] — the luck stat.
