---
id: perf-lead-lumen-robots
description: "Lead: since the Meshy chaser, draw and GPU grow about 5.6 µs per robot; one trace points at Lumen card captures. Not proven, not tried."
parent: perf-leads
kind: note
---
# Lead: robot rendering and Lumen (task-181)

- measured ([[perf-stress-chaser-meshy]], task-181-01): draw and GPU each grow about 5.6 µs per robot, about +3.1 ms from 50 to 600 chasers in S0; in M1 both were flat. Frames are now limited by the render thread.
- trace S0 600: per frame about 100 Lumen card captures (1.1 ms), Lumen scene update 1.4 ms and visibility 2.1 ms on the render thread. Robot parts re-captured into the Lumen lighting cache is the likely cause; no on/off comparison yet.
- options (none tried): first bound the gain with the S0 600 step and Lumen off; then exclude the chaser body mesh from Lumen (its Lumen card / distance field lighting settings) or lower the Lumen scene detail for small dynamic meshes. See also lead 1 of [[perf-leads-m1]] (rendering floor).
- matters only above about 300 robots or on a weaker machine.

## Relations
- [[perf-leads]] — parent index.
- [[perf-stress-chaser-meshy]] — the measurements and trace.
