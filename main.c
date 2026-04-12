#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "audio/audio_engine.h"
#include "ui/interface.h"

int main(int argc, char *argv[]) {
    printf("=== Application Musicale Multi-Instruments ===\n\n");
    
    // Initialiser le moteur audio
    printf("[INFO] Initialisation du moteur audio...\n");
    AudioEngine *engine = audio_engine_create();
    if (!engine) {
        fprintf(stderr, "[ERREUR] Impossible d'initialiser le moteur audio\n");
        return 1;
    }
    
    // Démarrer le moteur audio
    if (audio_engine_start(engine) != 0) {
        fprintf(stderr, "[ERREUR] Impossible de démarrer le moteur audio\n");
        audio_engine_destroy(engine);
        return 1;
    }
    
    printf("[OK] Moteur audio démarré\n");
    printf("[INFO] Lancement de l'interface utilisateur...\n\n");
    
    // Lancer l'interface utilisateur
    ui_run(engine);
    
    // Arrêter le moteur audio
    printf("\n[INFO] Arrêt du moteur audio...\n");
    audio_engine_stop(engine);
    audio_engine_destroy(engine);
    
    printf("[OK] Application fermée\n");
    return 0;
}
