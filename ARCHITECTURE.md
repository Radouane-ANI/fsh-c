# Architecture du Code

Ce document décrit l'architecture du code et le fonctionnement des principales fonctionnalités.

---

## Structure Globale

### Boucle Principale

La boucle principale constitue le cœur du programme. Elle fonctionne en réalisant les étapes suivantes :

1. **Initialisation :**
   - La variable `valeur_retour` est initialisée à `0`.
   - Les signaux sont configurés via la fonction `setup_signals()`.

2. **Exécution continue :**
   Une boucle infinie (`while (1)`) effectue les actions suivantes :
   - **Affichage de l'invite de commande :**
     La fonction `setup_prompt()` génère l'invite de commande en fonction de la valeur de `valeur_retour`.

   - **Lecture de la commande utilisateur :**
     La commande est lue via `readline()` et stockée dans une variable `ligne`. 
     Les commandes valides sont ajoutées à l'historique via `add_history()`.

   - **Séparation des mots :**
     La commande lue est décomposée en un tableau de mots grâce à la fonction `separer_chaine()`.

   - **Exécution de la commande :**
     La commande est traitée comme suit :
     - Si elle correspond à une commande interne, elle est exécutée par la fonction `execute_cmd_interne()`.
     - Sinon, elle est interprétée comme une commande externe et exécutée via `execute_cmd_externe()`.

---

## Fonctionnalités Clés

### Séparation des Commandes : `separer_chaine`

La fonction `separer_chaine` prend une ligne de commande en entrée et la sépare en un tableau de mots en utilisant l'espace comme séparateur. Cette fonction repose sur `strtok_r`.

#### Fonctionnement :
1. Alloue dynamiquement la mémoire nécessaire pour copier la ligne d'entrée.
2. Parcourt la chaîne pour extraire les mots, les stockant dans un tableau dynamique.
3. Gère la réallocation de mémoire si le tableau atteint sa capacité maximale.

#### Exemple de sortie :
Entrée : `"ls -l /home/user"`  
Sortie : `["ls", "-l", "/home/user", NULL]`

---

### Exécution des Commandes Internes : `execute_cmd_interne`

La fonction `execute_cmd_interne` traite les commandes internes spécifiques au programme. Elle suit les étapes suivantes :

1. **Détection des commandes spéciales :**
   - Recherche de symboles comme `|` pour traiter les pipelines.
   - Vérification via les fonctions `checkfor()` et `checkif()` pour d'autres types de commandes.

2. **Exécution des commandes :**
   - Reconnaît et exécute les commandes internes telles que :
     - `pwd` : Affiche le répertoire courant.
     - `cd` : Change le répertoire courant.
     - `ftype` : Vérifie le type de fichier.
     - `exit` : Termine l'exécution du programme avec un code de sortie.

3. **Traitement des erreurs :**
   - Si une commande reçoit plusieurs arguments ou un argument invalide, elle retourne une erreur.

4. **Exécution des commandes externes :**
   Si la commande n'est pas interne, elle est passée à `execute_cmd_externe()`.

#### Gestion des Erreurs :
- Les erreurs de mémoire (malloc, realloc) entraînent la terminaison immédiate du programme.
- Des messages d'erreur sont affichés en cas de mauvais usage des commandes internes.

---

# Architecture des Boucles `for` et Conditions `if`

## Boucle `for` (Fonction `checkfor`)

La boucle `for` implémentée dans le code suit une structure spécifique, où une commande peut être exécutée pour chaque élément d'un répertoire. La fonction `checkfor` est utilisée pour vérifier la validité de la syntaxe d'une boucle `for`, et si elle est correctement formée, elle déclenche l'exécution de la boucle.

### Vérification de la Syntaxe de la Boucle `for`

1. **Validité de la boucle** : 
   - La fonction commence par vérifier si le tableau de chaînes `boucle` est valide et bien formé.
   - La première condition vérifie si la commande est bien `for` et si les arguments suivants sont corrects. Si la commande ne correspond pas à `for`, la fonction retourne `-1` pour signaler une erreur.
   
2. **Conditions spécifiques à la boucle** :
   - Si les conditions initiales sont respectées (par exemple, le premier argument est `for`, il y a une clé `in`, et les accolades `{` sont correctement utilisées), on vérifie les options supplémentaires passées à la boucle.

3. **Exécution des commandes dans la boucle** :
   - Si la syntaxe de la boucle est correcte, la commande contenue dans la boucle (`cmd`) est reconstruite et exécutée pour chaque élément du répertoire spécifié (`rep`).
   - Le parcours des fichiers et répertoires s'effectue avec la fonction `parcours_rep`.

