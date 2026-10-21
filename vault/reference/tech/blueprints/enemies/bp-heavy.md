---
id: bp-heavy
description: "BP_Heavy, Blueprint of ARRHeavy: steel-blue 2x body with hazard stripes, telegraph glow and tilt; slam zone drawn by the ground telegraph."
parent: blueprints-enemies-index
kind: doc
---
# BP_Heavy

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_Heavy
- parent_class: ARRHeavy (/Script/RogueRigs.RRHeavy)
- purpose: the slam robot; visuals only, behaviour and slam in C++ ([[sys-enemies-heavy]]).
- components: inherited C++ components (capsule 48 x 124 left as C++ sets it) plus rigid primitives attached to the capsule, all NoCollision, no dynamic shadow: BodyMesh (engine Cylinder, scale 0.96, 0.96, 2.48 = twice the chaser, MI_GB_Heavy), StripeA / StripeC (Cylinder 0.98, 0.98, 0.18 at z -70 / -26, MI_GB_Chaser yellow), StripeB / StripeD (same at z -48 / -4, MI_GB_HazardBlack), EyeLeft / EyeRight (Sphere 0.2 at (46, -/+18, 80), MI_GB_Eye), LogoPlate (Cube 0.04, 0.5, 0.5 at (47, 0, 30), MI_GB_Chaser). ShockRing (hidden since task-123-01) was deleted in task-124-02.
- exposed_defaults: XPPickupClass = BP_PickupXP_C; all other C++ defaults kept.
- events: OnTelegraphStart (eyes and stripes A, C switch to MI_GB_HeavyGlow, BodyMesh pitch 14); OnSlam(Location, Radius) (empty, kept bound); OnRecover (materials back, pitch 0). No event shows, scales or hides ShockRing any more (task-123-01): the slam zone is the ground telegraph ([[ground-telegraph]]).
- logic_in_bp: visuals only (material swaps and body pitch), no gameplay.
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Heavy (new, steel blue 0.15, 0.35, 0.7), MI_GB_HazardBlack (new, 0.02), MI_GB_HeavyGlow (new, instance of M_GreyBoxGlow, orange); MI_GB_HeavyTelegraph (task-028-02, instance of M_GreyBoxTranslucent, Color 1, 0.35, 0, Opacity 0.3); MI_GB_Chaser, MI_GB_Eye reused.
- verified: PIE with the cheat route (bInvulnerable; spawner shortened on the PIE instance): 5 heavies alive (cap) among chasers; a heavy placed beside the miner went Chase, telegraph (BodyMesh pitch 14, eye MI_GB_HeavyGlow), Slam (ShockRing visible, scale 4.4 = 220 / 50), Recover (ring hidden, eye MI_GB_Eye, pitch 0), Chase; no game log error. Heavies far from the miner die to its weapons before reaching it. Slam damage on a vulnerable miner not re-measured here (C++ tests).
- gotcha: write_graph_dsl events use the full type id (Heavy|EventOnSlam); components read with (Variables|Default|GetName). The level-up screen pauses PIE: send ChooseUpgrade 0 ([[sys-cheats]]). An editor session open before the spawner change keeps stale weights on the placed instance until the map reloads (the saved actor file does not override them).
- verified_028: PIE (task-028-02): ShockRing RelativeScale3D (8, 8, 0.02) read on a live heavy, no Blueprint runtime error.
- task-124-02: ShockRing component deleted (nothing referenced it: EventGraph and construction script read back, find_nodes ShockRing empty, no variable); compiled and saved. Components read back: StripeA-D, EyeLeft, EyeRight, LogoPlate, BodyMesh plus C++ ones.
- task-123-01: ShockRing show/scale/hide nodes removed from OnTelegraphStart, OnSlam, OnRecover (stray old nodes of OnSlam deleted); component kept then, hidden, because `RRHeavyIntegrationTests.cpp` requires it by name. Read back: no ShockRing, SetVisibility or Delay left in the EventGraph.
- readability (task-266-01, provisional, owner 2026-10-20): MI_GB_Heavy Color lowered from (0.15, 0.35, 0.7) to (0.12, 0.28, 0.56) against burn; see [[bp-robot-readability]].
- owner_agent: agent-bp

## Relations
- [[bp-robot-readability]] — the colour tweak against burn.
- [[blueprints-index]] — parent index.
- [[sys-enemies-heavy]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
- [[bp-chaser]] — the sibling whose size it doubles.
- [[ground-telegraph]] — draws the slam zone now.
