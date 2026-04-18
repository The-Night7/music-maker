#include <stdlib.h>
#include <math.h>
#include "piano.h"

// Créer un piano
Piano *piano_create(int sample_rate) {
    Piano *piano = (Piano *)malloc(sizeof(Piano));
    if (!piano) return NULL;
    
    piano->sample_rate = sample_rate;
    piano->num_voices = 8;
    piano->voices = (Synthesizer **)malloc(sizeof(Synthesizer *) * piano->num_voices);
    
    if (!piano->voices) {
        free(piano);
        return NULL;
    }
    
    // Créer les synthétiseurs pour chaque voix
    for (int i = 0; i < piano->num_voices; i++) {
        piano->voices[i] = synthesizer_create(sample_rate, SINE_WAVE);
    }
    
    piano->current_voice = 0;
    piano->sustain_pedal = 0;
    
    return piano;
}

// Détruire le piano
void piano_destroy(Piano *piano) {
    if (!piano) return;
    
    for (int i = 0; i < piano->num_voices; i++) {
        if (piano->voices[i]) {
            synthesizer_destroy(piano->voices[i]);
        }
    }
    
    free(piano->voices);
    free(piano);
}

// Générer un échantillon audio du piano
float piano_generate_sample(Piano *piano) {
    float output = 0.0f;
    
    // Mixer toutes les voix
    for (int i = 0; i < piano->num_voices; i++) {
        if (synthesizer_is_playing(piano->voices[i])) {
            output += synthesizer_generate_sample(piano->voices[i]);
        }
    }
    
    // Normaliser
    if (piano->num_voices > 0) {
        output /= (float)piano->num_voices;
    }
    
    return output;
}

// Appuyer sur une touche
void piano_note_on(Piano *piano, float frequency, float velocity) {
    // Trouver une voix libre
    int voice_index = piano->current_voice;
    
    // Utiliser la prochaine voix disponible
    piano->current_voice = (piano->current_voice + 1) % piano->num_voices;
    
    // Démarrer la note sur cette voix
    synthesizer_note_on(piano->voices[voice_index], frequency);
    synthesizer_set_amplitude(piano->voices[voice_index], velocity);
    
    // Adapter les paramètres ADSR pour le piano
    piano->voices[voice_index]->attack = 0.005f;   // 5ms (très court)
    piano->voices[voice_index]->decay = 0.2f;      // 200ms
    piano->voices[voice_index]->sustain = 0.7f;    // 70% (sustain court)
    piano->voices[voice_index]->release = 0.3f;    // 300ms (long decay)
}

// Relâcher une touche
void piano_note_off(Piano *piano, int voice_index) {
    if (voice_index >= 0 && voice_index < piano->num_voices) {
        synthesizer_note_off(piano->voices[voice_index]);
    }
}

// Activer la pédale de sustain
void piano_set_sustain_pedal(Piano *piano, int active) {
    piano->sustain_pedal = active;
}

// Obtenir le nombre de voix actives
int piano_get_active_voices(Piano *piano) {
    int count = 0;
    for (int i = 0; i < piano->num_voices; i++) {
        if (synthesizer_is_playing(piano->voices[i])) {
            count++;
        }
    }
    return count;
}
