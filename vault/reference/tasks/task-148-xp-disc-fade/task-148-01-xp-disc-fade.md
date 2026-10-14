---
id: task-148-01-xp-disc-fade
description: "Fade the glow disc with the pickup phase: off in the air, on in 0.2 s at landing, off when magnetised."
parent: task-148-xp-disc-fade
kind: task
status: done
---
# XP glow disc: fade with the pickup phase

- goal: Fade the glow disc with the pickup phase: off in the air, on in 0.2 s at landing, off when magnetised.
- agent: cpp
- depends_on: none
- inputs: sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: C++ in Source/RogueRigs/Pickups (ARRPickup, URRPickupSubsystem): a glow fade value 0 to 1 passed to the mesh as Custom Primitive Data (UPrimitiveComponent::SetCustomPrimitiveDataFloat on PickupMesh, no dynamic material instance): EditDefaultsOnly bool bGlowFade (default false), float GlowFadeDuration (default 0.2 s, provisional), int32 GlowFadeDataIndex (default 0); the fade target is 0 during Pop (and at pool reset: value 0), 1 at the start of Rest (landing), 0 at the start of Magnet and Collected; the fade value moves toward its target at 1 / GlowFadeDuration per second, updated only while it differs from the target and only for pickups with bGlowFade true (other pickups: no cost, no change), in the existing pickup loop of URRPickupSubsystem (no new tick); written to the primitive each time it changes; pooled reuse resets it to 0. Document in sys-pickups and sys-pickups-lifecycle. The project builds with the editor CLOSED.
- done_when: The project compiles (RogueRigsEditor Win64 Development, editor closed); no existing test broken (only update expectations a requested change breaks, no new test); sys-pickups and sys-pickups-lifecycle updated; lint clean; committed
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 30, cache write 55028, cache read 564869, output 8641
- runs: 1
- milestone: M2
- result: bGlowFade, GlowFadeDuration 0.2 and GlowFadeDataIndex 0 in ARRPickup; fade target 0 in Pop, 1 at Rest, 0 at Magnet and Collect; updated in UpdateMotion (existing loop) only for pickups with bGlowFade; reset 0 on pool reuse and release; builds; visual result needs 148-02.

## Relations
- [[task-148-xp-disc-fade]] — parent global task.
