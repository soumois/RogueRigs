---
id: task-373-02-doc-extractor-rotor-spin
description: "Doc: update sys-objectives-shuttle and bp-extraction-shuttle for the rotor spin."
parent: task-373-extractor-rotor-spin
kind: task
status: in-progress
---
# Extractor: the four rotors spin: doc

- goal: The docs describe the spin tunables, directions, getter and event.
- agent: doc
- depends_on: task-373-01-cpp-extractor-rotor-spin
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md)
- expected_output: nodes updated; one commit.
- done_when: tunables, directions, GetRotorSpeedAlpha and OnRotorSpeedChanged documented with the commit hash; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T23:24:32Z
- waiting_for: none
- result: open

## Relations
- [[task-373-extractor-rotor-spin]] — parent global task.
