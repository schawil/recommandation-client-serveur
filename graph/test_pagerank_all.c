#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "pagerank.h"
#include "eval.h"  // Fonctions de calcul HR/MAP/NDCG

#define TRAIN_FILE "ratings.txt"
#define TEST_FILE "test.txt"
#define PREDICTED_FILE "pagerank_predicted_recos.txt"
#define TOP_N_RECOS 10

int main() {
    printf("== Démarrage du test PageRank ==\n");

    printf("== Construction du graphe PageRank ==\n");
    build_graph(TRAIN_FILE);

    printf("== Génération des recommandations pour le fichier de test ==\n");

    FILE* fout_predicted = fopen(PREDICTED_FILE, "w");
    if (!fout_predicted) {
        perror("Erreur ouverture predicted.txt");
        return EXIT_FAILURE;
    }

    FILE* f_test = fopen(TEST_FILE, "r");
    if (!f_test) {
        perror("Erreur ouverture test.txt");
        fclose(fout_predicted);
        return EXIT_FAILURE;
    }

    // Lire tous les user_id uniques du test
    uint32_t user_seen[MAX_USERS] = {0};
    uint32_t uid, iid, cid;
    float rating;
    long ts;
    uint32_t unique_uids[MAX_USERS];
    int n_users = 0;

    while (fscanf(f_test, "%u %u %u %f %ld", &uid, &iid, &cid, &rating, &ts) == 5) {
        if (uid < MAX_USERS && !user_seen[uid]) {
            user_seen[uid] = 1;
            unique_uids[n_users++] = uid;
        }
    }
    fclose(f_test);

    for (int i = 0; i < n_users; i++) {
        uint32_t user_id = unique_uids[i];
        int nb_recos = 0;

        uint32_t* recos = pagerank_recommender(user_id, TOP_N_RECOS, &nb_recos);

        fprintf(fout_predicted, "%u", user_id);
        for (int j = 0; j < nb_recos; j++) {
            fprintf(fout_predicted, " %u", recos[j]);
        }
        fprintf(fout_predicted, "\n");

        free(recos);
    }

    fclose(fout_predicted);

    printf("== Évaluation des performances PageRank ==\n");

    int nb_preds = 0;
    UserRecommendations* predictions = load_predictions(PREDICTED_FILE, &nb_preds);
    int nb_true = 0;
    UserRelevantItems* truths = load_relevant_items(TEST_FILE, &nb_true);

    float hr = compute_hit_ratio(predictions, nb_preds, truths, nb_true);
    float map = compute_map(predictions, nb_preds, truths, nb_true);
    float ndcg = compute_ndcg(predictions, nb_preds, truths, nb_true);

    printf("Hit Ratio@%d: %.4f\n", TOP_N_RECOS, hr);
    printf("MAP@%d:       %.4f\n", TOP_N_RECOS, map);
    printf("NDCG@%d:      %.4f\n", TOP_N_RECOS, ndcg);

    free_user_recommendations(predictions, nb_preds);
    free_user_relevant_items(truths, nb_true);
    free_pagerank_graph();

    return 0;
}
