---
id: bp-heavy-death-puff
description: "NS_HeavyDeathPuff, the one-shot Niagara puff, about 3x the robot puff, that hides the heavy's 0.15 s death shrink; open for its scaled values."
parent: blueprints-fx-index
kind: doc
---
# NS_HeavyDeathPuff

- asset_path: /Game/RogueRigs/FX/NS_HeavyDeathPuff (Niagara system, no Blueprint; duplicate of NS_RobotDeathPuff, same folder)
- parent_class: UNiagaraSystem
- purpose: hides the 0.15 s shrink of a dying heavy, 248 cm tall and about 150 cm wide ([[enemy-death]]); same one-shot structure as [[bp-robot-death-puff]], scaled about 3x. Not wired yet: the heavy's DeathPuffSystem is set in task-297-02.
- components: one CPU sprite emitter `OmnidirectionalBurst`, unchanged from [[bp-robot-death-puff]].
- exposed_defaults (task-297-01, provisional, values read back; robot puff in brackets): spawn count 24 [12]; sphere location radius 105 cm [35]; AddVelocity speed 180 to 780 cm/s [60 to 260], drag 0.75 (same); sprite size 70 to 190 cm uniform random [14 to 50] (rework: was 42 to 150); puff centre raised by InitializeParticle Position Offset z +130 cm (UsePositionOffset on, Simulation space; rework, was none) so the puff sits at the heavy top (248 cm) and hides it from the first frame in the top-down view; the template ScaleSpriteSize curve starts at 1.0, so sprites are full size at spawn; particle lifetime 0.2 to 0.3 s (same, covers the 0.15 s shrink); colour HDR warm orange (3, 1.3, 0.25) (same); gravity 0; AddVelocity origin offset z -20 (same).
- one_shot: loop settings identical to the robot puff (Once, inactive response Complete): the pooled component returns after the last particle dies.
- colour: same warm spark orange, a detail colour; owner default 2026-10-23, provisional ([[color-code]]).
- events: none
- logic_in_bp: none
- assets: used by BP_Heavy as DeathPuffSystem since task-297-02 (read back).
- verified: compiles (UpToDate, 0 errors, 0 warnings); NS_RobotDeathPuff read back unchanged. Not verified in game after the rework: the +130 offset assumes the heavy actor origin is near its mid-height; look and cost not judged.
- owner_review: feet and shins briefly visible below the puff (0.04 to 0.09 s) accepted, no rework (owner 2026-10-23, "On accepte, les pieds visibles ne gênent pas").
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[bp-robot-death-puff]] — the robot puff it was duplicated from.
- [[enemy-death]] — the design of the death puff.
- [[color-code]] — colours.
- [[bp-heavy]] — the Blueprint that uses it.
