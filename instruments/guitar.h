#ifndef GUITAR_H
#define GUITAR_H

#include <stdint.h>
#include "../audio/synthesizer.h"

// Structure de la guitare
typedef struct {
    int sample_rate;
    int num_strings;
    Synthesizer **strings;     // Une voix par corde
    int current_string;        // Prochaine corde à utiliser
    
    // Paramètres d'effets
    float vibrato_depth;       // Profondeur du vibrato
    float vibrato_rate;        // Vitesse du vibrato (Hz)
    float vibrato_time;        // Temps écoulé pour le vibrato
} Guitar;

// Fonctions de la guitare
Guitar *guitar_create(int sample_rate);
void guitar_destroy(Guitar *guitar);
float guitar_generate_sample(Guitar *guitar);
void guitar_pluck_string(Guitar *guitar, int string_index, float frequency, float velocity);
void guitar_pluck_next(Guitar *guitar, float frequency, float velocity);
void guitar_mute_string(Guitar *guitar, int string_index);
void guitar_mute_all(Guitar *guitar);
void guitar_set_vibrato(Guitar *guitar, float depth, float rate);
int guitar_get_active_strings(Guitar *guitar);
float guitar_get_standard_frequency(int string_index);
void guitar_tune_string(Guitar *guitar, int string_index, float frequency);

#endif // GUITAR_H
