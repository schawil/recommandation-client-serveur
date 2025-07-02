## PROJET : SYSTÈME DE RECOMMANDATION EN C


Ce projet est un ***système de recommandation complet développé en C***, conçu avec une architecture client-serveur modulaire. Il met en œuvre trois algorithmes de recommandation distincts : KNN, la Factorisation Matricielle (MF), et un modèle basé sur le PageRank (graphe). Le système inclut également des outils robustes pour l'évaluation des performances via des métriques comme le Hit Ratio, la MAP et le NDCG, offrant ainsi une solution de bout en bout pour la génération et l'analyse de recommandations.


### Server 

Compiler le code global pour avoir le bliothèque `libreco.so`, exécuter le server (Remplacer @IP par celle de la machine qui héberge le projet) avec la commande:

1. Compilation du projet
```bash
  make clean // nettoyer les fichiers *.o .so et de test
  make // compiler le projet
```

2. Compilation et Exécution su server 

```bash
  gcc server.c -o server -lpthread -ldl -L. -lreco -lm
  LD_LIBRARY_PATH=. ./server
```


### Client 

Après avoir recupérer le fichier du client dans le projet (client.c) le mettre dans un dossier quelconque et le compiler et l'exécuter
(Modifier : *SERVER_IP "192.168.161.215"* et remplacer par l'adresse de la machine ou du serveur qui contient le projet global)

1. Compilation

```bash
  gcc client.c -c
  gcc client.c -o client
```
2. Exécution

```bash
  ./client 
```


## Compilation du Projet

Pour compiler l'ensemble du projet (la bibliothèque partagée `libreco.so`, l'exécutable de test ), utilisez le `Makefile` principal :

```bash
make clean
make 
```
### Dockerisation

1. Configuration du Dockerfile

### Compiler et exécuter le server (Remplacer @IP par celle de la machine qui héberge le projet) avec la commande 
```bash
  gcc server.c -o server -L. -lreco -lpthread -lm
  LD_LIBRARY_PATH=. ./server
```

2. Construire l’image 

```bash 
  docker build -t reco-server .
```
3. Lancer le serveur
```bash
  docker run -it --rm -p 9000:9000 reco-server
```

### HINTS

Chaque dossier contient un Makefile propre afin de pourvoir tester la recommandation et l'évaluation pour chaque module (KNN, MF, GRAPH)

```bash
  make clean
  make 
  gcc test_knn.c -L. -lknn -o test_knn -lm | gcc test_knn.c -L. -lknn -o test_knn -lm | gcc test_mf.c -L. -lknn -o test_mf -lm | gcc test_pagerank.c -L. -lpagerank -o test_pagerank 
  LD_LIBRARY_PATH=. ./test_knn | LD_LIBRARY_PATH=. ./test_mf | LD_LIBRARY_PATH=. ./test_pagerank | LD_LIBRARY_PATH=. ./test_pagerank_all
```


### Auteurs

* **[MEFIRE]** - Développeur principal
* **[ANOU]** - Contribution à [module KNN]
* **[POLLA]** - Contribution à [module GRAPH]
