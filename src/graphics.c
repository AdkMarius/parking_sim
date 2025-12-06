#include "../include/graphics.h"

// Efface l'écran
void clear_screen() {
    // STUB
}

// Charge le plan (Retourne 1 pour simuler un succès et ne pas bloquer le main)
int load_map_from_file(const char *filename, ParkingMap *map) {
    // STUB
    (void)filename;
    (void)map;
    return 1; // Simulation de succès
}

// Charge un modèle de voiture
int load_car_model(const char *filename, char carrosserie[4][30]) {
    // STUB
    (void)filename;
    (void)carrosserie;
    return 1; // Simulation de succès
}

// Affiche une frame
void draw_frame(const GameContext *ctx) {
    // STUB: Affiche juste un texte pour prouver que ça tourne
    // Note: On utilise printf ici juste pour le debug du stub
    printf("--- SIMULATION EN COURS (STUB GRAPHIQUE) ---\n");
    (void)ctx;
}

// Déplace le curseur
void move_cursor(int x, int y) {
    // STUB
    (void)x;
    (void)y;
}