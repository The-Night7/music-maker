#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mixer.h"

#define MAX_TRACKS 16
#define MAX_SAMPLE_VALUE 1.0f
#define MIN_SAMPLE_VALUE -1.0f

// Créer un mixeur
Mixer *mixer_create(int num_tracks, int buffer_size, int sample_rate) {
    if (num_tracks > MAX_TRACKS) {
        fprintf(stderr, "[ERREUR] Le nombre de pistes dépassse le maximum (%d)\n", MAX_TRACKS);
        return NULL;
    }
    
    Mixer *mixer = (Mixer *)malloc(sizeof(Mixer));
    if (!mixer) return NULL;
    
    mixer->num_tracks = num_tracks;
    mixer->buffer_size = buffer_size;
    mixer->sample_rate = sample_rate;
    mixer->master_volume = 0.8f;
    
    // Initialiser les pistes
    mixer->tracks = (MixerTrack *)malloc(sizeof(MixerTrack) * num_tracks);
    if (!mixer->tracks) {
        free(mixer);
        return NULL;
    }
    
    for (int i = 0; i < num_tracks; i++) {
        mixer->tracks[i].volume = 0.8f;
        mixer->tracks[i].pan = 0.0f; // Centre
        mixer->tracks[i].muted = 0;
        mixer->tracks[i].solo = 0;
        mixer->tracks[i].instrument = NULL;
        mixer->tracks[i].buffer = (float *)malloc(sizeof(float) * buffer_size);
        
        if (!mixer->tracks[i].buffer) {
            for (int j = 0; j < i; j++) {
                free(mixer->tracks[j].buffer);
            }
            free(mixer->tracks);
            free(mixer);
            return NULL;
        }
    }
    
    // Allouer le buffer de sortie
    mixer->output_buffer = (float *)malloc(sizeof(float) * buffer_size);
    if (!mixer->output_buffer) {
        for (int i = 0; i < num_tracks; i++) {
            free(mixer->tracks[i].buffer);
        }
        free(mixer->tracks);
        free(mixer);
        return NULL;
    }
    
    return mixer;
}

// Détruire le mixeur
void mixer_destroy(Mixer *mixer) {
    if (!mixer) return;
    
    for (int i = 0; i < mixer->num_tracks; i++) {
        if (mixer->tracks[i].buffer) {
            free(mixer->tracks[i].buffer);
        }
    }
    
    if (mixer->tracks) free(mixer->tracks);
    if (mixer->output_buffer) free(mixer->output_buffer);
    free(mixer);
}

// Ajouter un instrument à une piste
int mixer_add_instrument(Mixer *mixer, int track_index, void *instrument) {
    if (track_index < 0 || track_index >= mixer->num_tracks) {
        fprintf(stderr, "[ERREUR] Index de piste invalide: %d\n", track_index);
        return -1;
    }
    
    mixer->tracks[track_index].instrument = instrument;
    return 0;
}

// Définir le volume d'une piste
void mixer_set_track_volume(Mixer *mixer, int track_index, float volume) {
    if (track_index < 0 || track_index >= mixer->num_tracks) return;
    
    if (volume > 1.0f) volume = 1.0f;
    if (volume < 0.0f) volume = 0.0f;
    
    mixer->tracks[track_index].volume = volume;
}

// Définir le panoramique d'une piste
void mixer_set_track_pan(Mixer *mixer, int track_index, float pan) {
    if (track_index < 0 || track_index >= mixer->num_tracks) return;
    
    if (pan > 1.0f) pan = 1.0f;
    if (pan < -1.0f) pan = -1.0f;
    
    mixer->tracks[track_index].pan = pan;
}

// Mettre en sourdine une piste
void mixer_mute_track(Mixer *mixer, int track_index, int mute) {
    if (track_index < 0 || track_index >= mixer->num_tracks) return;
    mixer->tracks[track_index].muted = mute;
}

