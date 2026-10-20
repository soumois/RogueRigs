---
id: lesson-08-constraints-and-demo
description: "Lesson 08 (Oct 13), part 2: constraints turn randomness into a game, evaluator rules, GER mapping per game type, Dungeon Builder demo, No Man's Sky, meaningful variety."
parent: lessons-index
kind: note
---
# Lesson 08 — Constraints, demo, case study

## Constraints
- Pure randomness makes noise, not games ("wide as an ocean, deep as a puddle"). Constraints are the walls that turn entropy into an experience, for classic procedural generation and for agents (inconsistent character styles, flat dialogue).
- The GDD is the single source of truth and the agents' rule book translated into evaluable logic; a dedicated document per system adds constraints. The GDD is also a map: about 10,000 tokens to find the docs instead of up to a million exploring the code.
- Roguelike examples: pathfinding (every critical place reachable; an agent walks the game once, records its actions, the replay becomes a deterministic test), difficulty pacing (chance to finish a fight without dying), visual and thematic consistency. Style rules such as a fixed palette (12 colours) or a font give identity; a "juice bible" covers animation, sound and pop effects.
- Example loop: chest placed in an impassable corner, evaluator FAIL (no navigation node), refiner moves it to the centre, PASS, no human involved.

## One structure, other games
Card game: write card data, check balance rules, adjust values. Narrative: write a line, check continuity, lore and personality sheet, rewrite flagged lines. Platformer: place obstacles, check reachability and difficulty curve, adjust. What the game generates is the Generator; the rules it must follow are the Evaluator. Agents are disposable tools: rewrite or replace them freely. Aim for 80 to 90 percent, then finish by hand.

## Demo: Dungeon Builder
Generator, evaluator (plays the level in the real game), refiner, verifier (scores 0 to 5: brief fit, fairness, flow, readability). An HTML preview of the level is approved by the human before implementation; a feedback file becomes a new rule for all future levels. The bot lost 60 percent of its health: the designer chooses the target.

## Case study: No Man's Sky
The 2016 launch had no evaluator: valid planets with no reason to visit, "infinite boredom". Later rules made worlds coherent (no lava planet with sea creatures). Build the evaluator first, while it is cheap.

## Meaningful variety
Variety needs logic: different worlds sharing biology and geology rules. Agents must learn what makes a space fun, not just different.

## Exercises
Write your Generator, Evaluator, Refiner and Circuit Breaker mapping; describe your game's infinite boredom and the GDD rule that prevents it.

## Relations
- [[lessons-index]] — parent index.
- [[lesson-08-ger-loop]] — part 1: the loop.
- [[assignment-6-style-guide-agent]] — rules as the evaluator.
