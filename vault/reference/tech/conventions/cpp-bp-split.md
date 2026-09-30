---
id: cpp-bp-split
description: "Split between C++ and Blueprints: logic in C++, Blueprints as a thin derived layer, every Blueprint documented."
parent: conventions-index
kind: doc
---
# C++ vs Blueprint split

- All gameplay logic is in C++.
- Blueprints are a thin layer:
  - classes derived from C++ classes, for placement in levels and asset references;
  - events that call C++ functions;
  - a few visual effects, such as camera shake.
- UMG widgets and Niagara hookups are made by the BP agent through Unreal MCP.
- Every Blueprint has a doc node in [[blueprints-index]]; a task that changes a Blueprint is done only when its doc is updated ([[_authoring]]).
- C++ has no per-class doc: one doc per system in [[systems-index]]; function detail lives in header comments.

## Relations
- [[conventions-index]] — parent index.
- [[editor-workflow]] — which agent works with the editor open.
