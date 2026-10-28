---
id: m-card-burn
description: "M_CardBurn and MI_CardBurn, the UI dissolve material with fire edge for discarded cards; open to wire or tune the burn."
parent: blueprints-ui-index
kind: doc
---
# M_CardBurn / MI_CardBurn

Built by task-246-01. Values are provisional. Compiled and saved; read back through MCP; never seen on screen.

- asset_path: /Game/RogueRigs/UI/Cards/M_CardBurn (material), /Game/RogueRigs/UI/Cards/MI_CardBurn (instance with the defaults)
- parent_class: none (material; no Blueprint graph)
- purpose: dissolve an image from bottom to top with noise and a glowing orange edge, for the cards not picked.
- material: domain User Interface, blend Translucent; outputs Emissive Color and Opacity only.
- parameters (defaults read back on MI_CardBurn): `Burn` scalar 0 (0 = intact, 1 = fully dissolved); `EdgeWidth` scalar 0.08 (fire band width, in threshold units); `FireIntensity` scalar 4 (emissive multiplier); `FireColor` vector (1, 0.45, 0.1, 1); `Tex` texture, default T_CardFrame_Stat_Baked.
- graph: height = 0.8 * (1 - V) + 0.1 * Noise(UV * 6) + 0.1 (Noise node, Gradient-ALU style computed noise, 3 levels, no texture; V = 0 at the top, so the bottom has the lowest value); threshold = Burn * 1.2 - 0.1; d = height - threshold. Opacity = Tex.a * saturate(d * 200) (cut where d < 0, 1 px-soft edge). Fire = 1 - saturate(d / EdgeWidth). Emissive = lerp(Tex.rgb, FireColor * FireIntensity, Fire). At Burn 0, d >= about 0.1 so there is no fire and the image is intact; at Burn 1, d < 0 everywhere so it is fully gone.
- use (for 246-02): the material is meant for one overlay Image that shows a snapshot or a plain card-shaped silhouette, not for the layered card widgets (a card is many stacked widgets). On that Image, set the brush resource to MI_CardBurn, get the dynamic material (`Widget|Brush|GetDynamicMaterial`), set `Tex` with SetTextureParameterValue to the image to burn, then animate `Burn` from 0 to 1 with SetScalarParameterValue (for example from the gated widget tick, eased). Keep the overlay the same size and position as the card so the V coordinate maps card top to bottom.
- changes in task-246-02: the Noise node (simplex texture noise) was replaced by a pure maths pattern that certainly works in the UI domain: n = 0.5 + 0.5 sin(2 pi (7 U + 0.2 sin(2 pi 11 V))), range 0..1, height = 0.8 (1 - V) + 0.1 n + 0.1 (so at Burn 0 the lowest d is 0.1 + 0.1 and at Burn 1 the highest height is 1.0 < threshold 1.1: opacity 0 everywhere, checked by hand). New vector parameter Tint (default white) multiplies Tex rgb before the fire lerp; WBP_UpgradeChoice sets it to the tier colour of the card. The parameter lookup by info needs Index -1 (see [[wbp-upgrade-choice]]). Recompiled and saved; not seen on screen.
- users: [[wbp-upgrade-choice]] (discarded cards) and, since task-344-01, [[wbp-shop-card]] (BurnOverlay on the bought shop card, Burn 0 to 1 over 0.5 s, Tint = tier colour, Tex = T_CardFrame_Stat_Baked). Seen in the task-344-02 capture: the fire is the material's fixed yellow because Tint multiplies Tex before the fire lerp, so the tier shows only in the tinted frame.
- logic_in_bp: none
- assets: T_CardFrame_Stat_Baked (default Tex)
- unverified: the look on screen (edge brightness, noise scale 6, band width), and the UI-domain Noise node rendering at runtime; tune `EdgeWidth`, `FireIntensity` or the noise scale in the material if the owner wants another look.
- owner_agent: agent-bp

## Relations
- [[blueprints-ui-index]] — parent index.
- [[wbp-upgrade-choice]] — the card widget whose discarded cards will use this material.
- [[wbp-shop-card]] — second user, burns the bought shop card.
