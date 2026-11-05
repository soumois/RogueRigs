---
id: bp-heavy-death-puff
description: "NS_HeavyDeathPuff, the one-shot cartoon flash, 3x the robot one, that hides the heavy's 0.15 s death shrink; open for its scaled values."
parent: blueprints-fx-index
kind: doc
---
# NS_HeavyDeathPuff

- asset_path: /Game/RogueRigs/FX/NS_HeavyDeathPuff (Niagara system, no Blueprint; same folder and same three M_CartoonBlast materials as [[bp-robot-death-puff]])
- parent_class: UNiagaraSystem
- purpose: hides the 0.15 s shrink of a dying heavy, 248 cm tall and about 150 cm wide ([[enemy-death]]); same emitters and timing as [[bp-robot-death-puff]], lengths 3x.
- components (task-436-02, provisional, values read back): the 4 CPU sprite emitters of the robot puff, 18 sprites per death, position offset z +130 cm (kept from task-297-01, top of the heavy) with AddVelocity origin offset z +130 so the throw stays flat:
  - Lines: size 21 x 210 to 360 cm, ring radius 210 cm, speed 1650 to 2400 cm/s.
  - Core: size 840 cm.
  - Blobs: size 102 to 150 cm, ring radius 150 cm, speed 1350 to 1950 cm/s.
  - Puffs: size 126 to 198 cm, ring radius 225 cm, speed 120 to 270 cm/s.
  - same as the robot puff: counts, burst times, lifetimes, drag, colours, sort order, shadows off, gravity 0.
- one_shot: identical to the robot puff (Once, loop duration 0.2 s, inactive response Complete, last particle at about 0.42 s): the pooled component returns after it dies.
- colour: as the robot puff, red included (owner 2026-11-05, [[readability-rules]]; [[color-code]]).
- events: none
- logic_in_bp: none
- assets: used by BP_Heavy as DeathPuffSystem since task-297-02.
- verified: compile state UpToDate, 0 errors, 0 warnings; values above read back; PIE start without a LogNiagara error. Not verified: the look in game (owner validates, task-436 A7, no capture); the +130 offset assumes the heavy actor origin is near its mid-height; cost not judged.
- owner_review: feet and shins briefly visible below the former puff (0.04 to 0.09 s) accepted, no rework (owner 2026-10-23, "On accepte, les pieds visibles ne gênent pas"); not reviewed again on the new look.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-robot-death-puff]] — the robot flash it scales, and its materials.
- [[readability-rules]] — the owner line allowing red here.
- [[enemy-death]] — the design of the death puff.
- [[color-code]] — colours.
- [[bp-heavy]] — the Blueprint that uses it.
