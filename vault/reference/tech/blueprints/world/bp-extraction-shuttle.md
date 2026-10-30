---
id: bp-extraction-shuttle
description: "BP_ExtractionShuttle, Blueprint of ARRShuttle: Rustwing extractor body with four rotors and floor chevrons; open for its meshes, rotor positions, material and event."
parent: blueprints-world-index
kind: doc
---
# BP_ExtractionShuttle

- asset_path: /Game/RogueRigs/Blueprints/Objectives/BP_ExtractionShuttle
- parent_class: ARRShuttle (/Script/RogueRigs.RRShuttle)
- purpose: the extraction shuttle landed by the run flow ([[sys-objectives]], [[extraction]]); visuals only. Shows the imported extractor since task-369-03 (4fead33b); the white box and its teal glow pulse are gone.
- components: Chevron0..3 (StaticMeshComponent, engine Plane scale 0.8, MI_Chevron, hidden, no collision, no shadow) added by task-027-02, kept. Inherited BoardZone (UBoxComponent, C++, task-377-01 ee1a7db8, [[sys-objectives-shuttle]]): the level-ending box, attached to ShuttleRoot (not scaled by Mesh); C++ defaults kept (1608 x 1608 x 400 uu, relative location (0, 0, 200)) until the owner edits it; he adjusts size, position and rotation in the Details panel or the viewport (owner 2026-10-30, provisional); values he sets are `open`. Inherited Mesh = SM_Extractor_Body, relative location 0, scale 4 since task-375-02 (b112688b; was 1, before task-369-03 z 40, scale 2.4, 1.6, 0.8), no material override (M_Extractor is on the mesh slots; MI_GB_Shuttle no longer used). Rotors (relative scale 1, relative locations unchanged) and the four wash components (attached to Mesh, scale 1) follow the Mesh scale x4; wash FloorHeight is computed at BeginPlay, so it grows about 4x by itself (about 247-298 uu). Inherited RotorFL/FR/RL/RR (C++, [[sys-objectives-shuttle]]) with SM_Extractor_Rotor_FL|FR|RL|RR at relative locations (uu, body frame) FL (32.808, -66.197, 61.866), FR (32.967, 66.619, 61.905), RL (-64.777, -62.144, 74.563), RR (-64.770, 62.529, 74.387). Rotors spin from C++ since task-373-01 (d88a3adf, [[sys-objectives-shuttle]]); the Blueprint must not rotate them itself. Since task-374-02 (178508af): 4 NiagaraComponents WashFL, WashFR, WashRL, WashRR (NS_ExtractorRotorWash, auto-activate) on the inherited Mesh, not on the spinning rotors, at relative locations FL (32.808, -66.197, 61.866), FR (32.967, 66.619, 61.905), RL (-64.777, -62.144, 74.563), RR (-64.770, 62.529, 74.387).
- exposed_defaults: C++ defaults kept. Set as `ShuttleClass` in `RunFlowConfig` of [[bp-rr-game-mode]].
- events: OnCountdown(Remaining) (every ticked frame) places Chevron0..3 on the floor at 150, 300, 450, 600 uu from the miner along the miner-to-shuttle line (yaw of MakeRotFromX, z = shuttle z + 6), each visible only if it is more than 600 uu short of the shuttle (threshold length - 600, was 150 before task-375-02, so none is drawn under the x4 hull; scale and spacing unchanged); chevron arrow direction along the plane U axis not viewed on screen. The former GlowAmount call was removed in task-369-03 (orphan math nodes left in the graph). OnLanded, OnExtracted and OnMissed are not implemented. OnRotorSpeedChanged(Alpha) (C++ event, alpha 0.6 on the ground, 1 in flight) sets User.Intensity = Alpha on the four wash components (SetNiagaraVariableByString Float). BeginPlay sets User.FloorHeight = component world Z minus actor Z (origin on the floor) once per component, then reads GetRotorSpeedAlpha once and sets Intensity. No Event Tick (default node deleted).
- logic_in_bp: display only (one event).
- assets: NS_ExtractorRotorWash and M_ExtractorRotorWash ([[bp-extractor-rotor-wash]], aeb5cb49). /Game/RogueRigs/Art/Extractor/ (task-369-01, eaf3a3c3): SM_Extractor_Body (45,420 tris, bounds 196.55 x 201.0 x 86.1 cm, pivot bottom centre), SM_Extractor_Rotor_FL|FR|RL|RR (736, 684, 684, 630 tris; total 48,154), T_Extractor_BaseColor|Metallic|Normal|Roughness (normal green flipped; metallic and roughness sRGB off), M_Extractor (texture parameters BaseColor, Metallic, Roughness, Normal; on every slot). Imported with Tools/unreal/import_extractor_meshes.py run from the editor Cmd box, because StaticMeshTools.import_file gave a 100x too small mesh ([[unreal-mcp-quirks]]). /Game/RogueRigs/FX/M_Chevron (M_GroundRing copy, mask |v| + u = 0.3 band, params Color, Opacity), MI_Chevron (teal, 0.8).
- owner_agent: agent-bp
- verified: task-016-03 PIE (old box): landed as BP_ExtractionShuttle_C_0, boarding gave the success screen, a 5 s countdown gave the run-lost screen, no log error. task-374 PIE on a landed extractor: 4 wash components, Intensity 0.6 on the ground, FloorHeight 61.866/74.387 read, no LogNiagara error; arrival extractor, Intensity 1 at takeoff, FR/RL values and the look unverified. task-375-02: not verified in PIE or by capture (owner refused visual checks). task-369: not verified in PIE or on screen (owner refused capture and tests); material compile errors, Y mirror against the scene, Nanite state open; no collision generated on the meshes.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-objectives]] — the C++ class and events.
- [[bp-rr-game-mode]] — holds it in RunFlowConfig.
- [[unreal-mcp-quirks]] — the import scale trap met by task-369-01.
- [[task-369-extractor-level-exit]] — the task that changed it.
- [[bp-extractor-rotor-wash]] — the rotor-wash Niagara system its four components play.
- [[task-374-extractor-rotor-wash]] — the task that added the wash.
- [[task-375-extractor-scale-x4]] — the task that scaled the Mesh x4 and moved the chevron threshold.
- [[task-377-extractor-board-box]] — the task that added the BoardZone box and removed the board radius.
