---
id: task-099-02-company-subtitles
description: "Subtitle widget at the bottom centre, placed by WBP_HUD, showing the company voice lines; the old alert line text is removed from WBP_HUD."
parent: task-099-company-subtitles
kind: task
status: done
---
# Subtitle widget

- goal: a separate subtitle widget (one line, speaker prefix) placed at the bottom centre above the XP bar by WBP_HUD, driven by the C++ subtitle events; WBP_HUD keeps no subtitle logic.
- agent: bp
- depends_on: task-099-01-company-subtitles, task-098-01-weapon-info-centred
- inputs: wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), sys-text-content (vault/reference/tech/systems/sys-text-content.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: create WBP_Subtitles (own widget Blueprint, own doc node in tech/blueprints): one wrapped text line, speaker name in front, bottom centre above the XP bar and clear of the weapon info, the minimap and the horde banner, shown on the C++ OnSubtitleShown event and hidden on OnSubtitleCleared (no tick polling); WBP_HUD instantiates it as a sub-widget and only places it (owner rule 2026-10-10: WBP_HUD is the single root containing all sub-widgets, only placing them; each sub-widget holds its own logic); remove AlertLineText and the functions that only served the alert line (ShowAlertLine, HideAlertLine, NoteShuttle, NoteConsole, NoteAlertEvents logic) from WBP_HUD, keeping the banners and icons; update wbp-hud and the new widget doc; no screenshot calls; group the work, commit after each group; quirks: real newline in pins, dirty-asset trick before saving.
- done_when: widget compiled 0 errors and saved; WBP_HUD only places it; old alert line logic removed and the rest compiles; wbp-hud and the new doc updated; lint clean; committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: WBP_Subtitles (own logic, bound to OnSubtitleShown/Cleared, no tick) placed by WBP_HUD at the bottom centre; old alert line logic removed from WBP_HUD; compiled and saved, not seen on screen. Commits ac2cb86, 0c25397. Cost counted in the resumed bp agent.

## Relations
- [[task-099-company-subtitles]] — parent global task.
