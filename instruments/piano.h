#ifndef PIANO_H
#define PIANO_H

#include <stdint.h>
#include "../audio/synthesizer.h"

// Structure du piano
typedef struct {
    int sample_rate;
    int num_voices;
    Synthesizer **voices;   // Polyphonie: plusieurs voix simultanées
    int current_voice;      // Prochaine voix à utiliser (round-robin)
    int sustain_pedal;      // État de la pédale de sustain
} Piano;

// Fonctions du piano
Piano *piano_create(int sample_rate);
void piano_destroy(Piano *piano);
float piano_generate_sample(Piano *piano);
void piano_note_on(Piano *piano, float frequency, float velocity);
void piano_note_off(Piano *piano, int voice_index);
void piano_set_sustain_pedal(Piano *piano, int active);
int piano_get_active_voices(Piano *piano);

#endif // PIANO_H
