#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Pour sqrtf, fabs
#include "knn.h"

// Structure pour stocker le profil d'un utilisateur
typedef struct {
    float ratings[MAX_ITEMS]; // Notes de l'utilisateur pour chaque item (0.0 si non noté)
    int count;                // Nombre d'items notés par cet utilisateur
    float mean;               // Moyenne des notes de cet utilisateur
} UserProfile;

// Variables globales pour stocker les profils utilisateurs et la matrice de similarité
UserProfile users[MAX_USERS];
float similarity_matrix[MAX_USERS][MAX_USERS];

// Initialise les profils utilisateurs et charge les notes depuis un fichier.
void load_ratings(const char* filename) {
    // Initialisation explicite de tous les profils utilisateurs
    for (int u = 0; u < MAX_USERS; u++) {
        memset(users[u].ratings, 0, sizeof(float) * MAX_ITEMS); // Met toutes les notes à 0.0 (non noté)
        users[u].count = 0;
        users[u].mean = 0.0f;
    }

    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Erreur: Impossible d'ouvrir le fichier de données (ratings.txt) pour le chargement KNN");
        exit(EXIT_FAILURE); // Quitte le programme en cas d'échec critique
    }

    int uid, iid, cid;
    float rating;
    long ts;

    while (fscanf(f, "%d %d %d %f %ld", &uid, &iid, &cid, &rating, &ts) == 5) {
        // Vérifier que les IDs sont dans les limites définies
        if (uid >= 0 && uid < MAX_USERS && iid >= 0 && iid < MAX_ITEMS) {
            // Stocke la note. Si une note existe déjà pour cette paire (uid, iid), elle est écrasée.
            // Le comptage final des notes distinctes se fera après la boucle de lecture.
            users[uid].ratings[iid] = rating;
            // users[uid].count n'est pas incrémenté ici pour éviter les problèmes de doublons
            // et sera calculé correctement dans la boucle suivante pour la moyenne.
        } else {
            fprintf(stderr, "Avertissement: Transaction avec ID utilisateur (%d) ou item (%d) hors limites ignorée.\n", uid, iid);
        }
    }
    fclose(f);

    // Calcul de la moyenne de chaque utilisateur après que toutes ses notes sont chargées
    for (int u = 0; u < MAX_USERS; u++) {
        float sum = 0;
        int n = 0; // Compteur local pour les notes réelles (non nulles)
        for (int i = 0; i < MAX_ITEMS; i++) {
            if (users[u].ratings[i] > 0.0f) { // Supposons que 0.0f signifie "non noté"
                sum += users[u].ratings[i];
                n++;
            }
        }
        users[u].count = n; // Met à jour le nombre réel d'items notés
        users[u].mean = (n > 0) ? sum / n : 0.0f;
    }
}


// Calcule la similarité de Pearson entre deux utilisateurs (u1 et u2)
float pearson_similarity(int u1, int u2) {
    float sum_xy = 0, sum_x2 = 0, sum_y2 = 0;
    // Parcourir tous les items pour trouver ceux notés en commun
    for (int i = 0; i < MAX_ITEMS; i++) {
        // Vérifier que les deux utilisateurs ont noté l'item 'i' (note > 0.0f)
        if (users[u1].ratings[i] > 0.0f && users[u2].ratings[i] > 0.0f) {
            float x = users[u1].ratings[i] - users[u1].mean;
            float y = users[u2].ratings[i] - users[u2].mean;
            sum_xy += x * y;
            sum_x2 += x * x;
            sum_y2 += y * y;
        }
    }
    // Gérer les cas où la variance est nulle pour éviter la division par zéro
    if (sum_x2 == 0 || sum_y2 == 0) return 0.0f;
    return sum_xy / sqrtf(sum_x2 * sum_y2);
}

