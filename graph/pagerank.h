#ifndef PAGERANK_H
#define PAGERANK_H

#define MAX_USERS 1000
#define MAX_ITEMS 1000
#define MAX_NODES (MAX_USERS + MAX_ITEMS)
#define DAMPING_FACTOR 0.85f
#define MAX_ITER 100
#define EPSILON 1e-6

#include <stdint.h>

void build_graph(const char* train_file);
void pagerank_compute_scores(uint32_t start_uid);
void pagerank_compute_scores_from_user(uint32_t start_uid);
uint32_t* pagerank_recommender(uint32_t user_id, int top_n, int* actual_returned);
void free_pagerank_graph();

#endif
