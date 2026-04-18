#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "music_player.h"

// Créer un lecteur musical
MusicPlayer *music_player_create(Composition *comp, AudioEngine *engine) {
    if (!comp || !engine) return NULL;
    
    MusicPlayer *player = (MusicPlayer *)malloc(sizeof(MusicPlayer));
    if (!player) return NULL;
    
    player->composition = comp;
    player->engine = engine;
    player->state = PLAYER_STOPPED;
    player->elapsed_time = 0.0f;
    player->note_start_time = 0.0f;
    player->current_note_duration = 0.0f;
    
    for (int i = 0; i < 3; i++) {
        player->current_note_index[i] = 0;
        player->active_instruments[i] = 0;
    }
    
    // Créer les instruments
    player->piano = piano_create(engine->sample_rate);
    player->drums = drums_create(engine->sample_rate);
    player->guitar = guitar_create(engine->sample_rate);
    
    if (!player->piano || !player->drums || !player->guitar) {
        printf("[ERREUR] Impossible de créer les instruments\n");
        free(player);
        return NULL;
    }
    
    return player;
}

// Détruire le lecteur
void music_player_destroy(MusicPlayer *player) {
    if (!player) return;
    
    if (player->piano) piano_destroy(player->piano);
    if (player->drums) drums_destroy(player->drums);
    if (player->guitar) guitar_destroy(player->guitar);
    
    free(player);
}

// Interpréter une note pour la batterie
static void play_drum_note(Drums *drums, const char *note, float velocity) {
    if (strcmp(note, "K") == 0 || strcmp(note, "kick") == 0) {
        drums_kick(drums, velocity);
    } else if (strcmp(note, "S") == 0 || strcmp(note, "snare") == 0) {
        drums_snare(drums, velocity);
    } else if (strcmp(note, "H") == 0 || strcmp(note, "hihat") == 0) {
        drums_hihat_closed(drums, velocity);
    } else if (strcmp(note, "O") == 0 || strcmp(note, "open") == 0) {
        drums_hihat_open(drums, velocity);
    }
}

// Démarrer la lecture
int music_player_start(MusicPlayer *player) {
    if (!player) return -1;
    
    player->state = PLAYER_PLAYING;
    player->elapsed_time = 0.0f;
    
    for (int i = 0; i < 3; i++) {
        player->current_note_index[i] = 0;
    }
    
    printf("[INFO] Début de la lecture: %s\n", player->composition->titre);
    
    return 0;
}

// Arrêter la lecture
int music_player_stop(MusicPlayer *player) {
    if (!player) return -1;
    
    player->state = PLAYER_STOPPED;
    player->elapsed_time = 0.0f;
    
    // Arrêter tous les sons
    drums_stop_all(player->drums);
    guitar_mute_all(player->guitar);
    
    printf("[INFO] Lecture arrêtée\n");
    
    return 0;
}

// Mettre en pause
int music_player_pause(MusicPlayer *player) {
    if (!player) return -1;
    
    if (player->state == PLAYER_PLAYING) {
        player->state = PLAYER_PAUSED;
        printf("[INFO] Lecture mise en pause\n");
        return 0;
    }
    
    return -1;
}

// Reprendre la lecture
int music_player_resume(MusicPlayer *player) {
    if (!player) return -1;
    
    if (player->state == PLAYER_PAUSED) {
        player->state = PLAYER_PLAYING;
        printf("[INFO] Lecture reprise\n");
        return 0;
    }
    
    return -1;
}

// Vérifier si la lecture est terminée
int music_player_is_finished(MusicPlayer *player) {
    if (!player || player->state == PLAYER_STOPPED) return 1;
    
    for (int i = 0; i < player->composition->num_instruments; i++) {
        if (player->current_note_index[i] < player->composition->instruments[i].num_notes) {
            return 0;
        }
    }
    
    return 1;
}

// Mettre à jour la lecture (doit être appelée régulièrement)
void music_player_update(MusicPlayer *player, float delta_time) {
    if (!player || player->state != PLAYER_PLAYING) return;
    
    player->elapsed_time += delta_time;
    
    for (int instr_idx = 0; instr_idx < player->composition->num_instruments; instr_idx++) {
        Instrument *instr = &player->composition->instruments[instr_idx];
        int current_idx = player->current_note_index[instr_idx];
        
        if (current_idx >= instr->num_notes) {
            continue;  // Cet instrument est terminé
        }
        
        MusicalNote *note = &instr->notes[current_idx];
        float note_duration = parser_beat_to_duration(note->duration, player->composition->tempo);
        
        // Commencer une nouvelle note
        if (player->note_start_time == 0.0f || 
            (player->elapsed_time - player->note_start_time) >= player->current_note_duration) {
            
            player->note_start_time = player->elapsed_time;
            player->current_note_duration = note_duration;
            
            // Jouer la note sur l'instrument approprié
            float frequency = parser_note_to_frequency(note->note);
            
            if (frequency > 0.0f) {  // Pas un silence
                switch (instr->type) {
                    case INSTR_PIANO:
                        piano_note_on(player->piano, frequency, note->velocity);
                        break;
                    case INSTR_DRUMS:
                        play_drum_note(player->drums, note->note, note->velocity);
                        break;
                    case INSTR_GUITAR:
                        guitar_pluck_next(player->guitar, frequency, note->velocity);
                        break;
                    default:
                        break;
                }
            }
            
            // Passer à la note suivante
            player->current_note_index[instr_idx]++;
        }
    }
}

// Afficher l'état du lecteur
void music_player_print_status(MusicPlayer *player) {
    if (!player) return;
    
    printf("\n=== Lecteur Musical ===\n");
    printf("Composition: %s\n", player->composition->titre);
    printf("Tempo: %d BPM\n", player->composition->tempo);
    printf("État: ");
    
    switch (player->state) {
        case PLAYER_STOPPED: printf("Arrêté\n"); break;
        case PLAYER_PLAYING: printf("En lecture\n"); break;
        case PLAYER_PAUSED: printf("En pause\n"); break;
    }
    
    printf("Temps écoulé: %.2f s\n", player->elapsed_time);
    printf("\nProgression par instrument:\n");
    
    for (int i = 0; i < player->composition->num_instruments; i++) {
        Instrument *instr = &player->composition->instruments[i];
        int current = player->current_note_index[i];
        int total = instr->num_notes;
        
        printf("  %s: %d/%d notes\n", 
               parser_instrument_type_name(instr->type),
               current, total);
    }
}
