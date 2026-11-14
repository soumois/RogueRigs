---
id: task-544-03-gauge-widget
description: "Bp: Horde gauge widget under the alert bar and the moved "!""
parent: task-544-horde-kill-gauge
kind: task
status: done
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: bp

- goal: Horde gauge widget under the alert bar and the moved "!"
- agent: bp
- depends_on: task-544-02-gauge-render
- inputs: level-flow (vault/reference/gdd/run/level-flow.md), the render and cut elements of task-544-02-gauge-render, umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: see done_when
- done_when: A1, A2 and A7 visible in play, the gauge built from the task-544-02 elements (or from the brief look if 02 recorded a gap); HUD otherwise unchanged.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-544
- usage: included in the executor run of task-544
- runs: 1
- milestone: M2
- started: 2026-11-14T00:31:31Z
- waiting_for: none
- result: Commits 3480f6c1, 7e? see log, final placement after owner addition: WBP_AlertObjectives HordeGauge (CanvasPanel, variable, anchors 0.5,0, alignment 0.5,0, offset top 92, 234 x 30, ZOrder -1 behind AlertBar/AlertFrame, Collapsed by default) > HordeGaugeBar (ProgressBar, anchors 0.0699,0.3 to 0.9258,0.711, flat red fill 1/0.12/0.05, dark track), HordeGaugeFrame (T_HUD_AlertFrame stretched, z1), HordeGaugeMark (T_HUD_AlertMark 15 x 31 at anchor 0.035,0.5, z2). Graphs: event OnHordeGaugeChanged (new URRHUDPanelWidget BlueprintImplementableEvent, bound natively to the run flow relay) -> ShowHordeGauge (Hidden collapses; Filling/Full RevealHordeGauge; Full starts PulseHordeGauge timer 0.04 s), RevealHordeGauge slides from -30 to 0 over 0.35 s (SlideHordeGauge) the first time; HideForOffer/HideForEnd collapse it, RestoreAfterOffer calls ShowHordeGauge. Seen in game: '!' at half of C, gauge under the bar after the mark, attached with no gap (Saved/Captures/task544_att2_1920x1080/crop.png). Owner addition (attached + slide) applied. ALERT: executor over the standard budget (cause: code reads in 01, MCP graph round trips and two capture rounds after the owner addition).

## Relations
- [[task-544-horde-kill-gauge]] — parent global task.
