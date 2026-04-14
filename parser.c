#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "parser.h"

#define MAX_LINE_LENGTH 256
#define MAX_INSTRUMENTS 3
#define MAX_NOTES_PER_INSTRUMENT 1024

// Notes MIDI et leurs fréquences
static const float NOTE_FREQUENCIES[] = {
    16.35f,   // C0
    17.32f,   // C#0
    18.35f,   // D0
    19.45f,   // D#0
    20.60f,   // E0
    21.83f,   // F0
    23.12f,   // F#0
    24.50f,   // G0
    25.96f,   // G#0
    27.50f,   // A0
    29.14f,   // A#0
    30.87f    // B0
};

// Trim whitespace from string
static void trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) {
        *str = 0;
        return;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
}

// Check if line is a comment
static int is_comment(const char *line) {
    if (line[0] == '#' || (line[0] == '/' && line[1] == '/')) {
        return 1;
    }
    return 0;
}

// Parse note string to frequency
float parser_note_to_frequency(const char *note) {
    if (strcmp(note, "pause") == 0 || strcmp(note, "rest") == 0) {
        return 0.0f;
    }
    
    char note_copy[8];
    strncpy(note_copy, note, sizeof(note_copy) - 1);
    note_copy[sizeof(note_copy) - 1] = 0;
    
    // Extract note name (C, D, E, F, G, A, B)
    char note_name = toupper(note_copy[0]);
    int note_index = -1;
    
    switch (note_name) {
        case 'C': note_index = 0; break;
        case 'D': note_index = 2; break;
        case 'E': note_index = 4; break;
        case 'F': note_index = 5; break;
        case 'G': note_index = 7; break;
        case 'A': note_index = 9; break;
        case 'B': note_index = 11; break;
        default: return 440.0f;
    }
    
    // Check for sharps/flats
    if (note_copy[1] == '#') {
        note_index++;
    } else if (note_copy[1] == 'b') {
        note_index--;
    }
    
    // Normalize to 0-11 range
    note_index = note_index % 12;
    if (note_index < 0) note_index += 12;
    
    // Extract octave
    char *octave_str = strchr(note_copy, '#') ? strchr(note_copy, '#') + 1 : strchr(note_copy, 'b') ? strchr(note_copy, 'b') + 1 : note_copy + 1;
    int octave = atoi(octave_str);
    if (octave < 0) octave = 4;
    if (octave > 8) octave = 8;
    
    // Calculate frequency (MIDI note 60 = C4 = 261.63 Hz)
    float base_freq = NOTE_FREQUENCIES[note_index];
    return base_freq * powf(2.0f, octave);
}

// Convert beat duration to time in seconds
float parser_beat_to_duration(float beat, int tempo) {
    // beat in quarter notes, tempo in BPM
    if (tempo <= 0) tempo = 120;
    float beat_duration = (60.0f / tempo);  // Duration of one beat in seconds
    return beat * beat_duration;
}

// Get instrument type from name
InstrumentType parser_get_instrument_type(const char *name) {
    if (!name) return INSTR_UNKNOWN;
    
    char upper_name[32];
    strncpy(upper_name, name, sizeof(upper_name) - 1);
    upper_name[sizeof(upper_name) - 1] = 0;
    
    for (int i = 0; upper_name[i]; i++) {
        upper_name[i] = toupper((unsigned char)upper_name[i]);
    }
    
    if (strcmp(upper_name, "PIANO") == 0 || strcmp(upper_name, "P") == 0) {
        return INSTR_PIANO;
    } else if (strcmp(upper_name, "DRUMS") == 0 || strcmp(upper_name, "DRUM") == 0 || 
               strcmp(upper_name, "PERCUSSION") == 0 || strcmp(upper_name, "D") == 0) {
        return INSTR_DRUMS;
    } else if (strcmp(upper_name, "GUITAR") == 0 || strcmp(upper_name, "G") == 0) {
        return INSTR_GUITAR;
    }
    
    return INSTR_UNKNOWN;
}

