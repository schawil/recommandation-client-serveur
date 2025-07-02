#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pagerank.h"

int main() {
    printf("== Construction du graphe ==\n");
    build_graph("ratings.txt");

    int uid;
    printf("Entrez l'ID utilisateur à tester (0 à %d) : ", MAX_USERS - 1);
    scanf("%d", &uid);

    int top_n = 10;
    int count = 0;
    uint32_t* recos = pagerank_recommender(uid, top_n, &count);

    printf("Top-%d recommandations pour utilisateur %d :\n", top_n, uid);
    for (int i = 0; i < count; i++) {
        printf("%d ", recos[i]);
    }
    printf("\n");

    free(recos);
    return 0;
}
