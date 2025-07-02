#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mf.h"

// Fonction pour charger les notes réelles du fichier test
float* load_true_ratings(const char* test_file, int* count) {
    FILE* f = fopen(test_file, "r");
    if (!f) { perror("open test"); return NULL; }

    int uid, iid, cid;
    float rating;
    long ts;

    int cap = 10000;
    float* true_vals = malloc(cap * sizeof(float));
    int n = 0;

    while (fscanf(f, "%d %d %d %f %ld", &uid, &iid, &cid, &rating, &ts) == 5) {
        true_vals[n++] = rating;
        if (n >= cap) {
            cap *= 2;
            true_vals = realloc(true_vals, cap * sizeof(float));
        }
    }

    fclose(f);
    *count = n;
    return true_vals;
}

float compute_rmse(const float* preds, const float* truths, int n) {
    float sse = 0.0;
    for (int i = 0; i < n; i++) {
        float err = preds[i] - truths[i];
        sse += err * err;
    }
    return sqrtf(sse / n);
}

float compute_mae(const float* preds, const float* truths, int n) {
    float sae = 0.0;
    for (int i = 0; i < n; i++) {
        sae += fabsf(preds[i] - truths[i]);
    }
    return sae / n;
}

int main() {
    const char* train_file = "ratings.txt";
    const char* test_file = "test.txt";
    const int num_factors = 10;
    const int epochs = 30;
    const float alpha = 0.01f;
    const float lambda = 0.05f;

    printf("== Entraînement Matrix Factorization ==\n");
    float** R = mf_train(train_file, num_factors, epochs, alpha, lambda);
    if (!R) {
        fprintf(stderr, "Erreur dans mf_train()\n");
        return EXIT_FAILURE;
    }

    printf("== Prédiction sur fichier test ==\n");
    int nb_preds = 0;
    float* preds = mf_predict_all(R, test_file, &nb_preds);
    if (!preds) {
        fprintf(stderr, "Erreur dans mf_predict_all()\n");
        free_matrix(R, MAX_USERS);
        return EXIT_FAILURE;
    }

    int nb_truths = 0;
    float* truths = load_true_ratings(test_file, &nb_truths);

    if (nb_preds != nb_truths) {
        fprintf(stderr, "Erreur : nombre de prédictions (%d) différent du nombre de notes réelles (%d)\n", nb_preds, nb_truths);
        free(preds);
        free(truths);
        free_matrix(R, MAX_USERS);
        return EXIT_FAILURE;
    }

    printf("== Évaluation ==\n");
    float rmse = compute_rmse(preds, truths, nb_preds);
    float mae = compute_mae(preds, truths, nb_preds);

    printf("RMSE: %.4f\n", rmse);
    printf("MAE : %.4f\n", mae);

    // (optionnel) Sauvegarde dans predicted.txt
    FILE* fout = fopen("predicted.txt", "w");
    if (fout) {
        for (int i = 0; i < nb_preds; i++)
            fprintf(fout, "%.3f\n", preds[i]);
        fclose(fout);
    }

    free(preds);
    free(truths);
    free_matrix(R, MAX_USERS);

    return EXIT_SUCCESS;
}