// Get instrument type name
const char *parser_instrument_type_name(InstrumentType type) {
    switch (type) {
        case INSTR_PIANO: return "Piano";
        case INSTR_DRUMS: return "Drums";
        case INSTR_GUITAR: return "Guitar";
        default: return "Unknown";
    }
}

// Load and parse music file
Composition *parser_load_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "[ERREUR] Impossible d'ouvrir le fichier: %s\n", filename);
        return NULL;
    }
    
    Composition *comp = (Composition *)malloc(sizeof(Composition));
    if (!comp) {
        fclose(file);
        return NULL;
    }
    
    // Initialize defaults
    strcpy(comp->titre, "Untitled");
    strcpy(comp->auteur, "Anonymous");
    comp->tempo = 120;
    strcpy(comp->signature_temps, "4/4");
    comp->longueur = 0;
    
    comp->instruments = (Instrument *)malloc(sizeof(Instrument) * MAX_INSTRUMENTS);
    if (!comp->instruments) {
        free(comp);
        fclose(file);
        return NULL;
    }
    
    comp->num_instruments = 0;
    
    char line[MAX_LINE_LENGTH];
    int current_instrument = -1;
    int current_note_count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        trim(line);
        
        // Skip empty lines and comments
        if (strlen(line) == 0 || is_comment(line)) {
            continue;
        }
        
        // Parse [METADATA] section
        if (strcmp(line, "[METADATA]") == 0) {
            while (fgets(line, sizeof(line), file)) {
                trim(line);
                if (strlen(line) == 0 || is_comment(line)) continue;
                if (line[0] == '[') {
                    // Put line back (can't really, so we'll handle it)
                    break;
                }
                
                char key[64], value[256];
                if (sscanf(line, "%63[^:]: %255[^\n]", key, value) == 2) {
                    trim(key);
                    trim(value);
                    
                    if (strcmp(key, "titre") == 0) {
                        strncpy(comp->titre, value, sizeof(comp->titre) - 1);
                    } else if (strcmp(key, "auteur") == 0) {
                        strncpy(comp->auteur, value, sizeof(comp->auteur) - 1);
                    } else if (strcmp(key, "tempo") == 0) {
                        comp->tempo = atoi(value);
                    } else if (strcmp(key, "signature_temps") == 0) {
                        strncpy(comp->signature_temps, value, sizeof(comp->signature_temps) - 1);
                    } else if (strcmp(key, "longueur") == 0) {
                        comp->longueur = atoi(value);
                    }
                }
            }
            continue;
        }
        
        // Parse [INSTRUMENT: NAME] section
        if (strncmp(line, "[INSTRUMENT:", 12) == 0) {
            if (comp->num_instruments >= MAX_INSTRUMENTS) {
                fprintf(stderr, "[AVERTISSEMENT] Trop d'instruments (max: %d)\n", MAX_INSTRUMENTS);
                continue;
            }
            
            current_instrument = comp->num_instruments;
            comp->num_instruments++;
            
            char instr_name[64];
            if (sscanf(line, "[INSTRUMENT: %63[^]]", instr_name) == 1) {
                trim(instr_name);
                comp->instruments[current_instrument].type = parser_get_instrument_type(instr_name);
            }
            
            // Set defaults
            comp->instruments[current_instrument].volume = 0.8f;
            comp->instruments[current_instrument].pan = 0.0f;
            comp->instruments[current_instrument].adsr_attack = 0.01f;
            comp->instruments[current_instrument].adsr_decay = 0.1f;
            comp->instruments[current_instrument].adsr_sustain = 0.7f;
            comp->instruments[current_instrument].adsr_release = 0.2f;
            
            comp->instruments[current_instrument].notes = 
                (MusicalNote *)malloc(sizeof(MusicalNote) * MAX_NOTES_PER_INSTRUMENT);
            comp->instruments[current_instrument].num_notes = 0;
            current_note_count = 0;
            
            // Parse instrument parameters
            while (fgets(line, sizeof(line), file)) {
                trim(line);
                if (strlen(line) == 0 || is_comment(line)) continue;
                if (line[0] == '[') break;
                
                char key[64], value[256];
                if (sscanf(line, "%63[^:]: %255[^\n]", key, value) == 2) {
                    trim(key);
                    trim(value);
                    
                    if (strcmp(key, "volume") == 0) {
                        comp->instruments[current_instrument].volume = atof(value);
                    } else if (strcmp(key, "pan") == 0) {
                        comp->instruments[current_instrument].pan = atof(value);
                    }
                }
            }
            continue;
        }
        
        // Parse [NOTES] section
        if (strcmp(line, "[NOTES]") == 0) {
            while (fgets(line, sizeof(line), file)) {
                trim(line);
                if (strlen(line) == 0 || is_comment(line)) continue;
                if (line[0] == '[') break;
                
                if (current_instrument < 0 || current_instrument >= comp->num_instruments) {
                    continue;
                }
                
                if (current_note_count >= MAX_NOTES_PER_INSTRUMENT) {
                    fprintf(stderr, "[AVERTISSEMENT] Trop de notes pour l'instrument\n");
                    break;
                }
                
                MusicalNote *note = &comp->instruments[current_instrument].notes[current_note_count];
                float velocity = 1.0f;
                
                if (sscanf(line, "%7s %f %f", note->note, &note->duration, &velocity) >= 2) {
                    note->velocity = velocity;
                    comp->instruments[current_instrument].num_notes++;
                    current_note_count++;
                }
            }
            continue;
        }
    }
    
    fclose(file);
    return comp;
}

