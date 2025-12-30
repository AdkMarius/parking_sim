#include "../include/graphics.h"
#include <stdio.h>
#include <string.h>

// ANSI color codes
#define ANSI_RESET      "\033[0m"
#define ANSI_BLACK      "\033[30m"
#define ANSI_RED        "\033[31m"
#define ANSI_GREEN      "\033[32m"
#define ANSI_YELLOW     "\033[33m"
#define ANSI_BLUE       "\033[34m"
#define ANSI_MAGENTA    "\033[35m"
#define ANSI_CYAN       "\033[36m"
#define ANSI_WHITE      "\033[37m"

// Background colors
#define ANSI_BG_BLACK   "\033[40m"
#define ANSI_BG_GRAY    "\033[100m"

// Efface l'écran et remet le curseur en haut à gauche
void clear_screen() {
    // ANSI escape codes:
    // \033[2J = Clear entire screen
    // \033[H = Move cursor to home position (0,0)
    printf("\033[2J\033[H");
    fflush(stdout);
}

// Déplace le curseur à la position (x, y)
void move_cursor(int x, int y) {
    // ANSI escape code: \033[y;xH
    // Note: Terminal coordinates start at 1,1 (not 0,0)
    printf("\033[%d;%dH", y + 1, x + 1);
}

// Charge le plan du parking depuis un fichier texte
int load_map_from_file(const char *filename, ParkingMap *map) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", filename);
        return 0;
    }

    // Initialize map
    map->height = 0;
    map->width = 0;
    map->entry_x = -1;
    map->entry_y = -1;
    map->exit_x = -1;
    map->exit_y = -1;

    // Clear grid
    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            map->grid[i][j] = 0;
        }
    }

    // Read file line by line
    char line[MAX_WIDTH + 2]; // +2 for newline and null terminator
    int row = 0;

    while (fgets(line, sizeof(line), file) != NULL && row < MAX_HEIGHT) {
        int col = 0;
        int line_len = strlen(line);

        // Remove newline if present
        if (line_len > 0 && line[line_len - 1] == '\n') {
            line[line_len - 1] = '\0';
            line_len--;
        }

        // Update max width
        if (line_len > map->width) {
            map->width = line_len;
        }

        // Parse each character
        for (col = 0; col < line_len && col < MAX_WIDTH; col++) {
            char c = line[col];

            switch (c) {
                case '#':  // Wall
                    map->grid[row][col] = 1;
                    break;
                case 'E':  // Entry point
                    map->entry_x = col;
                    map->entry_y = row;
                    map->grid[row][col] = 0;  // Free space
                    break;
                case 'S':  // Exit point (Sortie)
                    map->exit_x = col;
                    map->exit_y = row;
                    map->grid[row][col] = 0;  // Free space
                    break;
                case ' ':  // Empty space
                case '.':  // Parking spot
                default:
                    map->grid[row][col] = 0;  // Free space
                    break;
            }
        }

        row++;
    }

    map->height = row;

    fclose(file);

    // Validate that entry and exit were found
    if (map->entry_x == -1 || map->entry_y == -1) {
        fprintf(stderr, "Avertissement: Point d'entrée 'E' non trouvé dans la carte\n");
    }
    if (map->exit_x == -1 || map->exit_y == -1) {
        fprintf(stderr, "Avertissement: Point de sortie 'S' non trouvé dans la carte\n");
    }

    return 1;
}

// Charge un modèle de voiture depuis un fichier
int load_car_model(const char *filename, char carrosserie[4][30]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        // If file doesn't exist, use a default simple model
        // Simple car model (4 lines x 10 chars)
        const char *default_model[4] = {
            " ________ ",
            "|  CAR  | ",
            "|========|",
            " o    o  "
        };

        for (int i = 0; i < 4; i++) {
            strncpy(carrosserie[i], default_model[i], 30);
            // Pad with spaces if needed
            for (int j = strlen(default_model[i]); j < 30; j++) {
                carrosserie[i][j] = ' ';
            }
            carrosserie[i][29] = '\0';
        }

        return 1;  // Success with default model
    }

    // Read 4 lines from file
    for (int i = 0; i < 4; i++) {
        if (fgets(carrosserie[i], 30, file) != NULL) {
            // Remove newline if present
            int len = strlen(carrosserie[i]);
            if (len > 0 && carrosserie[i][len - 1] == '\n') {
                carrosserie[i][len - 1] = ' ';
            }
            // Pad with spaces
            for (int j = len; j < 30; j++) {
                carrosserie[i][j] = ' ';
            }
            carrosserie[i][29] = '\0';
        } else {
            // Fill with spaces if line is missing
            for (int j = 0; j < 30; j++) {
                carrosserie[i][j] = ' ';
            }
            carrosserie[i][29] = '\0';
        }
    }

    fclose(file);
    return 1;
}

