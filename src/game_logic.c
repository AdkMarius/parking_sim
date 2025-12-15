#include "../include/game_logic.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Clear vehicle from grid
static void clear_vehicle_from_grid(GameContext *ctx, VEHICULE *v) {
    for (int i = 0; i < CAR_HEIGHT; i++) {
        for (int j = 0; j < CAR_WIDTH; j++) {
            int y = v->posy + i;
            int x = v->posx + j;
            if (y >= 0 && y < MAX_HEIGHT && x >= 0 && x < MAX_WIDTH) {
                if (ctx->map.grid[y][x] == 2) {
                    ctx->map.grid[y][x] = 0;
                }
            }
        }
    }
}

// Mark vehicle on grid
static void mark_vehicle_on_grid(GameContext *ctx, VEHICULE *v) {
    for (int i = 0; i < CAR_HEIGHT; i++) {
        for (int j = 0; j < CAR_WIDTH; j++) {
            int y = v->posy + i;
            int x = v->posx + j;
            if (y >= 0 && y < MAX_HEIGHT && x >= 0 && x < MAX_WIDTH) {
                ctx->map.grid[y][x] = 2;
            }
        }
    }
}

// Calculate squared distance (avoid sqrt for performance)
static int distance_squared(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return dx * dx + dy * dy;
}

// Find a parking spot near the vehicle's current position
static int find_parking_spot(GameContext *ctx, int start_x, int start_y, int *out_x, int *out_y) {
    int search_radius = 20;

    for (int y = start_y - search_radius; y <= start_y + search_radius; y++) {
        for (int x = start_x - search_radius; x <= start_x + search_radius; x++) {
            if (x >= 0 && y >= 0 && x < ctx->map.width && y < ctx->map.height) {
                if (check_collision(ctx, x, y, CAR_WIDTH, CAR_HEIGHT) == 1) {
                    *out_x = x;
                    *out_y = y;
                    return 1; // Found a spot
                }
            }
        }
    }
    return 0; // No spot found
}

// Move vehicle forward in its current direction
static void move_vehicle_forward(GameContext *ctx, VEHICULE *v) {
    // Clear old position
    clear_vehicle_from_grid(ctx, v);

    // Calculate new position
    int new_x = v->posx;
    int new_y = v->posy;

    switch (v->direction) {
        case 'N': new_y -= v->vitesse; break;
        case 'S': new_y += v->vitesse; break;
        case 'E': new_x += v->vitesse; break;
        case 'O': new_x -= v->vitesse; break;
    }

    // Check collision at new position
    if (check_collision(ctx, new_x, new_y, CAR_WIDTH, CAR_HEIGHT) == 1) {
        v->posx = new_x;
        v->posy = new_y;
    }
    // If collision, vehicle stays at old position

    // Mark new (or old if blocked) position
    mark_vehicle_on_grid(ctx, v);
}

// Handle STATE_SEARCHING
static void handle_searching(GameContext *ctx, VEHICULE *v) {
    static int frame_counter = 0;

    // Move forward
    move_vehicle_forward(ctx, v);

    // Every 5 frames, scan for parking spot
    if (frame_counter % 5 == 0) {
        int spot_x, spot_y;
        if (find_parking_spot(ctx, v->posx, v->posy, &spot_x, &spot_y)) {
            v->target_x = spot_x;
            v->target_y = spot_y;
            v->logic_state = STATE_PARKING;
        }
    }
    frame_counter++;
}

// Handle STATE_PARKING
static void handle_parking(GameContext *ctx, VEHICULE *v) {
    int dx = v->target_x - v->posx;
    int dy = v->target_y - v->posy;

    // Choose direction based on largest delta
    if (abs(dx) > abs(dy)) {
        v->direction = (dx > 0) ? 'E' : 'O';
    } else {
        v->direction = (dy > 0) ? 'S' : 'N';
    }

    // Move toward target
    move_vehicle_forward(ctx, v);

    // Check if reached target (distance < 2)
    if (distance_squared(v->posx, v->posy, v->target_x, v->target_y) < 4) {
        v->logic_state = STATE_PARKED;
        v->tps = 0;
    }
}

