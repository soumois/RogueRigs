---
id: tech-index
description: "How the game is built: coding conventions, one doc per C++ system, one doc per Blueprint, and pipelines. Open before touching code or assets."
parent: root
kind: note
---
# Technical documentation

Engine: Unreal Engine 5.8, C++ first. Project created from the Top Down template; the Twin Stick variant is the starting base. Gameplay logic lives in C++; Blueprints stay a thin layer.

## Children
- [[conventions-index]] — naming, C++ vs Blueprint split, doc rules. Read before writing any code.
- [[systems-index]] — one doc per C++ system: responsibilities, main classes, data flow, entry points.
- [[blueprints-index]] — one doc per Blueprint, so nobody has to open a binary asset to know what it does.
- [[performance-index]] — performance risks at 150–200 enemies and the M1 stress test.
- [[pipelines-index]] — asset pipeline, run logs, build.

## Relations
- [[root]] — the vault map.
