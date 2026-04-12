#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <stdint.h>

typedef struct {
    float frequency;
    float amplitude;
    float phase;
    int sample_rate;
} Oscillator;

typedef struct {
    int sample_rate;
    int channels;
    int buffer_size;
    float *audio_buffer;
    int is_running;
    void *mixer; // Pointeur vers le mixeur
} AudioEngine;

// Fonctions du moteur audio
AudioEngine *audio_engine_create(void);
void audio_engine_destroy(AudioEngine *engine);
int audio_engine_start(AudioEngine *engine);
int audio_engine_stop(AudioEngine *engine);
void audio_engine_process(AudioEngine *engine);
void audio_engine_add_track(AudioEngine *engine, void *track);

#endif // AUDIO_ENGINE_H