// Handle STATE_PARKED
static void handle_parked(GameContext *ctx, VEHICULE *v) {
    (void)ctx; // Not used in this function

    // Increment parking time
    v->tps++;

    unsigned long int min_park_time = 100;  // 10 seconds at 10 FPS
    unsigned long int max_park_time = 500;  // 50 seconds at 10 FPS

    // Decide when to leave
    if (v->tps >= max_park_time) {
        // Force leave after max time
        v->logic_state = STATE_LEAVING;
        v->target_x = ctx->map.exit_x;
        v->target_y = ctx->map.exit_y;
    } else if (v->tps >= min_park_time) {
        // Random chance to leave (5% per frame)
        if ((rand() % 100) < 5) {
            v->logic_state = STATE_LEAVING;
            v->target_x = ctx->map.exit_x;
            v->target_y = ctx->map.exit_y;
        }
    }
}

// Handle STATE_LEAVING
static void handle_leaving(GameContext *ctx, VEHICULE *v) {
    int dx = v->target_x - v->posx;
    int dy = v->target_y - v->posy;

    // Choose direction
    if (abs(dx) > abs(dy)) {
        v->direction = (dx > 0) ? 'E' : 'O';
    } else {
        v->direction = (dy > 0) ? 'S' : 'N';
    }

    // Move toward exit
    move_vehicle_forward(ctx, v);

    // Check if reached exit
    if (distance_squared(v->posx, v->posy, v->target_x, v->target_y) < 4) {
        v->logic_state = STATE_PAYING;
    }
}

// Handle STATE_PAYING
static void handle_paying(GameContext *ctx, VEHICULE *v) {
    // Calculate revenue based on parking time
    int revenue = v->tps / 100; // 1 euro per 100 frames (10 seconds at 10 FPS)
    if (revenue < 1) {
        revenue = 1; // Minimum charge
    }

    // Add to score
    ctx->score += revenue;

    // Clear vehicle from grid
    clear_vehicle_from_grid(ctx, v);

    // Transition to exited
    v->logic_state = STATE_EXITED;
}

// Handle spawning logic
static void handle_spawning(GameContext *ctx) {
    static int frame_count = 0;
    frame_count++;

    int spawn_interval;
    if (ctx->game_mode == 0) {
        spawn_interval = 50; // FLUIDE: every 5 seconds
    } else {
        spawn_interval = 20; // CHARGÉ: every 2 seconds
    }

    if (frame_count >= spawn_interval) {
        frame_count = 0;

        // 80% cars, 20% trucks
        int type = (rand() % 10) < 8 ? 0 : 1;

        // Check if entry is clear before spawning
        if (check_collision(ctx, ctx->map.entry_x, ctx->map.entry_y, CAR_WIDTH, CAR_HEIGHT) == 1) {
            spawn_vehicle(ctx, type);
        }
    }
}

// ============================================================================
// PUBLIC FUNCTIONS (Declared in game_logic.h)
// ============================================================================

// Initialise le contexte de jeu
void init_game(GameContext *ctx, int mode) {
    ctx->game_mode = mode;
    ctx->running = 1;
    ctx->score = 0;
    ctx->car_list = NULL;

    // Initialize map (will be overridden by load_map_from_file)
    ctx->map.height = 0;
    ctx->map.width = 0;
    ctx->map.entry_x = -1;
    ctx->map.entry_y = -1;
    ctx->map.exit_x = -1;
    ctx->map.exit_y = -1;

    // Clear the grid
    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            ctx->map.grid[i][j] = 0;
        }
    }
}

