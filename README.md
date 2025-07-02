## PROJET : SYSTÈME DE RECOMMANDATION EN C



├── client.c              # Code source du client
├── Dockerfile            # Fichier pour la conteneurisation (si implémentée)
├── graph                 # Module PageRank
│   ├── eval.c            # Fonctions d'évaluation spécifiques au graphe (si nécessaires)
│   ├── eval.h
│   ├── pagerank.c        # Implémentation de l'algorithme PageRank
│   ├── pagerank.h
│   ├── ratings.txt       # Données de ratings pour le module graphe
│   └── test.txt          # Données de test pour le module graphe
├── knn                   # Module K-Nearest Neighbors
│   ├── knn.c             # Implémentation de l'algorithme KNN
│   ├── knn.h
│   ├── ratings.txt       # Données de ratings pour le module KNN
│   └── test.txt          # Données de test pour le module KNN
├── mf                    # Module Factorisation Matricielle
│   ├── mf.c              # Implémentation de l'algorithme MF
│   ├── mf.h
│   ├── ratings.txt       # Données de ratings pour le module MF
│   └── test.txt          # Données de test pour le module MF
├── Makefile              # Makefile principal pour compiler tout le projet
├── reco.c                # Logique de recommandation principale, agrège les algorithmes
├── reco.h                # Déclarations pour reco.c
├── server.c              # Code source du serveur
├── test_reco.c           # Exécutable de test pour la logique de recommandation et l'évaluation
├── preprocessing.c       # Fonctions de prétraitement des données
├── preprocessing.h
├── evaluation.c          # Fonctions d'évaluation des recommandations
├── evaluation.h
├── ratings.txt           # Fichier de données de ratings brut (ex: MovieLens)
└── test.txt              # Fichier de données de test (extrait de ratings.txt)




## Compilation du Projet

Pour compiler l'ensemble du projet (la bibliothèque partagée `libreco.so`, l'exécutable de test ), utilisez le `Makefile` principal :

```bash
make clean
make libreco.so
```

### Compiler et exécuter le server (Remplacer @IP par celle de la machine qui héberge le projet) avec la commande 
```bash
  gcc server.c -o server -L. -lreco -lpthread -lm
  LD_LIBRARY_PATH=. ./server
```


