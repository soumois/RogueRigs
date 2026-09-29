# Rogue Rigs — instructions for AI agents

Rogue Rigs is a top-down roguelite auto-shooter built with Unreal Engine 5.8, C++ first.

## Source of truth
All design, agent definitions, technical docs, production plans and tasks live in `vault/`.
1. Read `vault/root.md`, then `vault/_navigation.md`, and follow them strictly: descend through the indexes, never search the vault.
2. Before creating or modifying anything in `vault/`, read `vault/_authoring.md`.
3. A value marked `open` is undecided. Never invent it: report it.

## Hard rules
- No AI calls at runtime. AI is a development tool only.
- Gameplay logic in C++ (`Source/RogueRigs/`). Blueprints are a thin layer: placement, events calling C++, a few visual effects.
- Every Blueprint change updates its doc node in `vault/reference/tech/blueprints/`.
- Never put secrets in the repository.
- After editing the vault: `node vault/scripts/lint.js --write`, fix all problems, then commit.

## Build
Unreal Engine 5.8 with the MSVC 14.50 toolchain (Visual Studio 2026). Project file: `RogueRigs.uproject`.
