// Cette fonction s'ajoute à interface.c pour supporter la lecture de fichiers musicaux

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "music_player.h"

// Menu de lecture de fichiers musicaux
void music_file_menu(AudioEngine *engine) {
    if (!engine) return;
    
    char filename[256];
    
    printf("\n=== Lecteur Fichier Musical ===\n");
    printf("Entrez le chemin du fichier .mus (ou 'q' pour retour): ");
    
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        return;
    }
    
    // Supprimer le newline
    size_t len = strlen(filename);
    if (len > 0 && filename[len-1] == '\n') {
        filename[len-1] = 0;
    }
    
    if (strcmp(filename, "q") == 0 || strcmp(filename, "Q") == 0) {
        return;
    }
    
    // Parser le fichier
    printf("\n[INFO] Chargement du fichier: %s\n", filename);
    Composition *comp = parser_load_file(filename);
    
    if (!comp) {
        printf("[ERREUR] Impossible de charger le fichier\n");
        return;
    }
    
    // Valider
    if (!parser_validate(comp)) {
        printf("[ERREUR] Fichier invalide\n");
        parser_free_composition(comp);
        return;
    }
    
    // Afficher les infos
    parser_print_info(comp);
    
    // Créer le lecteur
    MusicPlayer *player = music_player_create(comp, engine);
    if (!player) {
        printf("[ERREUR] Impossible de créer le lecteur\n");
        parser_free_composition(comp);
        return;
    }
    
    // Démarrer la lecture
    music_player_start(player);
    
    printf("\n=== Contrôles ===\n");
    printf("ESPACE: Pause/Reprendre\n");
    printf("S: Statut\n");
    printf("Q: Quitter\n\n");
    
    // Boucle de lecture
    float update_interval = 0.01f;  // Mise à jour toutes les 10ms
    
    while (player->state != PLAYER_STOPPED) {
        // Vérifier les entrées
        char input = getchar();
        input = tolower(input);
        
        if (input == 'q') {
            music_player_stop(player);
            break;
        } else if (input == ' ') {  // Espace = pause/reprendre
            if (player->state == PLAYER_PLAYING) {
                music_player_pause(player);
                printf("[INFO] Lecture mise en pause\n");
            } else if (player->state == PLAYER_PAUSED) {
                music_player_resume(player);
                printf("[INFO] Lecture reprise\n");
            }
        } else if (input == 's') {
            music_player_print_status(player);
        }
        
        // Mettre à jour la lecture
        if (player->state == PLAYER_PLAYING) {
            music_player_update(player, update_interval);
            audio_engine_process(engine);
            
            // Vérifier si la lecture est terminée
            if (music_player_is_finished(player)) {
                printf("\n[INFO] Lecture terminée\n");
                music_player_stop(player);
                break;
            }
        }
        
        usleep(10000);  // 10ms
    }
    
    // Cleanup
    music_player_destroy(player);
    parser_free_composition(comp);
}

// À intégrer dans le menu principal de interface.c:
// Ajouter cette option au switch du menu:
//
//     case '7':
//         music_file_menu(engine);
//         break;
//
// Et modifier print_menu():
//     printf("7. Lire fichier musical (.mus)\n");
