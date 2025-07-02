#ifndef KNN_H
#define KNN_H

#define MAX_USERS 1000  // Nombre maximal d'utilisateurs (à ajuster selon votre dataset)
#define MAX_ITEMS 1000  // Nombre maximal d'items (à ajuster selon votre dataset)
#define K_NEIGHBORS 10  // Nombre de voisins à considérer pour la prédiction KNN

// Initialisation des profils utilisateurs et chargement des notes depuis un fichier.
// Cette fonction initialise les structures globales 'users'.
void load_ratings(const char* filename);

// Construit et stocke la matrice de similarité Pearson entre utilisateurs dans
// la variable globale 'similarity_matrix'.
void compute_pearson_matrix();

// Prédit la note d'un item pour un utilisateur donné en utilisant l'algorithme K-NN.
float predict_knn(int user_id, int item_id);

// Prédit les notes pour toutes les paires (utilisateur, item) présentes dans
// un fichier de test. Retourne un tableau de prédictions et le nombre de prédictions.
float* predict_all_knn(const char* test_file, int* count);

// Recommande les top-N items pour un utilisateur donné.
// Retourne une chaîne de caractères formatée avec les IDs des items recommandés.
char* recommend_knn(int user_id, int top_n);

// Déclaration de la matrice de similarité comme externe (globale) pour accès depuis d'autres fichiers
extern float similarity_matrix[MAX_USERS][MAX_USERS];

#endif // KNN_H