// Construit et stocke la matrice de similarité Pearson pour toutes les paires d'utilisateurs.
void compute_pearson_matrix() {
    for (int u1 = 0; u1 < MAX_USERS; u1++) {
        for (int u2 = 0; u2 < MAX_USERS; u2++) {
            if (u1 != u2) {
                // Calculer la similarité uniquement si les deux utilisateurs ont des notes
                if (users[u1].count > 0 && users[u2].count > 0) {
                    similarity_matrix[u1][u2] = pearson_similarity(u1, u2);
                } else {
                    similarity_matrix[u1][u2] = 0.0f; // Pas de similarité si l'un n'a pas de notes
                }
            } else {
                similarity_matrix[u1][u2] = 1.0f; // Similarité avec soi-même est 1
            }
        }
    }
}

// Structure temporaire pour stocker l'information d'un voisin pour le tri
typedef struct {
    int user_id;
    float sim_score;
    float rating_diff_from_mean; // (V_rating - V_mean) pour cet item
} NeighborInfo;

// Fonction de comparaison pour le tri des voisins par similarité absolue décroissante
int compare_neighbor_info(const void* a, const void* b) {
    const NeighborInfo* na = (const NeighborInfo*)a;
    const NeighborInfo* nb = (const NeighborInfo*)b;
    // Tri par valeur absolue de la similarité, en ordre décroissant
    if (fabs(na->sim_score) < fabs(nb->sim_score)) return 1;
    if (fabs(na->sim_score) > fabs(nb->sim_score)) return -1;
    return 0; // Si les similarités sont égales
}


float predict_knn(int user_id, int item_id) {
    if (user_id >= MAX_USERS || item_id >= MAX_ITEMS) {
        return 0.0f; // sécurité
    }

    typedef struct {
        int uid;
        float sim;
    } Neighbor;

    Neighbor neighbors[MAX_USERS];
    int count = 0;

    for (int v = 0; v < MAX_USERS; v++) {
        if (v == user_id) continue;
        if (users[v].ratings[item_id] > 0.0f) {
            float sim = similarity_matrix[user_id][v];
            neighbors[count].uid = v;
            neighbors[count].sim = sim;
            count++;
        }
    }

    // Tri des voisins par similarité décroissante (bubble sort simple, tu peux remplacer par qsort)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (neighbors[j].sim > neighbors[i].sim) {
                Neighbor tmp = neighbors[i];
                neighbors[i] = neighbors[j];
                neighbors[j] = tmp;
            }
        }
    }

    // Prédiction avec les K plus proches
    float num = 0.0f, den = 0.0f;
    int used = 0;
    for (int i = 0; i < count && used < K_NEIGHBORS; i++) {
        int v = neighbors[i].uid;
        float sim = neighbors[i].sim;

        if (fabs(sim) > 1e-6) { // ignore les similitudes nulles
            num += sim * (users[v].ratings[item_id] - users[v].mean);
            den += fabs(sim);
            used++;
        }
    }

    if (den == 0.0f)
        return users[user_id].mean; // fallback : moyenne de l'utilisateur

    return users[user_id].mean + num / den;
}

// Prédit les notes pour toutes les paires (utilisateur, item) dans un fichier de test.
float* predict_all_knn(const char* test_file, int* count_out) {
    FILE* f = fopen(test_file, "r");
    if (!f) {
        perror("Erreur: Impossible d'ouvrir le fichier de test pour la prédiction KNN");
        *count_out = 0;
        return NULL;
    }

    // Allocation dynamique du tableau de prédictions avec réallocation si nécessaire
    int capacity = 1000; // Capacité initiale
    float* predictions = malloc(capacity * sizeof(float));
    int current_count = 0;

    if (!predictions) {
        perror("Erreur d'allocation mémoire pour le tableau de prédictions");
        fclose(f);
        *count_out = 0;
        return NULL;
    }

    int uid, iid, cid;
    float rating_actual; // Note réelle (pour la lecture du format, non utilisée pour la prédiction)
    long ts;

    while (fscanf(f, "%d %d %d %f %ld", &uid, &iid, &cid, &rating_actual, &ts) == 5) {
        // Vérifier les limites des IDs
        if (uid >= 0 && uid < MAX_USERS && iid >= 0 && iid < MAX_ITEMS) {
            // S'assurer que le tableau a assez de place
            if (current_count >= capacity) {
                capacity *= 2; // Double la capacité
                float* new_predictions = realloc(predictions, capacity * sizeof(float));
                if (!new_predictions) {
                    perror("Erreur de réallocation mémoire pour les prédictions");
                    free(predictions); // Libérer l'ancien tableau
                    fclose(f);
                    *count_out = 0;
                    return NULL;
                }
                predictions = new_predictions;
            }
            predictions[current_count++] = predict_knn(uid, iid);
        } else {
             fprintf(stderr, "Avertissement: Transaction avec ID utilisateur (%d) ou item (%d) hors limites ignorée pour la prédiction.\n", uid, iid);
        }
    }

    fclose(f);
    *count_out = current_count;

    // Réduire la mémoire allouée à la taille exacte des prédictions (bonne pratique)
    if (current_count > 0) {
        float* final_predictions = realloc(predictions, current_count * sizeof(float));
        if (final_predictions) {
            predictions = final_predictions;
        } // else, predictions reste la version précédente si realloc échoue
    } else { // Si aucune prédiction, libérer complètement
        free(predictions);
        predictions = NULL;
    }

    return predictions;
}