// Crée un nouveau véhicule
VEHICULE* spawn_vehicle(GameContext *ctx, int type) {
    // Allocate memory
    VEHICULE *new_car = (VEHICULE*)malloc(sizeof(VEHICULE));
    if (new_car == NULL) {
        return NULL; // Memory allocation failed
    }

    // Initialize position (spawn at entry point)
    new_car->posx = ctx->map.entry_x;
    new_car->posy = ctx->map.entry_y;

    // Determine initial direction based on entry point
    if (ctx->map.entry_x <= 5) {
        new_car->direction = 'E'; // Left edge, move east
    } else if (ctx->map.entry_x >= ctx->map.width - CAR_WIDTH - 5) {
        new_car->direction = 'O'; // Right edge, move west (Ouest)
    } else if (ctx->map.entry_y <= 5) {
        new_car->direction = 'S'; // Top edge, move south
    } else {
        new_car->direction = 'N'; // Bottom edge, move north
    }

    // Set speed
    new_car->vitesse = 1;

    // Set vehicle type
    new_car->type = (type == 0) ? 'v' : 'c';

    // Set alignment (random)
    new_car->alignement = (rand() % 2) ? 'g' : 'd';

    // Initialize AI state
    new_car->logic_state = STATE_SEARCHING;
    new_car->target_x = -1;
    new_car->target_y = -1;

    // Initialize time and status
    new_car->tps = 0;
    new_car->etat = '1'; // Active

    // Initialize ID (for debugging)
    static int next_id = 1;
    new_car->id = next_id++;

    // Initialize color (random ANSI color 1-7)
    new_car->code_couleur = (rand() % 7) + 1;

    // Initialize carrosserie (empty for now - graphics module will handle this)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 30; j++) {
            new_car->Carrosserie[i][j] = ' ';
        }
    }

    // Add to linked list (insert at head)
    new_car->NXT = ctx->car_list;
    ctx->car_list = new_car;

    // Mark position on grid
    mark_vehicle_on_grid(ctx, new_car);

    return new_car;
}

// Met à jour la simulation
void update_simulation(GameContext *ctx) {
    // Phase 1: Update all existing vehicles
    VEHICULE *current = ctx->car_list;
    VEHICULE *prev = NULL;

    while (current != NULL) {
        VEHICULE *next = current->NXT; // Save next pointer

        // Process vehicle based on its state
        switch (current->logic_state) {
            case STATE_SEARCHING:
                handle_searching(ctx, current);
                break;
            case STATE_PARKING:
                handle_parking(ctx, current);
                break;
            case STATE_PARKED:
                handle_parked(ctx, current);
                break;
            case STATE_LEAVING:
                handle_leaving(ctx, current);
                break;
            case STATE_PAYING:
                handle_paying(ctx, current);
                break;
            case STATE_EXITED:
                // Remove from list and free
                if (prev == NULL) {
                    // Removing head
                    ctx->car_list = next;
                } else {
                    // Removing from middle or tail
                    prev->NXT = next;
                }
                free(current);
                current = next;
                continue; // Skip prev update
        }

        prev = current;
        current = next;
    }

    // Phase 2: Spawn new vehicles
    handle_spawning(ctx);
}

// Vérifie les collisions
int check_collision(GameContext *ctx, int x, int y, int width, int height) {
    // Bounds checking
    if (x < 0 || y < 0) {
        return 0; // Out of bounds = collision
    }

    if ((x + width) > ctx->map.width || (y + height) > ctx->map.height) {
        return 0; // Out of bounds = collision
    }

    // Check every cell in the rectangular area
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int cell_value = ctx->map.grid[y + i][x + j];

            if (cell_value == 1) {
                return 0; // Wall = collision
            }

            if (cell_value == 2) {
                return 0; // Another vehicle = collision
            }
        }
    }

    // Area is completely free
    return 1;
}

// Supprime un véhicule
void remove_vehicle(GameContext *ctx, VEHICULE *target) {
    // Safety check
    if (target == NULL) {
        return;
    }

    // Clear vehicle from grid
    clear_vehicle_from_grid(ctx, target);

    // Remove from linked list
    if (ctx->car_list == NULL) {
        return; // Empty list
    }

    // Case A: Target is the head
    if (ctx->car_list == target) {
        ctx->car_list = target->NXT;
        free(target);
        return;
    }

    // Case B: Target is in the middle or tail
    VEHICULE *current = ctx->car_list;
    while (current->NXT != NULL) {
        if (current->NXT == target) {
            // Found it - bypass in linked list
            current->NXT = target->NXT;
            free(target);
            return;
        }
        current = current->NXT;
    }

    // Target not found (shouldn't happen, but safe)
}

// Libère la mémoire
void free_all_vehicles(GameContext *ctx) {
    VEHICULE *current = ctx->car_list;

    while (current != NULL) {
        VEHICULE *next = current->NXT; // Save next before freeing
        free(current);
        current = next;
    }

    // Set head to NULL (list is now empty)
    ctx->car_list = NULL;

    // Optionally clear entire grid
    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            ctx->map.grid[i][j] = 0;
        }
    }
}
