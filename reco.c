#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reco.h"
#include "mf/mf.h"
#include "knn/knn.h"
#include "graph/pagerank.h"

static float **mf_model = NULL;
static int knn_loaded = 0;
static int graph_loaded = 0;

// === Fonction centrale ===
char *generate_recommendations(const char *algo_name, int user_id, int top_n)
{
    if (!algo_name)
    {
        fprintf(stderr, "[RECO] Erreur: algo_name est NULL\n");
        return NULL;
    }

    if (strcmp(algo_name, "knn") == 0)
    {
        if (!knn_loaded)
        {
            load_ratings("knn/ratings.txt") ;
            compute_pearson_matrix();
            knn_loaded = 1;
        }
        return recommend_knn(user_id, top_n);
    }
    else if (strcmp(algo_name, "mf") == 0)
    {
        if (mf_model == NULL)
        {
            mf_model = mf("mf/ratings.txt");
            if (!mf_model)
            {
                fprintf(stderr, "[MF] Erreur lors de l'entraînement du modèle MF.\n");
                return NULL;
            }
        }
        return recommend_mf(user_id, top_n, mf_model);
    } else if (strcmp(algo_name, "graph") == 0) {
    if (!graph_loaded) {
        build_graph("graph/ratings.txt");
        pagerank_compute_scores(user_id);
        graph_loaded = 1;
    }
    int dummy = 0;
    uint32_t* ids = pagerank_recommender(user_id, top_n, &dummy);
    if (!ids) return NULL;

    char* result = malloc(256);
    if (!result) return NULL;
    result[0] = '\0';

    char buffer[16];
    for (int i = 0; i < dummy; i++) {
        sprintf(buffer, "%u", ids[i]);
        strcat(result, buffer);
        if (i < dummy - 1) strcat(result, ",");
    }

    free(ids);
    return result;
} else {
    fprintf(stderr, "[RECO] Algorithme inconnu: '%s'\n", algo_name);
    return NULL;
}

}