// Structure temporaire pour le tri des items recommandés
typedef struct {
    int item_id;
    float score;
} ItemScore;

// Fonction de comparaison pour trier les ItemScore par score décroissant
int compare_item_scores(const void* a, const void* b) {
    const ItemScore* itemA = (const ItemScore*)a;
    const ItemScore* itemB = (const ItemScore*)b;
    if (itemA->score < itemB->score) return 1; // ItemA est moins bon que ItemB
    if (itemA->score > itemB->score) return -1; // ItemA est meilleur que ItemB
    return 0; // Scores égaux
}

// Recommande les top-N items pour un utilisateur
char* recommend_knn(int user_id, int top_n) {
    // Vérifier les limites de l'utilisateur
    if (user_id < 0 || user_id >= MAX_USERS) {
        fprintf(stderr, "[KNN] Erreur: user_id %d hors limites [0,%d).\n", user_id, MAX_USERS);
        return NULL;
    }

    ItemScore* scored_items = malloc(sizeof(ItemScore) * MAX_ITEMS);
    if (!scored_items) {
        perror("Erreur d'allocation mémoire pour les scores d'items à recommander");
        return NULL;
    }

    int recommendable_count = 0;
    // Parcourir tous les items pour trouver ceux non encore notés par l'utilisateur
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (users[user_id].ratings[i] <= 0.0f) { // Si l'item n'a pas été noté par l'utilisateur (ou note <= 0)
            scored_items[recommendable_count].item_id = i;
            scored_items[recommendable_count].score = predict_knn(user_id, i);
            recommendable_count++;
        }
    }

    // Tri des items recommandables par score décroissant
    qsort(scored_items, recommendable_count, sizeof(ItemScore), compare_item_scores);

    // Construction de la chaîne de sortie des top-N items
    // Estimer une taille suffisante pour la chaîne : (max chiffres d'un ID + 1 pour la virgule) * top_n + 1 pour '\0'
    // MAX_ITEMS = 10000 -> IDs jusqu'à 9999 (5 chiffres max)
    // 5 chiffres + 1 virgule = 6 caractères par item.
    int actual_top_n = (top_n < recommendable_count) ? top_n : recommendable_count;
    size_t output_buffer_size = actual_top_n * (5 + 1) + 1; // 5 chiffres + ',' + '\0'
    char* output_str = malloc(output_buffer_size);
    if (!output_str) {
        perror("Erreur d'allocation mémoire pour la chaîne de sortie des recommandations");
        free(scored_items);
        return NULL;
    }
    output_str[0] = '\0'; // S'assurer que la chaîne est vide au départ

    char temp_buffer[16]; // Buffer temporaire pour sprintf

    for (int i = 0; i < actual_top_n; i++) {
        sprintf(temp_buffer, "%d", scored_items[i].item_id);
        strcat(output_str, temp_buffer);
        if (i < actual_top_n - 1) { // Ajouter une virgule après chaque ID, sauf le dernier
            strcat(output_str, ",");
        }
    }

    free(scored_items);
    return output_str;
}