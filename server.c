#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // Pour fork(), close()
#include <sys/socket.h> // Pour socket(), bind(), listen(), accept()
#include <netinet/in.h> // Pour sockaddr_in, INADDR_ANY
#include <arpa/inet.h>  // Pour inet_ntoa()
#include <sys/wait.h>   // Pour waitpid() afin de gérer les processus zombies
#include <pthread.h>
#include "reco.h" 

#define SERVER_IP "192.168.161.215"
#define PORT 9000
#define BUFFER_SIZE 1024

void *handle_client(void *args)
{
    int client_sock = *(int *)args;
    free(args);
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    ssize_t bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0)
    {
        perror("recv");
        close(client_sock);
        return NULL;
    }

    buffer[bytes_received] = '\0';

    int user_id, top_n;
    char algo[32];

    if (sscanf(buffer, "%d %31s %d", &user_id, algo, &top_n) != 3)
    {
        char *error_msg = "Bad request format. Use <int:user_id> <char*:algo> <int:top_n>\n";
        send(client_sock, error_msg, strlen(error_msg), 0);
        close(client_sock);
        return NULL;
    }

    printf("Recu: user_id=%d, algo=%s, top_n=%d\n", user_id, algo, top_n);
    char *result = generate_recommendations(algo, user_id, top_n);
    if (!result)
        result = strdup("ERROR\n");

    send(client_sock, result, strlen(result), 0);
    //const char* new_line = "\n";
    send(client_sock, "\n", 1, 0);
    free(result);
    close(client_sock);
    return NULL;
}

int main()
{
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY};

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("bind");
        return 1;
    }

    if (listen(server_sock, 10) < 0)
    {
        perror("listen");
        return 1;
    }
    fprintf(stdout, "Serveur en écoute sur le port %d...\n", PORT);

    while(1){
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int* client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addrlen);

        if (*client_sock < 0)
        {
            perror("accept");
            free(client_sock);
            continue;
        }

        fprintf(stdout, "Connexion de %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
        
    }

    close(server_sock);
    return 0;
}