# ChatGPT prompt: end-of-level stats screen mockup (levels 1 to 3)

Task: task-378. Paste the prompt below into ChatGPT (image generation) with ONE attachment:
- `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp` (the Spare Parts screen, palette and look reference, Image 1).

Output wanted: one 16:9 full-screen UI mockup. Sample values only; texts in English.

## Prompt (verbatim)
```
Create a full-screen 16:9 game UI mockup (1920 x 1080 look) of the end-of-level stats screen of a top-down roguelite auto-shooter, shown when the player reaches the extraction shuttle after clearing level 1 of 4.

Use the attached image (Image 1, the "SPARE PARTS" upgrade shop screen of the same game) as the exact reference for palette, materials and UI style. Keep the same look: dark gunmetal and teal riveted metal panels with visible bolts and rivets, rusty edges and scratches, thin pipes in the corners, a title in a bold chunky industrial sans font in off-white with a thin dark outline, flanked by small horizontal side bars with a bolt at each end and a spaced small caps line under it, glowing violet ore crystals as the only saturated accent, and a rusty cream-white metal plate button with teal riveted ends and dark bold text, exactly like the CONTINUE button of the reference. Do not copy the cards, the tier tabs, the prices, the reroll button or the SOLD card: this is a different screen.

Background: the game arena seen from above, frozen (the game is paused): dark floor plates, grey-violet rocks with a few violet ore crystals, small dust. The whole background is darkened to about 15 percent brightness, almost black, so the panels read clearly. No characters in focus, no UI of the game on it.

Layout, centred, top to bottom:
1. Title bar in the style of "SPARE PARTS": the title "LEVEL 1 CLEARED" with the two small side bars, and under it a small teal subtitle line "Next up: level 2 of 4".
2. One wide riveted gunmetal and teal panel (same frame as the shop panel of the reference, empty dark inside) holding, from top to bottom:
   a. A recap strip with two lines of text, each with a small violet ore crystal icon like the one at the top left of the reference: "Ore secured this level: 34" and "Ore in this run: 61".
   b. Two columns side by side under it, separated by a thin riveted divider. Left column label "Robots destroyed", right column label "Damage dealt". Under each label a plain left-aligned list, one line per entry, "Name: value", in clean off-white text on the dark panel, with no icons at all:
      Left: "Chaser: 8", "Shooter: 5", "Heavy: 2", "Kamikaze: 3".
      Right: "Burst rifle: 125".
3. Under the panel, centred: one rusty cream-white metal button reading "NEXT LEVEL", same style as the CONTINUE button of the reference.

Rules: show only the elements listed above. Invent no other stat, rule, icon, rank, medal, timer, score, tooltip or button; no per-robot icons, text names only; no weapon icons; no reroll, no cards, no shop. The palette is the reference palette: gunmetal, teal, rust, cream, off-white text, violet only for ore. No red, yellow, green or orange in the UI. All text sharp, readable and spelled exactly as written. Flat clean UI render, same illustration style as the reference, no photo look, no watermark, no logo.
```

## After the answer
Save the owner image under `Art/Source/level-end-screen/reference/` and note it in the node `art-prompts-level-end-screen`. Sample values are placeholders; real values come from the run recap.
