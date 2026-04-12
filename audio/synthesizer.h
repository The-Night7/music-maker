#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <stdint.h>

// Types d'ondes disponibles
typedef enum {
    SINE_WAVE,
    SQUARE_WAVE,
    TRIANGLE_WAVE,
    SAWTOOTH_WAVE
} SynthType;

// Structure du synthétiseur avec enveloppe ADSR (déclaration forward)
struct Synthesizer;
typedef struct Synthesizer Synthesizer;

// Structure du synthétiseur avec enveloppe ADSR
struct Synthesizer {
    int sample_rate;
    SynthType type;
    
    // Paramètres de l'onde
    float phase;
    float frequency;
    float amplitude;
    
    // Paramètres ADSR
    float attack;   // Temps d'attaque
    float decay;    // Temps de décroissance
    float sustain;  // Niveau de sustain
    float release;  // Temps de relâchement
    float envelope_time;
    
    // État
    int is_playing;
};

// Fonctions du synthétiseur
Synthesizer *synthesizer_create(int sample_rate, SynthType type);
void synthesizer_destroy(Synthesizer *synth);
float synthesizer_generate_sample(Synthesizer *synth);
void synthesizer_set_frequency(Synthesizer *synth, float frequency);
void synthesizer_set_amplitude(Synthesizer *synth, float amplitude);
void synthesizer_note_on(Synthesizer *synth, float frequency);
void synthesizer_note_off(Synthesizer *synth);
int synthesizer_is_playing(Synthesizer *synth);

// Utilitaires pour les fréquences MIDI
#define MIDI_TO_FREQUENCY(note) (440.0f * powf(2.0f, (note - 69) / 12.0f))

#endif // SYNTHESIZER_H
