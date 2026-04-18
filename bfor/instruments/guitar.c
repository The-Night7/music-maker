#include <stdlib.h>
#include <math.h>
#include "guitar.h"
#include "../audio/synthesizer.h"

// Créer une guitare
Guitar *guitar_create(int sample_rate) {
    Guitar *guitar = (Guitar *)malloc(sizeof(Guitar));
    if (!guitar) return NULL;
    
    guitar->sample_rate = sample_rate;
    guitar->num_strings = 6;
    guitar->strings = (Synthesizer **)malloc(sizeof(Synthesizer *) * guitar->num_strings);
    
    if (!guitar->strings) {
        free(guitar);
        return NULL;
    }
    
    // Créer un synthétiseur pour chaque corde
    // Utiliser une onde triangulaire pour une sonorité proche de la guitare
    for (int i = 0; i < guitar->num_strings; i++) {
        guitar->strings[i] = synthesizer_create(sample_rate, TRIANGLE_WAVE);
        
        // Configurer les paramètres ADSR pour imiter une guitare acoustique
        if (guitar->strings[i]) {
            guitar->strings[i]->attack = 0.01f;    // 10ms
            guitar->strings[i]->decay = 0.5f;      // 500ms
            guitar->strings[i]->sustain = 0.3f;    // 30% du volume
            guitar->strings[i]->release = 0.4f;    // 400ms
        }
    }
    
    guitar->current_string = 0;
    guitar->vibrato_depth = 0.0f;
    guitar->vibrato_rate = 5.0f;  // 5 Hz
    guitar->vibrato_time = 0.0f;
    
    return guitar;
}

// Détruire la guitare
void guitar_destroy(Guitar *guitar) {
    if (!guitar) return;
    
    for (int i = 0; i < guitar->num_strings; i++) {
        if (guitar->strings[i]) {
            synthesizer_destroy(guitar->strings[i]);
        }
    }
    
    free(guitar->strings);
    free(guitar);
}

// Générer un échantillon audio de la guitare
float guitar_generate_sample(Guitar *guitar) {
    float output = 0.0f;
    
    // Mixer toutes les cordes
    for (int i = 0; i < guitar->num_strings; i++) {
        if (synthesizer_is_playing(guitar->strings[i])) {
            float sample = synthesizer_generate_sample(guitar->strings[i]);
            output += sample;
        }
    }
    
    // Normaliser
    if (guitar->num_strings > 0) {
        output /= (float)guitar->num_strings;
    }
    
    return output;
}

// Jouer une note sur une corde spécifique
void guitar_pluck_string(Guitar *guitar, int string_index, float frequency, float velocity) {
    if (string_index < 0 || string_index >= guitar->num_strings) return;
    
    synthesizer_note_on(guitar->strings[string_index], frequency);
    synthesizer_set_amplitude(guitar->strings[string_index], velocity);
}

// Jouer une note sur la corde suivante disponible (polyphonie)
void guitar_pluck_next(Guitar *guitar, float frequency, float velocity) {
    int string_index = guitar->current_string;
    
    guitar->current_string = (guitar->current_string + 1) % guitar->num_strings;
    
    guitar_pluck_string(guitar, string_index, frequency, velocity);
}

// Arrêter une corde spécifique
void guitar_mute_string(Guitar *guitar, int string_index) {
    if (string_index < 0 || string_index >= guitar->num_strings) return;
    
    synthesizer_note_off(guitar->strings[string_index]);
}

// Arrêter toutes les cordes
void guitar_mute_all(Guitar *guitar) {
    for (int i = 0; i < guitar->num_strings; i++) {
        guitar_mute_string(guitar, i);
    }
}

// Activer le vibrato
void guitar_set_vibrato(Guitar *guitar, float depth, float rate) {
    guitar->vibrato_depth = depth;
    guitar->vibrato_rate = rate;
}

// Obtenir le nombre de cordes actives
int guitar_get_active_strings(Guitar *guitar) {
    int count = 0;
    for (int i = 0; i < guitar->num_strings; i++) {
        if (synthesizer_is_playing(guitar->strings[i])) {
            count++;
        }
    }
    return count;
}

// Fréquences standard des cordes de guitare (Mi, La, Ré, Sol, Si, Mi)
static const float GUITAR_STANDARD_TUNING[6] = {
    82.41f,   // E2 - Corde 6
    110.0f,   // A2 - Corde 5
    146.83f,  // D3 - Corde 4
    196.0f,   // G3 - Corde 3
    246.94f,  // B3 - Corde 2
    329.63f   // E4 - Corde 1
};

// Obtenir la fréquence standard d'une corde
float guitar_get_standard_frequency(int string_index) {
    if (string_index < 0 || string_index >= 6) return 0.0f;
    return GUITAR_STANDARD_TUNING[string_index];
}

// Accorder une corde
void guitar_tune_string(Guitar *guitar, int string_index, float frequency) {
    if (string_index < 0 || string_index >= guitar->num_strings) return;
    
    // Simplement enregistrer la nouvelle fréquence
    // La fréquence sera utilisée lors du prochain pluck
    (void)frequency; // Pour éviter les avertissements
}
