#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "synthesizer.h"

#define PI 3.14159265359

// Créer un synthétiseur
Synthesizer *synthesizer_create(int sample_rate, SynthType type) {
    Synthesizer *synth = (Synthesizer *)malloc(sizeof(Synthesizer));
    if (!synth) return NULL;
    
    synth->sample_rate = sample_rate;
    synth->type = type;
    synth->phase = 0.0f;
    synth->frequency = 440.0f; // Fréquence par défaut (LA4)
    synth->amplitude = 0.5f;
    synth->attack = 0.01f;    // 10ms
    synth->decay = 0.1f;      // 100ms
    synth->sustain = 0.7f;    // 70% du volume
    synth->release = 0.2f;    // 200ms
    synth->envelope_time = 0.0f;
    synth->is_playing = 0;
    
    return synth;
}

// Détruire un synthétiseur
void synthesizer_destroy(Synthesizer *synth) {
    if (synth) free(synth);
}

// Générer une onde sinusoïdale
static float generate_sine_wave(Synthesizer *synth) {
    float sample = sinf(2.0f * PI * synth->phase) * synth->amplitude;
    synth->phase += synth->frequency / synth->sample_rate;
    
    // Réinitialiser la phase si elle dépasse 1.0
    if (synth->phase >= 1.0f) {
        synth->phase -= 1.0f;
    }
    
    return sample;
}

// Générer une onde carrée
static float generate_square_wave(Synthesizer *synth) {
    float sample = (synth->phase < 0.5f) ? synth->amplitude : -synth->amplitude;
    synth->phase += synth->frequency / synth->sample_rate;
    
    if (synth->phase >= 1.0f) {
        synth->phase -= 1.0f;
    }
    
    return sample;
}

// Générer une onde triangulaire
static float generate_triangle_wave(Synthesizer *synth) {
    float sample;
    if (synth->phase < 0.25f) {
        sample = synth->amplitude * (synth->phase * 4.0f);
    } else if (synth->phase < 0.75f) {
        sample = synth->amplitude * (2.0f - synth->phase * 4.0f);
    } else {
        sample = synth->amplitude * (synth->phase * 4.0f - 4.0f);
    }
    
    synth->phase += synth->frequency / synth->sample_rate;
    
    if (synth->phase >= 1.0f) {
        synth->phase -= 1.0f;
    }
    
    return sample;
}

// Générer une onde en dents de scie
static float generate_sawtooth_wave(Synthesizer *synth) {
    float sample = synth->amplitude * (2.0f * synth->phase - 1.0f);
    synth->phase += synth->frequency / synth->sample_rate;
    
    if (synth->phase >= 1.0f) {
        synth->phase -= 1.0f;
    }
    
    return sample;
}

// Appliquer l'enveloppe ADSR
static float apply_adsr_envelope(Synthesizer *synth, float sample) {
    float envelope = 1.0f;
    float total_time = synth->attack + synth->decay + synth->release;
    
    if (synth->envelope_time < synth->attack) {
        // Phase Attack
        envelope = synth->envelope_time / synth->attack;
    } else if (synth->envelope_time < synth->attack + synth->decay) {
        // Phase Decay
        float decay_time = synth->envelope_time - synth->attack;
        envelope = 1.0f - (decay_time / synth->decay) * (1.0f - synth->sustain);
    } else if (synth->is_playing) {
        // Phase Sustain
        envelope = synth->sustain;
    } else {
        // Phase Release
        float release_time = synth->envelope_time - (synth->attack + synth->decay);
        if (release_time < synth->release) {
            envelope = synth->sustain * (1.0f - release_time / synth->release);
        } else {
            envelope = 0.0f;
        }
    }
    
    return sample * envelope;
}

// Générer le prochain échantillon
float synthesizer_generate_sample(Synthesizer *synth) {
    if (!synth->is_playing && synth->envelope_time >= (synth->attack + synth->decay + synth->release)) {
        return 0.0f;
    }
    
    float sample = 0.0f;
    
    // Générer l'onde selon le type
    switch (synth->type) {
        case SINE_WAVE:
            sample = generate_sine_wave(synth);
            break;
        case SQUARE_WAVE:
            sample = generate_square_wave(synth);
            break;
        case TRIANGLE_WAVE:
            sample = generate_triangle_wave(synth);
            break;
        case SAWTOOTH_WAVE:
            sample = generate_sawtooth_wave(synth);
            break;
        default:
            sample = generate_sine_wave(synth);
    }
    
    // Appliquer l'enveloppe ADSR
    sample = apply_adsr_envelope(synth, sample);
    
    // Incrémenter le temps de l'enveloppe
    synth->envelope_time += 1.0f / synth->sample_rate;
    
    return sample;
}

// Définir la fréquence
void synthesizer_set_frequency(Synthesizer *synth, float frequency) {
    synth->frequency = frequency;
}

// Définir l'amplitude
void synthesizer_set_amplitude(Synthesizer *synth, float amplitude) {
    if (amplitude > 1.0f) amplitude = 1.0f;
    if (amplitude < 0.0f) amplitude = 0.0f;
    synth->amplitude = amplitude;
}

// Démarrer la lecture
void synthesizer_note_on(Synthesizer *synth, float frequency) {
    synth->frequency = frequency;
    synth->is_playing = 1;
    synth->envelope_time = 0.0f;
}

// Arrêter la lecture
void synthesizer_note_off(Synthesizer *synth) {
    synth->is_playing = 0;
}

// Vérifier si le synthétiseur joue
int synthesizer_is_playing(Synthesizer *synth) {
    return synth->is_playing || 
           (synth->envelope_time < synth->attack + synth->decay + synth->release);
}
