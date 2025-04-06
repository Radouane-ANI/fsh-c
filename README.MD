# fsh (Shell Personnalisé)

fsh est un interpréteur de commandes Unix développé en C, offrant une prise en charge complète des fonctionnalités essentielles d'un shell, notamment :

- Exécution des commandes Unix standard.
- Redirections d'entrée/sortie : `<`, `>`, `>>`.
- Pipes : `|`.
- Gestion des signaux : `SIGINT`, `SIGCHLD`, etc.
- Structures de contrôle : boucles `for` et conditionnelles `if`, `else`.

## Fonctionnalités

- **Exécution de commandes** : fsh permet d'exécuter toutes les commandes Unix standard, en gérant les processus enfants via les appels système `fork` et `exec`.
- **Redirections d'entrée/sortie** : Les redirections `<`, `>`, `2>` et `>>` sont implémentées en utilisant les appels système `dup2` pour manipuler les descripteurs de fichiers.
- **Pipes** : La communication entre processus est réalisée grâce aux pipes, configurés avec les appels `pipe`, `fork`, et `dup2` pour rediriger les flux entre les commandes.
- **Gestion des signaux** : fsh gère les signaux tels que `SIGINT` et `SIGCHLD` en utilisant l'API POSIX `signal` pour assurer une gestion appropriée des interruptions et des processus zombies.
- **Structures de contrôle** : Le shell supporte les boucles `for` et les structures conditionnelles `if`/`else`, permettant l'exécution de scripts complexes directement dans l'interpréteur.

## Technologies utilisées

- **Langage** : C
- **API POSIX** :
  - `fork`
  - `exec`
  - `pipe`
  - `dup2`
  - `signal`
  - `wait`

## Installation

1. Clonez le dépôt :

   ```bash
   git clone https://github.com/Radouane-ANI/fsh-c.git
   ```

2. Accédez au répertoire du projet :

   ```bash
   cd fsh-c
   ```

3. Compilez le projet :

   ```bash
   make
   ```

## Utilisation

Après compilation, exécutez le shell avec la commande suivante :

```bash
./fsh
```

Vous pouvez ensuite utiliser fsh comme n'importe quel autre interpréteur de commandes Unix, en profitant des fonctionnalités mentionnées ci-dessus.

## Contribuer

Les contributions sont les bienvenues. Pour proposer des améliorations ou signaler des problèmes, veuillez ouvrir une issue sur le dépôt GitHub.

