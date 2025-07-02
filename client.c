#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // Pour fork(), close()
#include <sys/socket.h> // Pour socket(), bind(), listen(), accept()
#include <netinet/in.h> // Pour sockaddr_in, INADDR_ANY
#include <arpa/inet.h>  // Pour inet_ntoa()
#include <sys/wait.h>   // Pour waitpid() afin de gérer les processus zombies
#include <pthread.h>

//#include "reco.h" // Votre en-tête central pour les fonctions de recommandation

#define PORT 9000                  // Port sur lequel le serveur écoute
#define BUFFER_SIZE 1024           // Taille du buffer pour les messages
#define SERVER_IP "192.168.161.215"  // Adresse IP du serveur
#define RATINGS_FILE "ratings.txt" // Chemin vers votre fichier de données de notation

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr =
    {
        // Configuration de l'adresse du serveur
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr(SERVER_IP)
};

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    // Exemple de requête 
    char message[BUFFER_SIZE];
    fprintf(stdout, "Entrez votre requête (ex: 6 knn 3) : ");
    fgets(message, BUFFER_SIZE, stdin);
    
    send(sock, message, strlen(message), 0);

    char response[BUFFER_SIZE];
    ssize_t  bytes = recv(sock, response, BUFFER_SIZE -1, 0);
    if ( bytes > 0)
    {
        response[bytes] = '\0';
        fprintf(stdout, "Recommandations : %s\n", response);
    }

    close(sock);

return 0;
}