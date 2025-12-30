#include "../include/utils.h"
#include <stdio.h>

// Static variable to store original terminal settings
static struct termios original_termios;
static int terminal_configured = 0;

// Configure le terminal en mode raw (saisie immédiate)
void configure_terminal() {
    struct termios raw;

    // Get current terminal settings
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        perror("tcgetattr");
        return;
    }

    // Mark that we've saved the original settings
    terminal_configured = 1;

    // Copy to modify
    raw = original_termios;

    // Disable canonical mode (line buffering) and echo
    raw.c_lflag &= ~(ICANON | ECHO);

    // Set minimum number of characters for read
    raw.c_cc[VMIN] = 0;   // Non-blocking read
    raw.c_cc[VTIME] = 0;  // No timeout

    // Apply new settings
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        return;
    }

    // Set stdin to non-blocking mode
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

// Restaure le terminal dans son état d'origine
void restore_terminal() {
    if (terminal_configured) {
        // Restore original terminal settings
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) {
            perror("tcsetattr");
        }

        // Restore blocking mode
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);

        terminal_configured = 0;
    }
}

// Récupère la touche appuyée sans bloquer
char key_pressed() {
    char c;
    int result = read(STDIN_FILENO, &c, 1);

    if (result == 1) {
        return c;  // Key was pressed
    }

    return 0;  // No key pressed
}

// Retourne le temps actuel en millisecondes
long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Convert to milliseconds
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// Pause en millisecondes
void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}