// Validate composition
int parser_validate(Composition *comp) {
    if (!comp) return 0;
    
    if (comp->tempo < 40 || comp->tempo > 300) {
        fprintf(stderr, "[AVERTISSEMENT] Tempo invalide: %d (doit être 40-300)\n", comp->tempo);
        comp->tempo = 120;
    }
    
    if (comp->num_instruments == 0) {
        fprintf(stderr, "[ERREUR] Aucun instrument défini\n");
        return 0;
    }
    
    for (int i = 0; i < comp->num_instruments; i++) {
        if (comp->instruments[i].num_notes == 0) {
            fprintf(stderr, "[AVERTISSEMENT] Instrument %d n'a pas de notes\n", i);
        }
    }
    
    return 1;
}

// Print composition info
void parser_print_info(Composition *comp) {
    if (!comp) return;
    
    printf("\n=== Composition Musicale ===\n");
    printf("Titre: %s\n", comp->titre);
    printf("Auteur: %s\n", comp->auteur);
    printf("Tempo: %d BPM\n", comp->tempo);
    printf("Signature: %s\n", comp->signature_temps);
    printf("Nombre d'instruments: %d\n\n", comp->num_instruments);
    
    for (int i = 0; i < comp->num_instruments; i++) {
        Instrument *instr = &comp->instruments[i];
        printf("[%d] %s\n", i + 1, parser_instrument_type_name(instr->type));
        printf("    Volume: %.0f%%\n", instr->volume * 100.0f);
        printf("    Pan: %+.1f\n", instr->pan);
        printf("    Nombre de notes: %d\n", instr->num_notes);
        printf("    ADSR: %.3fs / %.3fs / %.0f%% / %.3fs\n",
               instr->adsr_attack, instr->adsr_decay,
               instr->adsr_sustain * 100.0f, instr->adsr_release);
    }
}

// Free composition
void parser_free_composition(Composition *comp) {
    if (!comp) return;
    
    if (comp->instruments) {
        for (int i = 0; i < comp->num_instruments; i++) {
            if (comp->instruments[i].notes) {
                free(comp->instruments[i].notes);
            }
        }
        free(comp->instruments);
    }
    
    free(comp);
}
