---
id: editor-workflow
description: "Editor state per agent: C++ header changes with the editor closed, BP work with it open for MCP; the Orchestrator sequences batches."
parent: conventions-index
kind: doc
---
# Editor workflow

- The C++ agent compiles header changes with the editor closed. Practice retained for Live Coding; not verified in engine source.
- The BP agent works with the editor open, because Unreal MCP needs it.
- The Orchestrator sequences C++ batches and BP batches; otherwise Live Coding and the MCP get in each other's way.
- Integration and debugging stay human, even when agents write the code.

## Relations
- [[conventions-index]] — parent index.
- [[build-toolchain]] — compiler and engine versions.
