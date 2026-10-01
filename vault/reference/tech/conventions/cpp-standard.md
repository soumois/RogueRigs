---
id: cpp-standard
description: "C++ rules for agents: Epic coding standard naming, protected over private, folders, includes, UPROPERTY and data rules, logging, header comments, engine-source checks."
parent: conventions-index
kind: rule
applies_to: C++ code
---
# C++ standard

Base: the Epic C++ coding standard for Unreal Engine. Project-specific rules below win when they differ.

## Naming (Epic)
- Type prefixes: `U` UObject, `A` AActor, `S` Slate widget, `I` interface, `F` struct and plain class, `E` enum, `T` template.
- PascalCase for types, functions, variables; `b` prefix for booleans (`bIsStunned`).
- No abbreviations except established engine ones (`ASC`, `GE`, `GA`).
- Project prefix `RR` after the type prefix (`ARRArenaGrid`, `URRWeaponAttributeSet`); see [[naming]].
- File name = class name without the type prefix (`RRArenaGrid.h` for `ARRArenaGrid`).

## Access
- Default to `protected` for member variables and helper functions; never `private`. Public only for the API other systems call.
- Expose to Blueprints only what the BP layer needs ([[cpp-bp-split]]).

## Layout
- One folder per system: `Source/RogueRigs/<System>/` (e.g. `Grid/`, `Weapons/`, `GAS/`).
- Include with module-relative paths (`#include "Grid/RRArenaGrid.h"`); do not add entries to `PublicIncludePaths`.
- Forward-declare in headers; include in `.cpp`.
- UObject member pointers: `TObjectPtr<>` with `UPROPERTY()`.

## Data
- No tunable value hard-coded: tunables come from CSV/DataTables or `UPROPERTY(EditDefaultsOnly)` defaults.
- No AI call at runtime.
- Log through the `LogRogueRigs` category; no `GEngine->AddOnScreenDebugMessage` left in committed code.

## Comments
- Every class and public or protected function in a header has a short comment: what it does, and when to call it. This replaces per-class docs; the system doc lives in `tech/systems/`.

## Engine knowledge
- Target Unreal Engine 5.8. When unsure of an API, read the engine source under `C:\Program Files\Epic Games\UE_5.8\Engine\Source` or `Engine\Plugins` before writing code; never rely on memory for deprecated or changed APIs (see [[gas-api-notes]]).
- Build with the command line in [[build-toolchain]], editor closed ([[editor-workflow]]).

Reference: https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine

## Relations
- [[conventions-index]] — parent index.
- [[naming]] — project prefixes and tag roots.
