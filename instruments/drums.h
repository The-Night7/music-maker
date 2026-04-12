#ifndef DRUMS_H
#define DRUMS_H

#include <stdint.h>
#include "../audio/synthesizer.h"

// Structure de la batterie
typedef struct {
    int sample_rate;
    
    // Différents sons de batterie
    Synthesizer *kick;          // Grosse caisse
    Synthesizer *snare;         // Caisse claire
    Synthesizer *hihat_closed;  // Hi-hat fermé
    Synthesizer *hihat_open;    // Hi-hat ouvert
    
    float hihat_decay;          // Facteur de decay pour le hi-hat ouvert
    float hihat_open_time;      // Temps écoulé depuis l'activation du hi-hat ouvert
} Drums;

// Fonctions de la batterie
Drums *drums_create(int sample_rate);
void drums_destroy(Drums *drums);
float drums_generate_sample(Drums *drums);
void drums_kick(Drums *drums, float intensity);
void drums_snare(Drums *drums, float intensity);
void drums_hihat_closed(Drums *drums, float intensity);
void drums_hihat_open(Drums *drums, float intensity);
void drums_stop_all(Drums *drums);

#endif // DRUMS_H
