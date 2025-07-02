#ifndef EVAL_H
#define EVAL_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_USERS 1000
#define TOP_N_EVAL 10

typedef struct {
    uint32_t user_id;
    uint32_t* recommended_items;
    uint32_t num_recommended;
} UserRecommendations;

typedef struct {
    uint32_t user_id;
    uint32_t* relevant_items;
    uint32_t num_relevant;
} UserRelevantItems;

UserRecommendations* load_predictions(const char* predicted_file, int* num_predictions_out);
UserRelevantItems* load_relevant_items(const char* test_file, int* num_relevant_out);

void free_user_recommendations(UserRecommendations* recos, int count);
void free_user_relevant_items(UserRelevantItems* items, int count);

float compute_hit_ratio(UserRecommendations* preds, int num_preds, UserRelevantItems* truths, int num_truths);
float compute_map(UserRecommendations* preds, int num_preds, UserRelevantItems* truths, int num_truths);
float compute_ndcg(UserRecommendations* preds, int num_preds, UserRelevantItems* truths, int num_truths);

#endif
