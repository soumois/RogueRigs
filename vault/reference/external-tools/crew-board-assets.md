---
id: crew-board-assets
description: "Crew board mascots and palette: one pixel-art icon per agent, how it was generated and converted, how to add one. Open to change an icon."
parent: external-tools-index
kind: doc
---
# Crew board: mascots and palette

- responsibility: give each agent type an icon and a colour on the board.
- main_files: `Art/Source/crew-icons/` (prompts, run logs, chosen sheets, 96 px icons), `Tools/art/crew_icons_cut.js`, `Tools/art/crew_icon_one.js`, `Tools/crew-board/build_icons.js`, `Tools/crew-board/hooks/icons.ts` (generated, do not edit).
- entry_points: `node Tools/crew-board/build_icons.js Art/Source/crew-icons/icons 32` rewrites `icons.ts`.
- data: eleven mascots with a colour each: cpp blue, bp green, art pink, 3d violet, level-design lime, text gold, style-guide orange, planner cyan, executor yellow, doc coral, claude grey.
- depends_on: the art agent and `Tools/art/kie_image.js` (GPT Image 2).
- implements: none.
- gotchas: the pane draws pictures as SVG only (the image element is terminal-only), so each icon is converted to 32x32 coloured rectangles, about 10 KB each; keep a page of cards under a few hundred KB.

## Generation
One sheet of ten mascots (prompt `01-sheet.txt`, owner screenshot as style reference), then one run for `doc` (`02-doc.txt`, the sheet as reference). Flat `#15171c` background, same body and pixel scale. The sheet is cut by `crew_icons_cut.js`, a single image by `crew_icon_one.js`, both nearest-neighbour to 96x96 with the background exactly `#15171c`.

## Add an agent
Prompt a new mascot with the same body, cut it to `icons/<agent>.png`, run `build_icons.js`, add `<agent>: { glyph, color }` to `AGENTS` in `register.tsx`.

## Palette
Dark navy plates, slate edges, yellow frame; status colours: to do grey, in progress yellow, wait violet, done green, cancel red; tokens and units orange. It started from the game colours of [[color-code]].

## Relations
- [[external-tools-index]] — parent index.
- [[crew-board]] — the mod that draws them.
- [[agent-art]] — the agent that generates the images.
- [[color-code]] — the game colours the palette started from.
