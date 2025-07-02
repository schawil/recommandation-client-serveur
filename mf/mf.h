#ifndef MF_H
#define MF_H

#define MAX_USERS 1000
#define MAX_ITEMS 1000

    typedef struct {
        int item_id;
        float score;
    } Pair;


// Entrainer le modèle avec SGD et retourner la matrice complète R[u][i]
float** mf_train(const char* train_file, int num_factors, int num_epochs, float alpha, float lambda);

// Prédire toutes les notes du fichier test en utilisant la matrice R
float* mf_predict_all(float** R, const char* test_file, int* num_predictions_out);

// Recommandation pour un utilisateur : retourne une chaîne d'items triés
char* recommend_mf(int user_id, int top_n, float** R);

// 
float **mf(const char* mf_train_file);
// Outils de gestion mémoire
float** allocate_matrix(int rows, int cols);
void free_matrix(float** matrix, int rows);

#endif
