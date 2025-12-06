#include "../include/utils.h"

// Configure le terminal
void configure_terminal() {
    // STUB
}

// Restaure le terminal
void restore_terminal() {
    // STUB
}

// Récupère la touche appuyée
char key_pressed() {
    // STUB: Retourne 0 (aucune touche) par défaut
    return 0;
}

// Retourne le temps actuel
long get_time_ms() {
    // STUB
    return 0;
}

// Pause
void sleep_ms(int milliseconds) {
    // STUB: Utilise usleep basique pour éviter de bloquer le CPU dans le main
    usleep(milliseconds * 1000);
}