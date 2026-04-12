# Makefile pour l'application musicale multi-instruments en C

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -fPIC
LDFLAGS = -lm

# Répertoires
SRC_DIR = .
AUDIO_DIR = audio
INSTRUMENTS_DIR = instruments
UI_DIR = ui
UTILS_DIR = utils
BUILD_DIR = build
BIN_DIR = bin

# Fichiers sources
AUDIO_SOURCES = $(AUDIO_DIR)/audio_engine.c $(AUDIO_DIR)/synthesizer.c $(AUDIO_DIR)/mixer.c
INSTRUMENTS_SOURCES = $(INSTRUMENTS_DIR)/piano.c $(INSTRUMENTS_DIR)/drums.c $(INSTRUMENTS_DIR)/guitar.c
UI_SOURCES = $(UI_DIR)/interface.c
UTILS_SOURCES = $(UTILS_DIR)/wave.c
MAIN_SOURCE = main.c

ALL_SOURCES = $(MAIN_SOURCE) $(AUDIO_SOURCES) $(INSTRUMENTS_SOURCES) $(UI_SOURCES) $(UTILS_SOURCES)

# Fichiers objets
AUDIO_OBJECTS = $(BUILD_DIR)/audio_engine.o $(BUILD_DIR)/synthesizer.o $(BUILD_DIR)/mixer.o
INSTRUMENTS_OBJECTS = $(BUILD_DIR)/piano.o $(BUILD_DIR)/drums.o $(BUILD_DIR)/guitar.o
UI_OBJECTS = $(BUILD_DIR)/interface.o
UTILS_OBJECTS = $(BUILD_DIR)/wave.o
MAIN_OBJECT = $(BUILD_DIR)/main.o

ALL_OBJECTS = $(MAIN_OBJECT) $(AUDIO_OBJECTS) $(INSTRUMENTS_OBJECTS) $(UI_OBJECTS) $(UTILS_OBJECTS)

# Cibles
TARGET = $(BIN_DIR)/music_app

# Cible par défaut
.PHONY: all clean run debug help

all: $(TARGET)

# Règle de compilation principale
$(TARGET): $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "[LINK] Compilation de $(TARGET)..."
	$(CC) $(ALL_OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "[OK] Application compilée: $(TARGET)"

# Règle pour le main
$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	@echo "[CC] main.c"
	$(CC) $(CFLAGS) -c main.c -o $@

# Règles pour audio
$(BUILD_DIR)/audio_engine.o: $(AUDIO_DIR)/audio_engine.c $(AUDIO_DIR)/audio_engine.h | $(BUILD_DIR)
	@echo "[CC] $(AUDIO_DIR)/audio_engine.c"
	$(CC) $(CFLAGS) -c $(AUDIO_DIR)/audio_engine.c -o $@

$(BUILD_DIR)/synthesizer.o: $(AUDIO_DIR)/synthesizer.c $(AUDIO_DIR)/synthesizer.h | $(BUILD_DIR)
	@echo "[CC] $(AUDIO_DIR)/synthesizer.c"
	$(CC) $(CFLAGS) -c $(AUDIO_DIR)/synthesizer.c -o $@

$(BUILD_DIR)/mixer.o: $(AUDIO_DIR)/mixer.c $(AUDIO_DIR)/mixer.h | $(BUILD_DIR)
	@echo "[CC] $(AUDIO_DIR)/mixer.c"
	$(CC) $(CFLAGS) -c $(AUDIO_DIR)/mixer.c -o $@

# Règles pour instruments
$(BUILD_DIR)/piano.o: $(INSTRUMENTS_DIR)/piano.c $(INSTRUMENTS_DIR)/piano.h | $(BUILD_DIR)
	@echo "[CC] $(INSTRUMENTS_DIR)/piano.c"
	$(CC) $(CFLAGS) -c $(INSTRUMENTS_DIR)/piano.c -o $@

$(BUILD_DIR)/drums.o: $(INSTRUMENTS_DIR)/drums.c $(INSTRUMENTS_DIR)/drums.h | $(BUILD_DIR)
	@echo "[CC] $(INSTRUMENTS_DIR)/drums.c"
	$(CC) $(CFLAGS) -c $(INSTRUMENTS_DIR)/drums.c -o $@

$(BUILD_DIR)/guitar.o: $(INSTRUMENTS_DIR)/guitar.c $(INSTRUMENTS_DIR)/guitar.h | $(BUILD_DIR)
	@echo "[CC] $(INSTRUMENTS_DIR)/guitar.c"
	$(CC) $(CFLAGS) -c $(INSTRUMENTS_DIR)/guitar.c -o $@

# Règles pour UI
$(BUILD_DIR)/interface.o: $(UI_DIR)/interface.c $(UI_DIR)/interface.h | $(BUILD_DIR)
	@echo "[CC] $(UI_DIR)/interface.c"
	$(CC) $(CFLAGS) -c $(UI_DIR)/interface.c -o $@

# Règles pour utils
$(BUILD_DIR)/wave.o: $(UTILS_DIR)/wave.c $(UTILS_DIR)/wave.h | $(BUILD_DIR)
	@echo "[CC] $(UTILS_DIR)/wave.c"
	$(CC) $(CFLAGS) -c $(UTILS_DIR)/wave.c -o $@

# Créer les répertoires de build
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Nettoyer les fichiers compilés
clean:
	@echo "[CLEAN] Suppression des fichiers compilés..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "[OK] Nettoyage terminé"

# Compiler avec les symboles de débogage
debug: CFLAGS += -g -DDEBUG
debug: clean all

# Lancer l'application
run: all
	@echo "[RUN] Lancement de l'application..."
	./$(TARGET)

# Afficher l'aide
help:
	@echo "=== Makefile pour l'Application Musicale Multi-Instruments ==="
	@echo ""
	@echo "Cibles disponibles:"
	@echo "  make              - Compiler l'application (défaut)"
	@echo "  make run          - Compiler et lancer l'application"
	@echo "  make debug        - Compiler avec les symboles de débogage"
	@echo "  make clean        - Supprimer les fichiers compilés"
	@echo "  make help         - Afficher cette aide"
	@echo ""
	@echo "Exemple d'utilisation:"
	@echo "  make              # Compilation standard"
	@echo "  make run          # Compilation et exécution"
	@echo "  make debug        # Compilation avec débogage"
	@echo "  make clean        # Nettoyage"

.PHONY: $(BIN_DIR) $(BUILD_DIR)
