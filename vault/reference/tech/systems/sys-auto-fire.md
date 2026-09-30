---
id: sys-auto-fire
description: "URRAutoFireComponent: accumulator deciding when weapons, turrets and enemy shooters fire; burst math from FireRate, BurstCount, BurstDuty."
parent: systems-index
kind: doc
---
# Auto-fire

- responsibility: when to fire. The weapon decides what to fire (`FireOnce()`).
- main_classes: `URRAutoFireComponent`, one instance per weapon, turret or enemy shooter (approved). Replaces a `WaitDelay` ability loop.

## Burst timing (approved)
- `FireRate` = bursts per second, attribute on the weapon's ASC.
- Period = 1 / `FireRate`.
- Gap between two bullets = `BurstDuty` × period / (`BurstCount` − 1).
- Pause after the burst = period × (1 − `BurstDuty`).
- `BurstDuty` (share of the period used by the burst, 0 to 1) replaced `BurstInterval`, so the gap between bullets shrinks when fire rate rises.
- Example, [[burst-rifle]]: `FireRate` 2, `BurstCount` 3, `BurstDuty` 0.32 → bullets 0.08 s apart, 0.34 s pause. With +50 % fire rate (`FireRate` 3): 0.053 s and 0.227 s.
- [[shotgun]]: `BurstCount` 1, `BurstDuty` ignored.
- If the magazine empties mid-burst, the burst stops and the burst counter resets to 0.

- data: `BurstCount` and `BurstDuty` in the weapon's CSV row.
- depends_on: [[sys-weapons]].
- implements: pillar P2 auto-fire, [[enemy-shooter]] fire cadence.
- gotchas: none known yet.

## Relations
- [[systems-index]] — parent index.
