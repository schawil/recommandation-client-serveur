#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "mf.h"

#define MAX_USERS 1000
#define MAX_ITEMS 1000

int n_users = 0;
int n_items = 0;

float** allocate_matrix(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Erreur: dimensions invalides (%d, %d).\n", rows, cols);
        return NULL;
    }
    float** matrix = malloc(rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(float));
    }
    return matrix;
}

void free_matrix(float** matrix, int rows) {
    if (!matrix) return;
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

float** mf_train(const char* train_file, int num_factors, int num_epochs, float alpha, float lambda) {
    // Étape 1 : déterminer n_users et n_items dynamiquement
    FILE* scan = fopen(train_file, "r");
    if (!scan) {
        perror("Erreur ouverture du fichier pour le scan initial");
        return NULL;
    }

    int uid, iid, cid;
    float rating;
    long ts;
    int max_uid = 0, max_iid = 0;

    while (fscanf(scan, "%d %d %d %f %ld", &uid, &iid, &cid, &rating, &ts) == 5) {
        if (uid > max_uid) max_uid = uid;
        if (iid > max_iid) max_iid = iid;
    }
    fclose(scan);

    n_users = max_uid + 1;
    n_items = max_iid + 1;

    printf("[MF] n_users = %d, n_items = %d\n", n_users, n_items);

    float** P = allocate_matrix(n_users, num_factors);
    float** Q = allocate_matrix(n_items, num_factors);

    srand(time(NULL));
    for (int u = 0; u < n_users; u++)
        for (int f = 0; f < num_factors; f++)
            P[u][f] = 0.1f * ((float)rand() / RAND_MAX);

    for (int i = 0; i < n_items; i++)
        for (int f = 0; f < num_factors; f++)
            Q[i][f] = 0.1f * ((float)rand() / RAND_MAX);

    // Apprentissage par descente de gradient
    for (int epoch = 0; epoch < num_epochs; epoch++) {
        FILE* f = fopen(train_file, "r");
        if (!f) { perror("fopen train"); break; }

        while (fscanf(f, "%d %d %d %f %ld", &uid, &iid, &cid, &rating, &ts) == 5) {
            if (uid >= n_users || iid >= n_items) continue;

            float pred = 0.0f;
            for (int k = 0; k < num_factors; k++)
                pred += P[uid][k] * Q[iid][k];

            float err = rating - pred;

            for (int k = 0; k < num_factors; k++) {
                float pu = P[uid][k];
                float qi = Q[iid][k];
                P[uid][k] += alpha * (err * qi - lambda * pu);
                Q[iid][k] += alpha * (err * pu - lambda * qi);
            }
        }

        fclose(f);
        // printf("Epoch %d terminée.\n", epoch);
    }

    // Reconstruire la matrice finale
    float** R = allocate_matrix(n_users, n_items);
    for (int u = 0; u < n_users; u++)
        for (int i = 0; i < n_items; i++)
            for (int f = 0; f < num_factors; f++)
                R[u][i] += P[u][f] * Q[i][f];

    free_matrix(P, n_users);
    free_matrix(Q, n_items);
    return R;
}

// Fonction pour charger les données de notation
float** mf(const char* ratings_file) {
    // Choix d'hyperparamètres par défaut
    int num_factors = 10;
    int num_epochs = 20;
    float alpha = 0.01f;
    float lambda = 0.1f;

    return mf_train(ratings_file, num_factors, num_epochs, alpha, lambda);
}




float* mf_predict_all(float** R, const char* test_file, int* num_predictions_out) {
    FILE* f = fopen(test_file, "r");
    if (!f) { perror("open test"); return NULL; }

    int uid, iid, cid;
    float rating;
    long ts;

    int capacity = 10000;
    float* predictions = malloc(capacity * sizeof(float));
    int count = 0;

    while (fscanf(f, "%d %d %d %f %ld", &uid, &iid, &cid, &rating, &ts) == 5) {
        if (uid >= n_users || iid >= n_items) continue;

        float pred = R[uid][iid];
        predictions[count++] = pred;

        if (count >= capacity) {
            capacity *= 2;
            predictions = realloc(predictions, capacity * sizeof(float));
        }
    }

    fclose(f);
    *num_predictions_out = count;
    return predictions;
}

char* recommend_mf(int user_id, int top_n, float** mf_matrix) {
    if (!mf_matrix) {
        fprintf(stderr, "[MF] Erreur: mf_matrix NULL\n");
        return NULL;
    }

    if (user_id < 0 || user_id >= n_users) {
        fprintf(stderr, "[MF] Erreur: user_id %d hors limites [0,%d]\n", user_id, n_users);
        return NULL;
    }

    float* scores = malloc(sizeof(float) * n_items);
    int* item_ids = malloc(sizeof(int) * n_items);
    if (!scores || !item_ids) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        return NULL;
    }

    for (int i = 0; i < n_items; i++) {
        scores[i] = mf_matrix[user_id][i];
        item_ids[i] = i;
    }

    // Tri par score décroissant
    for (int i = 0; i < n_items - 1; i++) {
        for (int j = i + 1; j < n_items; j++) {
            if (scores[j] > scores[i]) {
                float tmp = scores[i]; scores[i] = scores[j]; scores[j] = tmp;
                int ti = item_ids[i]; item_ids[i] = item_ids[j]; item_ids[j] = ti;
            }
        }
    }

    char* output = malloc(256); output[0] = '\0';
    char buffer[16];
    for (int i = 0; i < top_n && i < n_items; i++) {
        sprintf(buffer, "%d", item_ids[i]);
        strcat(output, buffer);
        if (i < top_n - 1) strcat(output, ",");
    }

    free(scores);
    free(item_ids);
    return output;
}




