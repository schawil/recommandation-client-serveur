#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "eval.h"

#define MAX_LINE 2048

// === Chargement des prédictions depuis predicted.txt ===
UserRecommendations* load_predictions(const char* filename, int* num_out) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("load_predictions fopen");
        *num_out = 0;
        return NULL;
    }

    UserRecommendations* arr = malloc(MAX_USERS * sizeof(UserRecommendations));
    char line[MAX_LINE];
    int count = 0;

    while (fgets(line, sizeof(line), f)) {
        uint32_t uid;
        int offset = 0;
        if (sscanf(line, "%u %n", &uid, &offset) != 1) continue;

        arr[count].user_id = uid;
        arr[count].recommended_items = malloc(TOP_N_EVAL * sizeof(uint32_t));
        arr[count].num_recommended = 0;

        char* ptr = line + offset;
        char* tok = strtok(ptr, " \n\r");
        while (tok && arr[count].num_recommended < TOP_N_EVAL) {
            arr[count].recommended_items[arr[count].num_recommended++] = atoi(tok);
            tok = strtok(NULL, " \n\r");
        }
        count++;
    }

    fclose(f);
    *num_out = count;
    return arr;
}

// === Chargement des items pertinents depuis test.txt ===
UserRelevantItems* load_relevant_items(const char* filename, int* num_out) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("load_relevant_items fopen");
        *num_out = 0;
        return NULL;
    }

    UserRelevantItems* arr = malloc(MAX_USERS * sizeof(UserRelevantItems));
    uint32_t* seen = calloc(MAX_USERS, sizeof(uint32_t));
    int counts[MAX_USERS] = {0};
    int caps[MAX_USERS] = {0};
    uint32_t** buffer = calloc(MAX_USERS, sizeof(uint32_t*));

    int uid, iid, cid;
    float rating;
    long ts;

    while (fscanf(f, "%d %d %d %f %ld", &uid, &iid, &cid, &rating, &ts) == 5) {
        if (uid >= MAX_USERS) continue;

        if (!buffer[uid]) {
            buffer[uid] = malloc(5 * sizeof(uint32_t));
            caps[uid] = 5;
        } else if (counts[uid] >= caps[uid]) {
            caps[uid] *= 2;
            buffer[uid] = realloc(buffer[uid], caps[uid] * sizeof(uint32_t));
        }

        buffer[uid][counts[uid]++] = iid;
        seen[uid] = 1;
    }

    int index = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (seen[i]) {
            arr[index].user_id = i;
            arr[index].num_relevant = counts[i];
            arr[index].relevant_items = buffer[i];
            index++;
        } else if (buffer[i]) {
            free(buffer[i]);
        }
    }

    free(buffer);
    free(seen);
    *num_out = index;
    return arr;
}

void free_user_recommendations(UserRecommendations* arr, int n) {
    if (!arr) return;
    for (int i = 0; i < n; i++) {
        free(arr[i].recommended_items);
    }
    free(arr);
}

void free_user_relevant_items(UserRelevantItems* arr, int n) {
    if (!arr) return;
    for (int i = 0; i < n; i++) {
        free(arr[i].relevant_items);
    }
    free(arr);
}

// === Helpers ===
bool contains(uint32_t item_id, uint32_t* list, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        if (list[i] == item_id) return true;
    }
    return false;
}

UserRelevantItems* get_truth(uint32_t uid, UserRelevantItems* arr, int n) {
    for (int i = 0; i < n; i++) {
        if (arr[i].user_id == uid)
            return &arr[i];
    }
    return NULL;
}

// === METRICS ===

float compute_hit_ratio(UserRecommendations* preds, int n_preds, UserRelevantItems* truths, int n_truths) {
    int hits = 0, total = 0;

    for (int i = 0; i < n_preds; i++) {
        UserRelevantItems* truth = get_truth(preds[i].user_id, truths, n_truths);
        if (!truth) continue;

        total++;

        for (uint32_t j = 0; j < preds[i].num_recommended; j++) {
            if (contains(preds[i].recommended_items[j], truth->relevant_items, truth->num_relevant)) {
                hits++;
                break;
            }
        }
    }

    return (total > 0) ? ((float)hits / total) : 0.0f;
}

float compute_map(UserRecommendations* preds, int n_preds, UserRelevantItems* truths, int n_truths) {
    float sum_ap = 0.0f;
    int count = 0;

    for (int i = 0; i < n_preds; i++) {
        UserRelevantItems* truth = get_truth(preds[i].user_id, truths, n_truths);
        if (!truth || truth->num_relevant == 0) continue;

        int relevant_found = 0;
        float ap = 0.0;

        for (uint32_t j = 0; j < preds[i].num_recommended; j++) {
            if (contains(preds[i].recommended_items[j], truth->relevant_items, truth->num_relevant)) {
                relevant_found++;
                ap += (float)relevant_found / (j + 1);
            }
        }

        if (relevant_found > 0)
            sum_ap += ap / relevant_found;
        count++;
    }

    return (count > 0) ? sum_ap / count : 0.0f;
}

float compute_ndcg(UserRecommendations* preds, int n_preds, UserRelevantItems* truths, int n_truths) {
    float sum_ndcg = 0.0;
    int count = 0;

    for (int i = 0; i < n_preds; i++) {
        UserRelevantItems* truth = get_truth(preds[i].user_id, truths, n_truths);
        if (!truth || truth->num_relevant == 0) continue;

        float dcg = 0.0, idcg = 0.0;
        for (uint32_t k = 0; k < truth->num_relevant && k < TOP_N_EVAL; k++) {
            idcg += 1.0 / log2f(k + 2);
        }

        for (uint32_t j = 0; j < preds[i].num_recommended && j < TOP_N_EVAL; j++) {
            if (contains(preds[i].recommended_items[j], truth->relevant_items, truth->num_relevant)) {
                dcg += 1.0 / log2f(j + 2);
            }
        }

        if (idcg > 0.0f) {
            sum_ndcg += dcg / idcg;
            count++;
        }
    }

    return (count > 0) ? sum_ndcg / count : 0.0f;
}
