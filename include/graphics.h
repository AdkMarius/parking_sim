#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "models.h"

// Efface l'écran et remet le curseur à zéro
void clear_screen();

// Charge le plan du parking depuis un fichier .txt dans la structure Map
// Retourne 1 si succès, 0 si erreur
int load_map_from_file(const char *filename, ParkingMap *map);

// Charge un modèle de voiture depuis un fichier .txt
// (À utiliser lors de la création d'un véhicule)
int load_car_model(const char *filename, char carrosserie[4][30]);

// Affiche une frame complète (Décor + Véhicules + UI)
void draw_frame(const GameContext *ctx);

// Fonction interne pour positionner le curseur (style gotoxy)
void move_cursor(int x, int y);

#endif