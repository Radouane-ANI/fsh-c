.PHONY: all clean 

# Nom de l'exécutable
TARGET = fsh

# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -g

# Bibliothèques nécessaires
LIBS = -lreadline

# Fichiers sources et objets
SRC = fsh.c interne.c externe.c for.c split.c\
	free.c signal.c prompt.c main.c if.c 
OBJ = $(SRC:.c=.o)

# Règle par défaut : compiler l'exécutable
all: $(TARGET)

# Règle pour créer l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

# Règle pour compiler les fichiers .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour nettoyer les fichiers objets et l'exécutable
clean:
	rm -f $(OBJ) $(TARGET)
