---
id: sys-hud-locator
description: "Objective locator: pure edge-arrow functions of the HUD widget (task-032-01). Open to change the objective arrow."
parent: sys-hud
kind: doc
---
# HUD — locator

- Objective locator (task-032-01, [[objective-console]] edge arrow; pure, sent by `OnLocatorViewChanged` since task-103-01, [[sys-hud-events]]): `GetObjectiveTarget(OutTarget, OutWorldLocation)` (`ERRLocatorTarget` None / Console / Shuttle: the console of the bound run flow while not `Complete`, else the landed shuttle, None before the drop, between the hack and the landing and after the run end); `GetObjectiveLocator(OutLocator, EdgeMargin = 48)` fills `FRRObjectiveLocator` {`Target`, `WorldLocation`, `bOnScreen`, `ScreenPosition`, `EdgePosition`, `ArrowAngle`, `ViewportSize`} with `GetLocatorView` (the bound miner's camera component, current inside its `TransformUpdated`; else the owning player's `GetCameraCacheView`; render-only camera shake ignored), in viewport widget units (pixels / viewport scale, top-left origin): `EdgePosition` = the target when on screen, else where the line from the viewport center meets the viewport inset by `EdgeMargin`; `ArrowAngle` degrees clockwise from screen right (render angle of a right-pointing arrow). False without a target, a view or a game viewport (headless). Static helpers `ProjectToViewport(View, ViewportSize, World, Out)` (perspective, a point behind the camera pushed far out along its direction) and `ComputeEdgeLocator`. Test `RogueRigs.HUD.ObjectiveLocator` ([[sys-automation-tests]]).

## Relations
- [[sys-hud]] — parent hub: summary and verdict.
- [[objective-console]] — referenced above.
- [[sys-hud-events]] — referenced above.
- [[sys-automation-tests]] — referenced above.
