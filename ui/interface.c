#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "interface.h"
#include "../audio/audio_engine.h"
#include "../audio/synthesizer.h"
#include "../instruments/piano.h"
#include "../instruments/drums.h"
#include "../instruments/guitar.h"
#include "../utils/wave.h"

// Structure pour stocker les instruments
typedef struct {
    Piano *piano;
    Drums *drums;
    Guitar *guitar;
} Instruments;

// Afficher le menu principal
static void print_menu(void) {
    printf("\n=== Menu Principal ===\n");
    printf("1. Jouer des notes au piano\n");
    printf("2. Jouer la batterie\n");
    printf("3. Jouer de la guitare\n");
    printf("4. Afficher l'état du mixeur\n");
    printf("5. Exporter en WAV\n");
    printf("6. Quitter\n");
    printf("Choix: ");
}

// Menu piano
static void piano_menu(Piano *piano, AudioEngine *engine) {
    printf("\n=== Piano ===\n");
    printf("Appuyez sur les touches pour jouer des notes\n");
    printf("Touches disponibles: A(C), Z(D), E(E), R(F), T(G), Y(A), U(B)\n");
    printf("Touches hautes: W(+1), X(-1)\n");
    printf("Q pour quitter\n\n");
    
    int octave = 4; // Octave de départ
    float base_notes[] = {262.0f, 294.0f, 330.0f, 349.0f, 392.0f, 440.0f, 494.0f}; // Do à Si
    
    while (1) {
        char input = getchar();
        input = tolower(input);
        
        if (input == 'q') {
            break;
        } else if (input == 'w') {
            octave++;
            printf("Octave: %d\n", octave);
        } else if (input == 'x') {
            octave--;
            printf("Octave: %d\n", octave);
        } else if (input == 'a') {
            piano_note_on(piano, base_notes[0] * (1 << (octave - 4)), 0.8f);
            printf("♪ Do\n");
        } else if (input == 'z') {
            piano_note_on(piano, base_notes[1] * (1 << (octave - 4)), 0.8f);
            printf("♪ Ré\n");
        } else if (input == 'e') {
            piano_note_on(piano, base_notes[2] * (1 << (octave - 4)), 0.8f);
            printf("♪ Mi\n");
        } else if (input == 'r') {
            piano_note_on(piano, base_notes[3] * (1 << (octave - 4)), 0.8f);
            printf("♪ Fa\n");
        } else if (input == 't') {
            piano_note_on(piano, base_notes[4] * (1 << (octave - 4)), 0.8f);
            printf("♪ Sol\n");
        } else if (input == 'y') {
            piano_note_on(piano, base_notes[5] * (1 << (octave - 4)), 0.8f);
            printf("♪ La\n");
        } else if (input == 'u') {
            piano_note_on(piano, base_notes[6] * (1 << (octave - 4)), 0.8f);
            printf("♪ Si\n");
        }
        
        // Traiter le moteur audio
        audio_engine_process(engine);
        usleep(100000); // Délai pour éviter les entrées trop rapides
    }
}

// Menu batterie
static void drums_menu(Drums *drums, AudioEngine *engine) {
    printf("\n=== Batterie ===\n");
    printf("K - Kick (Grosse caisse)\n");
    printf("S - Snare (Caisse claire)\n");
    printf("H - Hi-hat fermé\n");
    printf("O - Hi-hat ouvert\n");
    printf("Q pour quitter\n\n");
    
    while (1) {
        char input = getchar();
        input = tolower(input);
        
        if (input == 'q') {
            break;
        } else if (input == 'k') {
            drums_kick(drums, 0.8f);
            printf("💣 Kick!\n");
        } else if (input == 's') {
            drums_snare(drums, 0.7f);
            printf("📢 Snare!\n");
        } else if (input == 'h') {
            drums_hihat_closed(drums, 0.5f);
            printf("🥁 Hi-hat fermé\n");
        } else if (input == 'o') {
            drums_hihat_open(drums, 0.6f);
            printf("🥁 Hi-hat ouvert\n");
        }
        
        // Traiter le moteur audio
        audio_engine_process(engine);
        usleep(50000);
    }
}

// Menu guitare
static void guitar_menu(Guitar *guitar, AudioEngine *engine) {
    printf("\n=== Guitare ===\n");
    printf("1-6 - Jouer sur la corde 1-6\n");
    printf("W - Augmenter la fréquence\n");
    printf("S - Diminuer la fréquence\n");
    printf("Q pour quitter\n\n");
    
    float frequency = 220.0f; // La par défaut
    
    while (1) {
        char input = getchar();
        input = tolower(input);
        
        if (input == 'q') {
            break;
        } else if (input >= '1' && input <= '6') {
            int string = input - '1';
            guitar_pluck_string(guitar, string, frequency, 0.7f);
            printf("🎸 Corde %d jouée à %.1f Hz\n", string + 1, frequency);
        } else if (input == 'w') {
            frequency *= 1.05f;
            printf("↑ Fréquence: %.1f Hz\n", frequency);
        } else if (input == 's') {
            frequency /= 1.05f;
            printf("↓ Fréquence: %.1f Hz\n", frequency);
        }
        
        // Traiter le moteur audio
        audio_engine_process(engine);
        usleep(100000);
    }
}

// Lancer l'interface utilisateur
void ui_run(struct AudioEngine *engine) {
    if (!engine) return;
    
    // Créer les instruments
    Instruments instr;
    instr.piano = piano_create(audio_engine_get_sample_rate(engine));
    instr.drums = drums_create(audio_engine_get_sample_rate(engine));
    instr.guitar = guitar_create(audio_engine_get_sample_rate(engine));
    
    if (!instr.piano || !instr.drums || !instr.guitar) {
        fprintf(stderr, "[ERREUR] Impossible de créer les instruments\n");
        return;
    }
    
    printf("=== Application Musicale Multi-Instruments ===\n");
    printf("Instruments chargés: Piano, Batterie, Guitare\n");
    
    while (1) {
        print_menu();
        
        char choice = getchar();
        
        switch (choice) {
            case '1':
                piano_menu(instr.piano, engine);
                break;
            case '2':
                drums_menu(instr.drums, engine);
                break;
            case '3':
                guitar_menu(instr.guitar, engine);
                break;
            case '4':
                mixer_print_status(audio_engine_get_mixer(engine));
                break;
            case '5':
                printf("Export WAV: fonctionnalité non implémentée dans l'interface\n");
                break;
            case '6':
                printf("\nAu revoir!\n");
                goto cleanup;
            default:
                printf("Choix invalide\n");
        }
    }
    
cleanup:
    // Nettoyer
    if (instr.piano) piano_destroy(instr.piano);
    if (instr.drums) drums_destroy(instr.drums);
    if (instr.guitar) guitar_destroy(instr.guitar);
}
