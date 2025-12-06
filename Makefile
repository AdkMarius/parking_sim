# Compilateur
CC = gcc

# Options de compilation
# -Iinclude : Indique où trouver les fichiers .h
# -Wall -Wextra : Active tous les avertissements (Bonne pratique ingénieur)
# -g : Ajoute les infos de débogage (utile pour gdb ou valgrind)
CFLAGS = -Wall -Wextra -Iinclude -g

# Fichiers sources
# Note: main.c est à la racine, les autres sont dans src/
SRCS = main.c src/game_logic.c src/graphics.c src/utils.c

# Transformation des .c en .o (objets)
OBJS = $(SRCS:.c=.o)

# Nom de l'exécutable final
TARGET = parking_sim

# Règle par défaut
all: $(TARGET)

# Édition de liens (Linking)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
	@echo "Compilation terminée avec succès ! Lancez avec ./$(TARGET)"

# Règle générique pour compiler les .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers compilés
clean:
	rm -f $(OBJS) $(TARGET)

# Nettoyage complet
fclean: clean
	rm -f *~

# Recompiler depuis zéro
re: fclean all

.PHONY: all clean fclean re