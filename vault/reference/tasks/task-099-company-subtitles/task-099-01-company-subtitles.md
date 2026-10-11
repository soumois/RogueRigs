---
id: task-099-01-company-subtitles
description: "Company voice lines are shown as one-line subtitles at the bottom centre, from a separate widget; no more stacking on the horde banner."
parent: task-099-company-subtitles
kind: task
status: done
---
# Subtitles for the company voice

- goal: Company voice lines are shown as one-line subtitles at the bottom centre, from a separate widget; no more stacking on the horde banner.
- agent: cpp
- depends_on: task-097-01-explosion-visual
- inputs: sys-text-content (vault/reference/tech/systems/sys-text-content.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), sys-hud (vault/reference/tech/systems/sys-hud.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), text-content-events (vault/reference/tech/pipelines/text-content-events.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: owner playtest: the company line "Security response escalated..." overlaps the HORDE INCOMING banner; it is a voice line, so it becomes a subtitle. Decisions 2026-10-10: one subtitle widget at the bottom centre above the XP bar, one line at a time (a new line replaces the current one), speaker name in front ("Kessler-Voss: ..."), duration 4 s + 0.06 s per character; it serves the 5 company alert lines now (console_dropped, bonus_chest_dropped, horde_incoming, extraction_ready, shuttle_countdown shown once for its duration as in task-085) and later the Rook and Vesna barks (data already loaded by URRTextContentSubsystem: design the API with a speaker id and a line so barks can use it, do not display barks yet). The HORDE INCOMING banner stays. HUD architecture rule (owner): WBP_HUD stays the single root widget that contains every sub-widget and only places them; each sub-widget holds its own logic; code initialises only WBP_HUD. This subtask is the C++ part: a subtitle subsystem or component (queue of one, current line, remaining time, speaker, delegates/events OnSubtitleShown / OnSubtitleCleared callable by the Blueprint), the five alert events call it instead of URRHUDWidget alert-line text (keep GetAlertLine for the loader), tests (replace, duration, clear, speaker prefix). Then task-099-02 (bp) builds the widget. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs sys-text-content and sys-hud-pressure updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 76, cache write 91607, cache read 2397902, output 39020
- runs: 3
- milestone: M2
- result: URRSubtitleSubsystem (Text/): Show/ShowAlertLine/Clear, replace rule, 4 s + 0.06 s per char, delegates OnSubtitleShown/OnSubtitleCleared, 5 alert events wired; 231/231 tests. Commit 8a0fec0. Step 2 (bp widget) next.

## Relations
- [[task-099-company-subtitles]] — parent global task.
