#include "../include/game_logic.h"

// Initialise le contexte de jeu
void init_game(GameContext *ctx, int mode) {
    // STUB: Initialisation par défaut
    (void)ctx;
    (void)mode;
}

// Crée un nouveau véhicule
VEHICULE* spawn_vehicle(GameContext *ctx, int type) {
    // STUB: Retourne NULL pour l'instant
    (void)ctx;
    (void)type;
    return NULL;
}

// Met à jour la simulation
void update_simulation(GameContext *ctx) {
    // STUB: Pas de logique pour l'instant
    (void)ctx;
}

// Vérifie les collisions
int check_collision(GameContext *ctx, int x, int y, int width, int height) {
    // STUB: Retourne 0 (pas de collision)
    (void)ctx; (void)x; (void)y; (void)width; (void)height;
    return 0;
}

// Supprime un véhicule
void remove_vehicle(GameContext *ctx, VEHICULE *target) {
    // STUB
    (void)ctx;
    (void)target;
}

// Libère la mémoire
void free_all_vehicles(GameContext *ctx) {
    // STUB: Rien à libérer pour l'instant
    (void)ctx;
}