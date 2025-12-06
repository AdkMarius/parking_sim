#ifndef MODELS_H
#define MODELS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dimensions maximales (à ajuster selon ton fichier .txt de map)
#define MAX_HEIGHT 50
#define MAX_WIDTH 150
#define CAR_HEIGHT 4
#define CAR_WIDTH 10

// --- Énumérations pour la Machine à États (IA) ---
typedef enum {
    STATE_SEARCHING, // Cherche une place
    STATE_PARKING,   // Manœuvre pour se garer
    STATE_PARKED,    // Garé (accumule du temps)
    STATE_LEAVING,   // Cherche la sortie
    STATE_PAYING,    // À la borne de sortie
    STATE_EXITED     // A quitté la map (prêt à être supprimé)
} CarState;

typedef struct voiture VEHICULE;
struct voiture {
    char direction;          /* N, S, E, O */
    int posx;                /* Coin haut gauche X */
    int posy;                /* Coin haut gauche Y */
    int vitesse;             /* Vitesse de déplacement */
    char alignement;         /* 'g' => gauche, 'd' => droite */
    char type;               /* 'v' => voiture, 'c' => camion */
    char Carrosserie[4][30]; /* Matrice visuelle */
    int code_couleur;        /* Couleur d'affichage */
    char etat;               /* '1' => actif, '0' => inactif */
    unsigned long int tps;   /* Temps passé dans le parking */
    
    CarState logic_state;    // État actuel de l'IA
    int target_x;            // Destination X
    int target_y;            // Destination Y
    int id;                  // Identifiant unique (utile pour le debug)

    // Linked List Pointer
    struct voiture *NXT;     /* Pointeur vers le suivant */
};

// --- Structure CARTE (Gestion des collisions) ---
typedef struct {
    int grid[MAX_HEIGHT][MAX_WIDTH]; // 0=Vide, 1=Mur, 2=Voiture
    int height;
    int width;
    // Points d'intérêts
    int entry_x, entry_y;
    int exit_x, exit_y;
} ParkingMap;

// --- Structure CONTEXTE (État global du jeu) ---
typedef struct {
    VEHICULE *car_list;  // Tête de la liste chaînée
    ParkingMap map;      // La carte
    int game_mode;       // 0=Fluide, 1=Chargé
    int running;         // Flag pour la boucle principale
    int score;           // Revenus du parking
} GameContext;

#endif