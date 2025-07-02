// test_reco.c
#include <stdio.h>
#include "reco.h"

int main() {
    int user_id = 0;
    int top_n = 5;

    char* r1 = generate_recommendations("knn", user_id, top_n);
    printf("KNN: %s\n", r1);

    char* r2 = generate_recommendations("mf", user_id, top_n);
    printf("MF: %s\n", r2);

    char* r3 = generate_recommendations("graph", user_id, top_n);
    printf("GRAPH: %s\n", r3);

    return 0;
}
