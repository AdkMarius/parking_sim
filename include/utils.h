#ifndef UTILS_H
#define UTILS_H

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

// Configure le terminal en mode "raw" (saisie immédiate sans Entrée)
void configure_terminal();

// Restaure le terminal dans son état d'origine (TRES IMPORTANT pour quitter proprement)
void restore_terminal();

// Récupère la touche appuyée sans bloquer le programme (Fonction du PDF)
char key_pressed();

// Retourne le temps actuel en millisecondes (utile pour gérer la vitesse de simulation)
long get_time_ms();

// Petite fonction utilitaire pour faire dormir le programme (en ms)
void sleep_ms(int milliseconds);

#endif