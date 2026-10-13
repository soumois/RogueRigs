---
id: bp-extraction-shuttle
description: "BP_ExtractionShuttle, Blueprint of ARRShuttle: white flat box with a teal glow that pulses each second of the countdown; open for its mesh, material and event."
parent: blueprints-world-index
kind: doc
---
# BP_ExtractionShuttle

- asset_path: /Game/RogueRigs/Blueprints/Objectives/BP_ExtractionShuttle
- parent_class: ARRShuttle (/Script/RogueRigs.RRShuttle)
- purpose: the extraction shuttle landed by the run flow ([[sys-objectives]], [[extraction]]); visuals only.
- components: Chevron0..3 (StaticMeshComponent, engine Plane scale 0.8, MI_Chevron, hidden, no collision, no shadow) added by task-027-02; inherited Mesh: StaticMesh = engine Cube, scale (2.4, 1.6, 0.8), relative location z 40, material slot 0 = MI_GB_Shuttle, NoCollision, no dynamic shadow.
- exposed_defaults: C++ defaults kept. Set as `ShuttleClass` in `RunFlowConfig` of [[bp-rr-game-mode]].
- events: OnCountdown(Remaining) (every ticked frame) calls SetScalarParameterValueOnMaterials(Mesh, GlowAmount, 1 + 2 * Fraction(Remaining)) and places Chevron0..3 on the floor at 150, 300, 450, 600 uu from the miner along the miner-to-shuttle line (yaw of MakeRotFromX, z = shuttle z + 6), each visible only if it is more than 150 uu short of the shuttle; chevron arrow direction along the plane U axis not viewed on screen. OnLanded, OnExtracted and OnMissed are not implemented.
- logic_in_bp: display only (one event).
- assets: /Game/RogueRigs/FX/M_Chevron (M_GroundRing copy, mask |v| + u = 0.3 band, params Color, Opacity), MI_Chevron (teal, 0.8); /Game/RogueRigs/Materials/GreyBox/MI_GB_Shuttle (parent M_GreyBoxGlow: Color white, GlowColor teal, GlowAmount 1.5).
- owner_agent: agent-bp
- verified: task-016-03 PIE: landed as BP_ExtractionShuttle_C_0, boarding gave the success screen, a 5 s countdown gave the run-lost screen, no log error; glow not viewed on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-objectives]] — the C++ class and events.
- [[bp-rr-game-mode]] — holds it in RunFlowConfig.
