#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "knn.h"

float compute_rmse(float *pred, float *truth, int n)
{
    float sse = 0.0f;
    for (int i = 0; i < n; i++)
    {
        float e = pred[i] - truth[i];
        sse += e * e;
    }
    return sqrtf(sse / n);
}

float compute_mae(float *pred, float *truth, int n)
{
    float mae = 0.0f;
    for (int i = 0; i < n; i++)
    {
        mae += fabsf(pred[i] - truth[i]);
    }
    return mae / n;
}

float *load_ground_truth(const char *file, int *n_out)
{
    FILE *f = fopen(file, "r");
    if (!f)
        return NULL;
    int uid, iid, cid;
    float rating;
    long ts;
    float *truth = malloc(10000 * sizeof(float));
    int n = 0;
    while (fscanf(f, "%d %d %d %f %ld", &uid, &iid, &cid, &rating, &ts) == 5)
    {
        truth[n++] = rating;
    }
    fclose(f);
    *n_out = n;
    return truth;
}

int main()
{
    printf("== Test KNN Recommender ==\n");

    load_ratings("ratings.txt");
    compute_pearson_matrix();

    int n_preds = 0;
    float *preds = predict_all_knn("test.txt", &n_preds);
    int n_truths = 0;
    float *truths = load_ground_truth("test.txt", &n_truths);

    if (n_preds != n_truths)
    {
        printf("Nombre incohérent entre prédictions et vérité.\n");
        return 1;
    }
    FILE *fout = fopen("predicted.txt", "w");
    for (int i = 0; i < n_preds; i++)
    {
        fprintf(fout, "%.3f\n", preds[i]);
    }
    fclose(fout);
    float rmse = compute_rmse(preds, truths, n_preds);
    float mae = compute_mae(preds, truths, n_preds);

    printf("RMSE = %.4f\n", rmse);
    printf("MAE  = %.4f\n", mae);

    free(preds);
    free(truths);

    return 0;
}
