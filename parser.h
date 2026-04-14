#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

// Types d'instruments
typedef enum {
    INSTR_PIANO,
    INSTR_DRUMS,
    INSTR_GUITAR,
    INSTR_UNKNOWN
} InstrumentType;

// Structure pour une note
typedef struct {
    char note[8];           // "C4", "D#4", "pause", etc.
    float duration;         // Durée en temps (1.0 = noire)
    float velocity;         // Volume 0.0-1.0
} MusicalNote;

// Structure pour les paramètres d'instrument
typedef struct {
    InstrumentType type;
    float volume;
    float pan;
    float adsr_attack;
    float adsr_decay;
    float adsr_sustain;
    float adsr_release;
    
    MusicalNote *notes;     // Array de notes
    int num_notes;          // Nombre de notes
} Instrument;

// Structure principale pour une composition
typedef struct {
    char titre[256];
    char auteur[256];
    int tempo;              // BPM
    char signature_temps[16]; // ex: "4/4"
    int longueur;           // Nombre de mesures
    
    Instrument *instruments;
    int num_instruments;
} Composition;

// Fonctions de parsing
Composition *parser_load_file(const char *filename);
void parser_free_composition(Composition *comp);
int parser_validate(Composition *comp);
void parser_print_info(Composition *comp);

// Utilitaires
float parser_note_to_frequency(const char *note);
float parser_beat_to_duration(float beat, int tempo);
InstrumentType parser_get_instrument_type(const char *name);
const char *parser_instrument_type_name(InstrumentType type);

#endif // PARSER_H