// Affiche une frame complète
void draw_frame(const GameContext *ctx) {
    // Clear screen first
    clear_screen();

    // Draw header
    printf("╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║         SIMULATION DE PARKING - Mode: %s                        ║\n",
           ctx->game_mode == 0 ? "FLUIDE " : "CHARGÉ");
    printf("║         Score: %-10d                  Appuyez sur 'q' pour quitter ║\n",
           ctx->score);
    printf("╚════════════════════════════════════════════════════════════════════════╝\n");

    // Create a display buffer (copy of grid)
    char display[MAX_HEIGHT][MAX_WIDTH];
    for (int i = 0; i < ctx->map.height; i++) {
        for (int j = 0; j < ctx->map.width; j++) {
            int cell = ctx->map.grid[i][j];
            switch (cell) {
                case 0: display[i][j] = ' '; break;  // Empty
                case 1: display[i][j] = '#'; break;  // Wall
                case 2: display[i][j] = 'V'; break;  // Vehicle (will be overridden)
                default: display[i][j] = '?'; break;
            }
        }
    }

    // Mark entry and exit on display
    if (ctx->map.entry_x >= 0 && ctx->map.entry_y >= 0) {
        display[ctx->map.entry_y][ctx->map.entry_x] = 'E';
    }
    if (ctx->map.exit_x >= 0 && ctx->map.exit_y >= 0) {
        display[ctx->map.exit_y][ctx->map.exit_x] = 'S';
    }

    // Draw vehicles on display
    VEHICULE *current = ctx->car_list;
    int vehicle_count = 0;
    while (current != NULL) {
        // Mark vehicle position with a colored character
        int x = current->posx;
        int y = current->posy;

        // Draw a simple representation (top-left corner)
        if (y >= 0 && y < ctx->map.height && x >= 0 && x < ctx->map.width) {
            // Use different symbols based on vehicle type
            char symbol = (current->type == 'v') ? 'C' : 'T';  // C=Car, T=Truck

            // Draw a small representation (2x4 for visibility)
            for (int dy = 0; dy < 2 && (y + dy) < ctx->map.height; dy++) {
                for (int dx = 0; dx < 4 && (x + dx) < ctx->map.width; dx++) {
                    if (dx == 0 && dy == 0) {
                        display[y + dy][x + dx] = symbol;
                    } else {
                        display[y + dy][x + dx] = (dy == 1) ? '-' : '=';
                    }
                }
            }
        }

        vehicle_count++;
        current = current->NXT;
    }

    // Draw the map
    for (int i = 0; i < ctx->map.height; i++) {
        printf("  ");  // Indent
        for (int j = 0; j < ctx->map.width; j++) {
            char c = display[i][j];

            // Color coding
            if (c == '#') {
                printf("%s%s", ANSI_BG_GRAY, ANSI_BLACK);
            } else if (c == 'E') {
                printf("%s", ANSI_GREEN);
            } else if (c == 'S') {
                printf("%s", ANSI_RED);
            } else if (c == 'C' || c == 'T') {
                printf("%s", ANSI_CYAN);
            }

            printf("%c", c);
            printf("%s", ANSI_RESET);
        }
        printf("\n");
    }

    // Draw footer with stats
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────────────┐\n");
    printf("│ Véhicules actifs: %-3d                                              │\n", vehicle_count);
    printf("│ Légende: E=Entrée S=Sortie #=Mur C=Voiture T=Camion               │\n");
    printf("└─────────────────────────────────────────────────────────────────────┘\n");

    fflush(stdout);
}
