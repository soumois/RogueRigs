---
id: bp-objective-console
description: "BP_ObjectiveConsole, Blueprint of ARRConsole: white cylinder whose teal glow follows the hack progress; zone circle drawn by the C++ ground telegraph."
parent: blueprints-world-index
kind: doc
---
# BP_ObjectiveConsole

- asset_path: /Game/RogueRigs/Blueprints/Objectives/BP_ObjectiveConsole
- parent_class: ARRConsole (/Script/RogueRigs.RRConsole)
- purpose: the command console dropped by the run flow ([[sys-objectives]]); visuals only.
- components: no own component (the HackRing plane added by task-027-02 was removed by task-113-02; the zone circle is the inherited GroundTelegraph of [[sys-ground-telegraph]]); inherited Mesh: StaticMesh = engine Cylinder, scale (1, 1, 1.5), relative location z 75, material slot 0 = MI_GB_Console, NoCollision, no dynamic shadow.
- exposed_defaults: C++ defaults kept (zone, mark radius, hack duration, drain). Set as `ConsoleClass` in `RunFlowConfig` of [[bp-rr-game-mode]].
- events: OnHackProgress(Progress, bHacking) calls SetScalarParameterValueOnMaterials(Mesh, GlowAmount, 0.15 + Progress * 3.5): the teal glow grows with the gauge. The old SetRelativeScale3D(HackRing) call was removed by task-113-02. OnMarkedRocksChanged and OnHackStateChanged are not implemented (the rock outline and the HUD carry that feedback).
- logic_in_bp: display only (one event, one call).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Console (parent M_GreyBoxGlow: Color white, GlowColor teal 0/0.85/1, GlowAmount 0.3).
- owner_agent: agent-bp
- verified: task-016-03 PIE: dropped as BP_ObjectiveConsole_C_0, hack completed (state Complete, progress 1) with no log error; glow not viewed on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-objectives]] — the C++ class and events.
- [[bp-rr-game-mode]] — holds it in RunFlowConfig.
- [[sys-ground-telegraph]] — draws the hack zone circle.
