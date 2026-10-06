---
id: bp-objective-console
description: "BP_ObjectiveConsole, Blueprint of ARRConsole: white cylinder with a teal glow that brightens with the hack progress; open for its mesh, material and event."
parent: blueprints-index
kind: doc
---
# BP_ObjectiveConsole

- asset_path: /Game/RogueRigs/Blueprints/Objectives/BP_ObjectiveConsole
- parent_class: ARRConsole (/Script/RogueRigs.RRConsole)
- purpose: the command console dropped by the run flow ([[sys-objectives]]); visuals only.
- components: HackRing (StaticMeshComponent, engine Plane, MI_GroundRingTeal, relative z 4, no collision, no shadow, scale 0.01 at rest) added by task-027-02; inherited Mesh: StaticMesh = engine Cylinder, scale (1, 1, 1.5), relative location z 75, material slot 0 = MI_GB_Console, NoCollision, no dynamic shadow.
- exposed_defaults: C++ defaults kept (zone, mark radius, hack duration, drain). Set as `ConsoleClass` in `RunFlowConfig` of [[bp-rr-game-mode]].
- events: OnHackProgress(Progress, bHacking) calls SetScalarParameterValueOnMaterials(Mesh, GlowAmount, 0.15 + Progress * 3.5): the teal glow grows with the gauge; it also sets HackRing relative scale to 0.01 + Progress x GetZoneRadius / 44 in X and Y (the ring edge sits at 0.88 of the plane half width, so it reaches the zone circle at progress 1). OnMarkedRocksChanged and OnHackStateChanged are not implemented (the rock outline and the HUD carry that feedback).
- logic_in_bp: display only (one event).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Console (parent M_GreyBoxGlow: Color white, GlowColor teal 0/0.85/1, GlowAmount 0.3).
- owner_agent: agent-bp
- verified: task-016-03 PIE: dropped as BP_ObjectiveConsole_C_0, hack completed (state Complete, progress 1) with no log error; glow not viewed on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-objectives]] — the C++ class and events.
- [[bp-rr-game-mode]] — holds it in RunFlowConfig.
