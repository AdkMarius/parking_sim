#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "models.h"

// Initialise le contexte de jeu (mémoire, valeurs par défaut)
void init_game(GameContext *ctx, int mode);

// Crée un nouveau véhicule et l'ajoute à la liste chaînée
// Retourne un pointeur vers le véhicule créé ou NULL si erreur
VEHICULE* spawn_vehicle(GameContext *ctx, int type);

// Met à jour toute la simulation (appelé à chaque tour de boucle)
// Gère: Mouvements, États IA, Timers
void update_simulation(GameContext *ctx);

// Vérifie si un mouvement est possible (Collision avec murs ou autres voitures)
// Retourne 1 si libre, 0 si obstacle
int check_collision(GameContext *ctx, int x, int y, int width, int height);

// Supprime un véhicule spécifique de la liste chaînée (quand il sort)
void remove_vehicle(GameContext *ctx, VEHICULE *target);

// Libère toute la mémoire (liste chaînée) à la fin du jeu
void free_all_vehicles(GameContext *ctx);

#endif