#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "pagerank.h"

bool adj[MAX_NODES][MAX_NODES];
int degree[MAX_NODES];
float pagerank_scores[MAX_NODES];
float personalization_vector[MAX_NODES];

void build_graph(const char *train_file)
{
    memset(adj, 0, sizeof(adj));
    memset(degree, 0, sizeof(degree));

    FILE *f = fopen(train_file, "r");
    if (!f)
    {
        perror("train");
        exit(1);
    }

    int uid, iid, cid;
    float rating;
    long ts;

    while (fscanf(f, "%d %d %d %f %ld", &uid, &iid, &cid, &rating, &ts) == 5)
    {
        if (uid < MAX_USERS && iid < MAX_ITEMS)
        {
            int u_node = uid;
            int i_node = MAX_USERS + iid;

            if (!adj[u_node][i_node])
            {
                adj[u_node][i_node] = true;
                adj[i_node][u_node] = true;
                degree[u_node]++;
                degree[i_node]++;
            }
        }
    }

    fclose(f);
}

void pagerank_compute_scores_from_user(uint32_t start_uid)
{
    int N = MAX_USERS + MAX_ITEMS;
    float pr_old[MAX_NODES];
    for (int i = 0; i < N; i++)
    {
        pagerank_scores[i] = 1.0f / N;
        personalization_vector[i] = 0.0f;
    }

    personalization_vector[start_uid] = 1.0f;

    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        memcpy(pr_old, pagerank_scores, sizeof(float) * N);
        float delta = 0.0f;

        for (int i = 0; i < N; i++)
        {
            float sum = 0.0f;
            for (int j = 0; j < N; j++)
            {
                if (adj[j][i] && degree[j] > 0)
                {
                    sum += pr_old[j] / degree[j];
                }
            }

            pagerank_scores[i] = DAMPING_FACTOR * sum + (1.0f - DAMPING_FACTOR) * personalization_vector[i];
            delta += fabsf(pagerank_scores[i] - pr_old[i]);
        }

        if (delta < EPSILON)
            break;
    }
}

int compare_pair_desc(const void *a, const void *b)
{
    float diff = ((float *)b)[1] - ((float *)a)[1];
    return (diff > 0) - (diff < 0);
}

uint32_t *pagerank_recommender(uint32_t user_id, int top_n, int *actual_returned)
{
    pagerank_compute_scores_from_user(user_id);

    typedef struct
    {
        uint32_t item_id;
        float score;
    } ItemScore;

    ItemScore *list = malloc(MAX_ITEMS * sizeof(ItemScore));
    int count = 0;

    for (uint32_t iid = 0; iid < MAX_ITEMS; iid++)
    {
        int item_node = MAX_USERS + iid;

        if (!adj[user_id][item_node])
        {
            list[count].item_id = iid;
            list[count].score = pagerank_scores[item_node];
            count++;
        }
    }

    qsort(list, count, sizeof(ItemScore), (int (*)(const void *, const void *))compare_pair_desc);

    uint32_t *recos = malloc(top_n * sizeof(uint32_t));
    int i;
    for (i = 0; i < top_n && i < count; i++)
    {
        recos[i] = list[i].item_id;
    }

    *actual_returned = i;
    free(list);
    return recos;
}

void free_pagerank_graph()
{
    memset(adj, 0, sizeof(adj));
    memset(degree, 0, sizeof(degree));
    memset(pagerank_scores, 0, sizeof(pagerank_scores));
    memset(personalization_vector, 0, sizeof(personalization_vector));
}

void pagerank_compute_scores(uint32_t start_uid)
{
    pagerank_compute_scores_from_user(start_uid);
}
