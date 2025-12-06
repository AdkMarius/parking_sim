#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // Pour srand()
#include <unistd.h> // Pour usleep()

// Import des headers de notre architecture
#include "include/models.h"
#include "include/game_logic.h"
#include "include/graphics.h"
#include "include/utils.h"

// Vitesse de la simulation (en microsecondes)
// 100000 us = 100ms = 10 FPS (Frames Per Second)
#define GAME_SPEED_DELAY 100000 

int main() {
    // 1. Initialisation du hasard (pour la génération aléatoire des voitures)
    srand(time(NULL));

    // 2. Création du contexte de jeu
    GameContext ctx;
    
    // 3. Configuration du terminal (Mode non-bloquant pour key_pressed) [cite: 78-81]
    configure_terminal();

    // 4. Initialisation des données
    // On commence en mode FLUIDE par défaut (0)
    init_game(&ctx, 0);

    // Chargement de la carte (Si échec, on quitte proprement)
    if (!load_map_from_file("assets/parking_plan.txt", &ctx.map)) {
        printf("Erreur: Impossible de charger le plan du parking.\n");
        restore_terminal(); // Important : remettre le terminal en état
        return 1;
    }

    // 5. Boucle Principale (Game Loop)
    ctx.running = 1;
    while (ctx.running) {
        // --- A. GESTION DES ENTRÉES (INPUT) ---
        // On récupère la touche sans bloquer l'exécution [cite: 84]
        char key = key_pressed(); 
        
        if (key == 'q' || key == 'Q') {
            ctx.running = 0; // On demande l'arrêt de la boucle
        }
        // Ici on pourra ajouter d'autres touches (ex: 'c' pour changer de mode)

        // --- B. MISE À JOUR LOGIQUE (UPDATE) ---
        // C'est ici que l'IA des voitures est calculée
        update_simulation(&ctx);

        // --- C. AFFICHAGE (RENDER) ---
        // On dessine tout ce qui s'est passé
        draw_frame(&ctx);

        // --- D. RÉGULATION DE VITESSE ---
        // On dort un peu pour ne pas utiliser 100% du CPU
        usleep(GAME_SPEED_DELAY);
    }

    // 6. Nettoyage et Sortie (CLEANUP)
    // C'est critique pour la note : aucune fuite mémoire !
    free_all_vehicles(&ctx);
    
    // Restauration du terminal (pour ne pas laisser le shell buggé) [cite: 82-83]
    restore_terminal();
    
    printf("\nFin de la simulation. Au revoir !\n");
    return 0;
}