// Activer le solo pour une piste
void mixer_solo_track(Mixer *mixer, int track_index, int solo) {
    if (track_index < 0 || track_index >= mixer->num_tracks) return;
    mixer->tracks[track_index].solo = solo;
}

// Définir le volume maître
void mixer_set_master_volume(Mixer *mixer, float volume) {
    if (volume > 1.0f) volume = 1.0f;
    if (volume < 0.0f) volume = 0.0f;
    mixer->master_volume = volume;
}

// Appliquer le panoramique à un échantillon
static float apply_pan(float sample, float pan) {
    if (pan > 0.0f) {
        // Panoramique à droite
        return sample * (1.0f - pan);
    } else if (pan < 0.0f) {
        // Panoramique à gauche
        return sample * (1.0f + pan);
    }
    return sample;
}

// Mixer tous les signaux
void mixer_process(Mixer *mixer) {
    // Réinitialiser le buffer de sortie
    memset(mixer->output_buffer, 0, sizeof(float) * mixer->buffer_size);
    
    // Vérifier s'il y a une piste en solo
    int has_solo = 0;
    for (int i = 0; i < mixer->num_tracks; i++) {
        if (mixer->tracks[i].solo) {
            has_solo = 1;
            break;
        }
    }
    
    // Mixer chaque piste
    for (int i = 0; i < mixer->num_tracks; i++) {
        MixerTrack *track = &mixer->tracks[i];
        
        // Sauter les pistes muettes
        if (track->muted) continue;
        
        // Sauter les pistes non en solo si une autre l'est
        if (has_solo && !track->solo) continue;
        
        if (track->instrument) {
            // Générer les échantillons de la piste
            for (int j = 0; j < mixer->buffer_size; j++) {
                // La fonction generate_sample dépendra du type d'instrument
                // Pour l'instant, on suppose que l'instrument a une telle fonction
                float sample = 0.0f; // À implémenter avec l'instrument spécifique
                
                // Appliquer le volume de la piste et le panoramique
                sample *= track->volume;
                sample = apply_pan(sample, track->pan);
                
                // Ajouter au buffer de sortie
                mixer->output_buffer[j] += sample;
            }
        }
    }
    
    // Appliquer le volume maître et limiter le signal
    for (int i = 0; i < mixer->buffer_size; i++) {
        mixer->output_buffer[i] *= mixer->master_volume;
        
        // Soft clipping pour éviter la distorsion
        float sample = mixer->output_buffer[i];
        if (sample > MAX_SAMPLE_VALUE) {
            mixer->output_buffer[i] = MAX_SAMPLE_VALUE;
        } else if (sample < MIN_SAMPLE_VALUE) {
            mixer->output_buffer[i] = MIN_SAMPLE_VALUE;
        }
    }
}

// Obtenir le buffer de sortie du mixeur
float *mixer_get_output(Mixer *mixer) {
    return mixer->output_buffer;
}

// Obtenir le nombre de pistes
int mixer_get_num_tracks(Mixer *mixer) {
    return mixer->num_tracks;
}

// Obtenir le volume d'une piste
float mixer_get_track_volume(Mixer *mixer, int track_index) {
    if (track_index < 0 || track_index >= mixer->num_tracks) return 0.0f;
    return mixer->tracks[track_index].volume;
}

// Afficher les informations du mixeur
void mixer_print_status(Mixer *mixer) {
    printf("\n=== État du Mixeur ===\n");
    printf("Nombre de pistes: %d\n", mixer->num_tracks);
    printf("Volume maître: %.2f (%.0f%%)\n", mixer->master_volume, mixer->master_volume * 100.0f);
    
    for (int i = 0; i < mixer->num_tracks; i++) {
        printf("  Piste %d: Volume=%.2f (%.0f%%), Pan=%+.2f, ", i,
               mixer->tracks[i].volume, mixer->tracks[i].volume * 100.0f,
               mixer->tracks[i].pan);
        
        if (mixer->tracks[i].muted) printf("Muette");
        else if (mixer->tracks[i].solo) printf("Solo");
        else printf("Normale");
        
        printf("\n");
    }
}
