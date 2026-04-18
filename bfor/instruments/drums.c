#include <stdlib.h>
#include <math.h>
#include "drums.h"
#include "../audio/synthesizer.h"

#define PI 3.14159265359

// Créer une batterie
Drums *drums_create(int sample_rate) {
    Drums *drums = (Drums *)malloc(sizeof(Drums));
    if (!drums) return NULL;
    
    drums->sample_rate = sample_rate;
    
    // Créer les synthétiseurs pour chaque tambour
    drums->kick = synthesizer_create(sample_rate, SINE_WAVE);
    drums->snare = synthesizer_create(sample_rate, SQUARE_WAVE);
    drums->hihat_closed = synthesizer_create(sample_rate, SAWTOOTH_WAVE);
    drums->hihat_open = synthesizer_create(sample_rate, TRIANGLE_WAVE);
    
    // Configurer les paramètres ADSR pour la kick
    if (drums->kick) {
        drums->kick->attack = 0.001f;
        drums->kick->decay = 0.3f;
        drums->kick->sustain = 0.0f;
        drums->kick->release = 0.05f;
    }
    
    // Configurer les paramètres ADSR pour la snare
    if (drums->snare) {
        drums->snare->attack = 0.005f;
        drums->snare->decay = 0.15f;
        drums->snare->sustain = 0.0f;
        drums->snare->release = 0.1f;
    }
    
    drums->hihat_open_time = 0;
    drums->hihat_decay = 0.1f;
    
    return drums;
}

// Détruire la batterie
void drums_destroy(Drums *drums) {
    if (!drums) return;
    
    if (drums->kick) synthesizer_destroy(drums->kick);
    if (drums->snare) synthesizer_destroy(drums->snare);
    if (drums->hihat_closed) synthesizer_destroy(drums->hihat_closed);
    if (drums->hihat_open) synthesizer_destroy(drums->hihat_open);
    
    free(drums);
}

// Générer un échantillon audio de la batterie
float drums_generate_sample(Drums *drums) {
    float output = 0.0f;
    
    // Ajouter la kick
    if (synthesizer_is_playing(drums->kick)) {
        output += synthesizer_generate_sample(drums->kick) * 0.5f;
    }
    
    // Ajouter la snare
    if (synthesizer_is_playing(drums->snare)) {
        output += synthesizer_generate_sample(drums->snare) * 0.3f;
    }
    
    // Ajouter le hi-hat fermé
    if (synthesizer_is_playing(drums->hihat_closed)) {
        output += synthesizer_generate_sample(drums->hihat_closed) * 0.2f;
    }
    
    // Ajouter le hi-hat ouvert avec decay exponentiel
    if (synthesizer_is_playing(drums->hihat_open)) {
        float envelope = expf(-drums->hihat_decay * drums->hihat_open_time);
        output += synthesizer_generate_sample(drums->hihat_open) * envelope * 0.2f;
        drums->hihat_open_time += 1.0f / drums->sample_rate;
    }
    
    return output * 0.5f; // Normaliser
}

// Déclencher la kick drum
void drums_kick(Drums *drums, float intensity) {
    if (!drums || !drums->kick) return;
    
    // Fréquence de la kick drum (environ 60 Hz)
    synthesizer_note_on(drums->kick, 60.0f);
    synthesizer_set_amplitude(drums->kick, intensity);
}

// Déclencher la snare drum
void drums_snare(Drums *drums, float intensity) {
    if (!drums || !drums->snare) return;
    
    // Fréquence de la snare drum (environ 200 Hz)
    synthesizer_note_on(drums->snare, 200.0f);
    synthesizer_set_amplitude(drums->snare, intensity);
}

// Déclencher le hi-hat fermé
void drums_hihat_closed(Drums *drums, float intensity) {
    if (!drums || !drums->hihat_closed) return;
    
    // Fréquence haute pour le hi-hat (environ 8000 Hz)
    synthesizer_note_on(drums->hihat_closed, 8000.0f);
    synthesizer_set_amplitude(drums->hihat_closed, intensity * 0.5f);
}

// Déclencher le hi-hat ouvert
void drums_hihat_open(Drums *drums, float intensity) {
    if (!drums || !drums->hihat_open) return;
    
    // Fréquence plus basse pour le hi-hat ouvert
    synthesizer_note_on(drums->hihat_open, 9000.0f);
    synthesizer_set_amplitude(drums->hihat_open, intensity * 0.6f);
    drums->hihat_open_time = 0;
}

// Arrêter tous les tambours
void drums_stop_all(Drums *drums) {
    if (!drums) return;
    
    if (drums->kick) synthesizer_note_off(drums->kick);
    if (drums->snare) synthesizer_note_off(drums->snare);
    if (drums->hihat_closed) synthesizer_note_off(drums->hihat_closed);
    if (drums->hihat_open) synthesizer_note_off(drums->hihat_open);
}
