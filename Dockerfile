# Utilise GCC pour compiler C
#FROM gcc:latest
FROM docker.io/library/gcc:latest

# Définir le répertoire de travail dans le conteneur
WORKDIR /app

# Copier le code source dans le conteneur
COPY . /app

# Compiler la bibliothèque partagée
RUN gcc -fPIC -shared -I./knn -I./mf -I./graph \
    reco.c knn/knn.c mf/mf.c graph/pagerank.c \
    -o libreco.so -lm && \
    gcc server.c -o server -L. -lreco -lpthread -lm

# Le serveur écoute sur le port 9000
EXPOSE 9000

# Exécuter le serveur avec la bibliothèque dynamique visible
CMD ["bash", "-c", "LD_LIBRARY_PATH=. ./server"]