---

## Parcours des Répertoires et Exécution des Commandes (Fonction `parcours_rep`)

La fonction `parcours_rep` permet d'effectuer un parcours d'un répertoire, et d'exécuter des commandes sur les éléments du répertoire selon les conditions spécifiées dans la boucle `for`.

### Fonctionnement de `parcours_rep` :

1. **Parcours des éléments du répertoire** :
   - Le répertoire est ouvert avec `opendir()`, et les fichiers sont parcourus avec `readdir()`.
   - Chaque fichier est traité en fonction des options spécifiées dans la boucle (`-A`, `-r`, `-e`, etc.).

2. **Exécution des commandes** :
   - Une fois un fichier validé par les conditions de filtrage, la commande `cmd` est exécutée sur ce fichier. 
   - La commande peut contenir un caractère spécial (`$c`), qui sera remplacé par le chemin du fichier actuel grace a la fonction
   remplace.

3. **Parcours récursif** :
   - Si l'option `-r` est activée et qu'un sous-répertoire est rencontré, la fonction s'appelle récursivement pour parcourir le sous-répertoire.

4. **Gestion du parallélisme** :
   - L'option `-p` permet de limiter le nombre de processus simultanés. La variable `n` contrôle quel processus est autorisé à exécuter une commande à un moment donné. Un processus ne peut exécuter une commande que si son numéro `process_` est égal à `n`. Et `n` est incrementer à chaque tour de boucle modulo le nombre de process.

---

## Condition `if` (Fonction `checkif`)

La fonction `checkif` est utilisée pour vérifier les conditions d'une structure `if`. Si les conditions sont remplies, la commande associée au bloc `if` est exécutée.

### Vérification de la condition `if` :

1. **Vérification de la syntaxe** :
   - La fonction commence par vérifier que la commande est bien structurée et que les conditions sont valides.
   - Elle vérifie la présence du mot-clé `if` et s'assure que l'accolade `{` est présente pour débuter le bloc de commandes.

2. **Évaluation de la condition** :
   - Si les conditions de l'`if` sont remplies, le bloc de commandes à exécuter est extrait et exécuté.

3. **Exécution de la commande** :
   - Une fois que la condition `if` est évaluée à vrai, la commande associée est exécutée.

---

# Architecture des commandes structurées et pipelines

## Commandes structurées

Les commandes structurées permettent de regrouper plusieurs commandes les unes après les autres en les séparant par des `;` ou bien `|`. Elles peuvent être utilisées dans des blocs d'instructions ou sans.

### Fonctionnement des commandes structurées : `execute_cmd`

1. **Détection des `;`**
    - On parcours le mot jusqu'à trouver un `;`. Si avant d'en trouver un on croise une accolade ouvrante
    `{` alors on incrémente un compteur dès qu'on trouve un `{` et on le décrémente dès qu'on croise celle fermante.
2. **Exécution des commandes**
    - Si on croise un `;` en dehors d'un bloc d'instruction, on exécute l'instruction précédent le `;` en lui appliquant la même formule que précédement.
    - On renvoie la valeur de retour de la dernière commande

### Fonctionnement des pipelines : `execute_cmd_interne`

1. **Détection des `|`**
    - On utilise le même principe que pour les commandes structurées sauf qu'on ajoute une variable `nb_cmds` qui compte le nombre de `|` en dehors de blocs d'instructions.

2. **Séparation des commandes**
    - Allocation d'un tableau de commandes
    - Découpage à chaque `|` hors structures
    - Création de copies des commandese

3. **Création des tubes**
    - Un tube pour chaque paire de commandes
    - `nb_cmd` - 1 tubes to total

4. **Exécution**
    - Fork pour chaque commande
    - Configuration des redirections
    - Exécution de `execute_cmd_interne`

---

### Ordre d'éxécution

Les commandes structurées (`;`) sont séparées avant les pipelines (`|`)
Voici la hiérarchie pour le traitement des commandes:

1. **Niveau supérieur : Commandes structurées (`execute_cmd`)**
    - Fonction appelée dès qu'on reçoit une commande
    - Fais appelle à `execute_cmd_interne` pour chaque commandes en dehors des blocs d'instructions

2. **Niveau secondaire : Pipeline (`execute_cmd_interne`)**
    - Exécute chaque commande reçu selon le procédé cité plus haut