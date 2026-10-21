---
id: perf-lead-blob-shadow-instancing
description: "Lead: draw every blob shadow from one instanced static mesh per world instead of one component per actor. Not measured, not tried."
parent: perf-leads
kind: note
---
# Lead: instanced blob shadows (task-264)

- now ([[sys-blob-shadow]], task-264-02): one `URRBlobShadowComponent` per robot, miner and drone. At 400 robots: 400 translucent primitives, each a sorted mesh draw and a proxy transform update when its robot moves, plus one more child transform per root move on the game thread ([[perf-robot-components-audit]]). Not measured.
- option: a world subsystem owning one `UInstancedStaticMeshComponent` (Plane + MI_BlobShadow, same render flags); each frame it writes one instance per living, visible robot (location from the actor, Z = floor + offset, scale from the footprint) with `BatchUpdateInstancesTransforms`, hides unused instances by zero scale. Expected: one translucent draw for all discs, no per-robot child transform, no per-robot proxy; cost moves to one instance buffer upload per frame. Robots already iterate in `URRRobotSeparationSubsystem`, which could feed it.
- measure first: S0 at 400-600 robots ([[perf-stress-test-m1]]) with the per-actor discs, then with `BlobShadowDiameterScale` 0 (no disc) to bound the gain; only worth it if the difference shows on the render thread ([[perf-lead-lumen-robots]] already names it as the limit).
- drawback: one draw sorts as a single translucent object (depth order between discs and other translucent effects becomes coarse).

## Relations
- [[perf-leads]] — parent index.
- [[sys-blob-shadow]] — the per-actor component this would replace.
