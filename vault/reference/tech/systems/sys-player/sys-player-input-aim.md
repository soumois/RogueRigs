---
id: sys-player-input-aim
description: "Player input axes, mouse and stick aim, dig trigger by contact. Open to change controls or aiming."
parent: sys-player
kind: doc
---
# Player — input aim

  - `ApplyMoveInput(FVector2D)`, `ApplyStickAimInput(FVector2D)`, `UseMouseAim()`, `GetMouseAimPoint(FVector&)` (last cursor point on the aim plane, mouse aim only); `GetAimDirection()`, `IsUsingMouseAim()`, `GetRRAbilitySystemComponent()`.
  - Input axes: screen axes, X = right, Y = up, rotated by the arm's world yaw. A raw gamepad stick needs no modifier; WASD needs the usual swizzle on W/S and negate on S/A. Actions are bound on `Triggered`.
  - Dig trigger (end of `Tick`, arc mining by contact, task-029-05): `ApplyMoveInput` stores the world-space move input of the frame; `Tick` consumes it (zero when idle). Contact = a block cell whose square (2D) is within capsule radius + `DigContactMargin` of the miner location (no push needed); `IsTouchingRock()` (BlueprintPure) reports it. Wall direction = opposite of the mean contact normal (nearest block point to miner; facing direction if the normals cancel). Arc direction = the move input when longer than `DigMinMoveInput` and pointing into the wall, else the wall direction. Contact: `Drone->RequestArcDig(ArenaGrid, ArcDirection)`; no contact or dead: `Drone->ReleaseDig()` ([[sys-drone]]). Grid assumed unrotated.
  - Mouse aim: each tick the cursor ray is intersected with a horizontal plane at the capsule bottom + `MouseAimPlaneHeight`; a stick input switches to stick aim, any mouse aim trigger switches back.

## Relations
- [[sys-player]] — parent hub: summary and verdict.
- [[sys-drone]] — referenced above.
