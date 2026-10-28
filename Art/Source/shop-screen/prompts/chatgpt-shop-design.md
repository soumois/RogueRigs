Tu es directeur artistique UI et game designer spécialisé en "game feel" pour jeux indés. Je te joins une capture d'écran de l'écran de boutique de mon jeu (image jointe : l'écran "SPARE PARTS", tel qu'il est aujourd'hui, 4 cartes grisées "Short on ore", le 5e emplacement vendu, solde 7, Reroll stock 20, Continue). Je veux que tu me fasses des propositions de design et de game feel pour cet écran, puis que nous en discutions ensemble. Ne génère aucune image : réponds en texte.

## 1. Le jeu
Rogue Rigs est un roguelite auto-shooter en vue du dessus (Unreal Engine 5.8). Le joueur est un mineur indépendant qui vole du minerai à une corporation sur un astéroïde, avant l'extraction. L'arène elle-même est sa meilleure arme. Une run enchaîne 4 niveaux. Après une extraction réussie aux niveaux 1 à 3, deux écrans s'enchaînent : l'écran de stats, puis cet écran de boutique de cartes. Pas de boutique après le niveau 4.

Piliers de design (tout élément doit passer au moins un test) :
- P1 Game feel / juice : on ajoute un retour que le joueur ressent, pas seulement un chiffre.
- P2 Visée manuelle + tir automatique.
- P3 L'arène est une arme.
Contrainte dure : aucune IA ne tourne pendant le jeu. L'IA n'est qu'un outil de développement.

## 2. Style visuel et couleurs
- Low-poly à facettes, ombrage plat ; côté joueur ("Rigs") : rafistolé, boulonné, blanc et sarcelle avec de la rouille ; côté corporation (Kessler-Voss) : robots propres, industriels, yeux rouges, petite plaque logo jaune.
- Sol de l'arène : grandes plaques de pierre fissurées, violet-magenta sombre. Vide spatial derrière : nuages indigo, violet électrique, magenta, bleu outremer. Cristaux violets lumineux (Duskite).
- Le minerai (la monnaie) est le cristal violet Duskite, icône violette à plusieurs pointes.
- Règles de lisibilité : le rouge veut dire tir ennemi ou soin ; l'orange veut dire "explose" ; le vert acide et le bleu acier sont réservés aux ennemis ; le jaune est la couleur de la corporation ; les couleurs de rareté n'existent que dans l'interface, jamais sur un objet de l'arène.
- Couleurs de rareté des cartes (valeurs actuelles de l'interface) : gris, vert, bleu, violet, jaune (du plus faible au plus fort). L'effet "yellow" est toujours le plus haut.
- Tout le texte à l'écran est en anglais pour l'instant. Mes noms de cartes et textes de flavour sont en anglais.
- Pas d'audio dans le jeu pour l'instant : aucun asset sonore n'existe encore.

## 3. Contraintes techniques
- Cadre 1920 x 1080, interface en UMG (widgets Unreal), sans code runtime généré par IA.
- UMG sait faire : transformations de rendu (translation, rotation, échelle, opacité) pilotées au tick ou par animation, matériaux d'interface (par exemple un matériau de "brûlure" à paramètre), images, bordures, textes, boutons, étincelles dessinées en images, défilements. Pas de shaders complexes ni de particules 3D dans l'UMG ; Niagara est possible mais plus coûteux : dis-moi quand tu en supposes.
- L'écran actuel : titre "SPARE PARTS" en haut, un panneau gris contenant 5 cartes en une rangée (cartes de 330 px de large, même look que les cartes de montée de niveau, avec cadre à tier, icône de stat, valeur, nom, cible, ligne de flavour), sous chaque carte le prix (icône de minerai + chiffre) et un bouton Buy, en bas à gauche le solde de minerai, en bas à droite le bouton Reroll avec son coût, puis le bouton Continue sous le panneau. Fond noir opaque. Boutons au style par défaut d'Unreal, c'est-à-dire aucun design pour l'instant.
- Écran de montée de niveau (pour comparaison, il a déjà son propre game feel) : 3 grandes cartes, avec distribution animée, suspense de rareté, retournement, survol (légère mise à l'échelle, reflet), et à la sélection la carte vole vers l'arme ou la barre d'XP pendant que les autres brûlent. La boutique n'a volontairement pas la distribution, le suspense ni le vol.

## 4. Règles FIXES (ne les change pas)
- Monnaie : le minerai collecté pendant la run (le compteur de minerai actuel ; à l'intermission, tout est sécurisé). Acheter est facultatif.
- 5 cartes d'amélioration à la fois, tirées comme une offre de montée de niveau avec la chance du mineur, cartes de stats uniquement (jamais de carte nouvelle arme).
- Prix par rareté : gris 25, vert 40, bleu 60, violet 80, jaune 100.
- On peut acheter plusieurs cartes. Une carte achetée s'applique tout de suite, reste pour les niveaux suivants de la run, et laisse son emplacement vide. Les achats ne valent que pour la run en cours.
- Une carte que le mineur ne peut pas payer est grisée (aujourd'hui : bouton désactivé "Short on ore", prix en rouge, carte assombrie).
- Reroll : retire les 5 cartes ; coût 15 pour le premier, puis +5 à chaque reroll supplémentaire (15, 20, 25...) ; le compteur repart à 15 à chaque intermission. Sur ma capture, "Reroll stock 20" signifie que le coût du prochain reroll est 20.
- Le solde de minerai est affiché sur l'écran. Il y a un bouton Continue. Le niveau suivant se charge en arrière-plan dès l'ouverture de l'écran ; au clic sur Continue : fondu sortant 0,3 s puis fondu entrant 0,5 s sur le niveau suivant (si le niveau n'est pas prêt : écran de chargement).
- Écran propre, fond noir opaque (le niveau terminé n'est plus visible), le récapitulatif est sur l'écran de stats précédent.
- Les valeurs chiffrées exactes des cartes (par exemple "+45% Fire rate") sont encore ouvertes : n'invente pas de valeurs d'équilibrage, travaille avec les exemples de la capture.
- Texte de carte vendue actuel : "Sold. No refunds." (je suis ouvert à une meilleure formule, voir plus bas).

## 5. Effets d'achat DÉJÀ PLANIFIÉS (valeurs provisoires)
Trois tâches sont prévues. Je veux que tes propositions s'y intègrent, les améliorent ou les contredisent avec une bonne raison :
1. Vol du minerai vers le bouton Buy : quand j'achète une carte, des icônes de minerai (une par tranche de 10 du prix, de 3 à 10) partent du solde et volent jusqu'au bouton Buy de cette carte, pendant que le solde décompte.
2. Punch du bouton Buy : quand la dernière icône arrive, le bouton fait un "punch", un flash et projette des étincelles à la couleur de la rareté (autre effet que le précédent).
3. Brûlure de la carte achetée : ensuite la carte brûle (même effet de brûlure que les cartes non choisies à la montée de niveau, teinté par la rareté), puis affiche "Sold". Pas de vol vers une arme ni vers la barre d'XP, contrairement à la montée de niveau.
Pendant la séquence (environ 1,2 s) tous les boutons de la boutique sont bloqués.

## 6. Ce qui est OUVERT à tes propositions
Mise en page et hiérarchie visuelle (où est l'information la plus importante : solde, prix, rareté, bouton ?), apparence des cartes de boutique (plus petites que celles de la montée de niveau), design des boutons Buy, Reroll et Continue (aujourd'hui le style par défaut), traitement du solde et du prix, état grisé "Short on ore" (plus lisible et moins punitif ?), emplacement vide / carte vendue, titre, fond (noir opaque imposé, mais éléments d'ambiance possibles), animation d'entrée de l'écran, survol et clic, animation du reroll, retour de l'achat (en plus des trois effets planifiés), sons et musique à prévoir (même s'il n'y a pas encore d'assets, je veux une liste d'événements sonores avec leur intention), ton et texte (pars des textes de la capture, par exemple "Sold. No refunds." et les lignes de flavour des cartes).

## 7. Forme de réponse attendue
- Commence par 3 à 5 questions de clarification seulement si elles changent vraiment tes propositions ; sinon propose directement.
- Donne plusieurs propositions numérotées (au moins 5, de la plus simple à la plus ambitieuse). Pour chacune : (a) le principe en 2 ou 3 phrases, (b) pourquoi cela sert le game feel et les piliers (surtout P1) et reste lisible avec mon code couleur, (c) une estimation du coût d'implémentation en UMG : Faible (quelques propriétés ou une animation), Moyen (une fonction de tick, un matériau, quelques widgets), Élevé (système d'effets, Niagara, nombreux assets), avec ce qu'il faut créer comme assets, (d) les risques (lisibilité, durée de la séquence, confusion avec l'écran de montée de niveau).
- Termine par une recommandation : les 3 propositions que tu retiendrais en premier et l'ordre, puis une courte liste de questions à trancher ensemble. Ensuite nous discutons.
- Ne propose rien qui exige une IA au moment du jeu, ne change aucune règle de la section 4, n'invente aucune valeur d'équilibrage.
