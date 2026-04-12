#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "audio_engine.h"
#include "mixer.h"

// Créer le moteur audio
AudioEngine *audio_engine_create(void) {
    AudioEngine *engine = (AudioEngine *)malloc(sizeof(AudioEngine));
    if (!engine) {
        fprintf(stderr, "[ERREUR] Impossible d'allouer la mémoire pour le moteur audio\n");
        return NULL;
    }
    
    engine->sample_rate = 44100;
    engine->channels = 2;
    engine->buffer_size = 1024;
    engine->is_running = 0;
    
    // Allouer le buffer audio
    engine->audio_buffer = (float *)malloc(sizeof(float) * engine->buffer_size * engine->channels);
    if (!engine->audio_buffer) {
        fprintf(stderr, "[ERREUR] Impossible d'allouer le buffer audio\n");
        free(engine);
        return NULL;
    }
    
    // Créer le mixeur
    engine->mixer = mixer_create(4, engine->buffer_size, engine->sample_rate);
    if (!engine->mixer) {
        fprintf(stderr, "[ERREUR] Impossible de créer le mixeur\n");
        free(engine->audio_buffer);
        free(engine);
        return NULL;
    }
    
    memset(engine->audio_buffer, 0, sizeof(float) * engine->buffer_size * engine->channels);
    
    return engine;
}

// Détruire le moteur audio
void audio_engine_destroy(AudioEngine *engine) {
    if (!engine) return;
    
    if (engine->mixer) {
        mixer_destroy((Mixer *)engine->mixer);
    }
    
    if (engine->audio_buffer) {
        free(engine->audio_buffer);
    }
    
    free(engine);
}

// Démarrer le moteur audio
int audio_engine_start(AudioEngine *engine) {
    if (!engine) return -1;
    
    engine->is_running = 1;
    printf("[OK] Moteur audio démarré (Fréq: %dHz, Buffer: %d samples)\n",
           engine->sample_rate, engine->buffer_size);
    
    return 0;
}

// Arrêter le moteur audio
int audio_engine_stop(AudioEngine *engine) {
    if (!engine) return -1;
    
    engine->is_running = 0;
    printf("[OK] Moteur audio arrêté\n");
    
    return 0;
}

// Traiter le moteur audio
void audio_engine_process(AudioEngine *engine) {
    if (!engine || !engine->is_running) return;
    
    Mixer *mixer = (Mixer *)engine->mixer;
    
    // Traiter le mixeur
    mixer_process(mixer);
    
    // Obtenir le buffer de sortie du mixeur
    float *output = mixer_get_output(mixer);
    
    // Copier dans le buffer du moteur audio
    memcpy(engine->audio_buffer, output, sizeof(float) * engine->buffer_size);
}

// Ajouter une piste
void audio_engine_add_track(AudioEngine *engine, void *track) {
    if (!engine || !track) return;
    
    Mixer *mixer = (Mixer *)engine->mixer;
    // La fonction spécifique d'ajout de piste dépendra du type d'instrument
    (void)mixer; // Pour éviter les avertissements du compilateur
}

// Obtenir la fréquence d'échantillonnage
int audio_engine_get_sample_rate(AudioEngine *engine) {
    return engine ? engine->sample_rate : 0;
}

// Obtenir la taille du buffer
int audio_engine_get_buffer_size(AudioEngine *engine) {
    return engine ? engine->buffer_size : 0;
}

// Obtenir le mixeur
Mixer *audio_engine_get_mixer(AudioEngine *engine) {
    return engine ? (Mixer *)engine->mixer : NULL;
}
