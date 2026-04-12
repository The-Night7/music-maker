# Architecture Technique - Application Musicale Multi-Instruments

## Vue d'Ensemble de l'Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                 Interface Utilisateur (UI)                  │
│                  Terminal Interactive Menu                  │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
        ┌──────────────────────────────────┐
        │   Instruments Virtuels           │
        ├──────────────────────────────────┤
        │ ├─ Piano (8 voix polyphoniques)  │
        │ ├─ Drums (4 sons percussion)    │
        │ └─ Guitar (6 cordes)            │
        └──────────────────┬───────────────┘
                           │
                           ▼
        ┌──────────────────────────────────┐
        │   Moteur Audio (Audio Engine)    │
        └──────────────────┬───────────────┘
                           │
        ┌──────────────────┴──────────────────┐
        │                                     │
        ▼                                     ▼
    ┌─────────────┐                  ┌──────────────┐
    │ Synthétiseur│                  │    Mixeur    │
    │  (ADSR)     │                  │  Multi-piste │
    └─────────────┘                  └──────────────┘
        │                                     │
        └──────────────────┬──────────────────┘
                           │
                           ▼
        ┌──────────────────────────────────┐
        │    Gestion des Fichiers WAV      │
        │   (Lecture/Écriture Audio)       │
        └──────────────────────────────────┘
```

## Structure des Fichiers

### Modules Principaux

#### 1. Module Audio (`audio/`)

**audio_engine.c/h** - Moteur Audio Principal
- Initialisation et destruction du moteur
- Gestion du buffer audio
- Fréquence d'échantillonnage: 44100 Hz
- Taille buffer: 1024 samples
- Nombre de canaux: 2 (stéréo)

```c
typedef struct {
    int sample_rate;        // 44100 Hz
    int channels;           // 2 (stéréo)
    int buffer_size;        // 1024 samples
    float *audio_buffer;    // Buffer audio principal
    int is_running;         // État d'exécution
    void *mixer;           // Pointeur vers le mixeur
} AudioEngine;
```

**synthesizer.c/h** - Synthétiseur avec Enveloppe ADSR
- Génération d'ondes (Sine, Square, Triangle, Sawtooth)
- Enveloppe ADSR complète
- Contrôle de fréquence et amplitude
- Gestion de l'état de la note (on/off)

```c
typedef struct Synthesizer {
    int sample_rate;
    SynthType type;
    float phase;            // Position dans l'onde
    float frequency;        // Fréquence en Hz
    float amplitude;        // Amplitude 0.0-1.0
    
    // Enveloppe ADSR
    float attack;           // Temps d'attaque
    float decay;            // Temps de décroissance
    float sustain;          // Niveau de sustain
    float release;          // Temps de relâchement
    float envelope_time;    // Temps actuel de l'enveloppe
    
    int is_playing;         // État de la note
} Synthesizer;
```

**mixer.c/h** - Mixeur Multi-Pistes
- 16 pistes audio simultanées
- Contrôle volume par piste
- Panoramique stéréo par piste
- Fonction mute et solo
- Anti-clipping (soft clipping)

```c
typedef struct {
    float volume;           // 0.0 - 1.0
    float pan;              // -1.0 (gauche) à +1.0 (droite)
    int muted;              // 1 = muette
    int solo;               // 1 = solo actif
    void *instrument;       // Pointeur vers l'instrument
    float *buffer;          // Buffer de la piste
} MixerTrack;

typedef struct {
    int num_tracks;         // Nombre de pistes
    int buffer_size;        // Taille du buffer
    int sample_rate;        // Fréquence d'échantillonnage
    float master_volume;    // Volume maître
    MixerTrack *tracks;     // Array de pistes
    float *output_buffer;   // Buffer de sortie
} Mixer;
```

#### 2. Module Instruments (`instruments/`)

**piano.c/h** - Piano Virtuel
- 8 voix polyphoniques
- Synthèse onde sinusoïdale
- Pédale de sustain
- Round-robin voice allocation

```c
typedef struct {
    int sample_rate;
    int num_voices;         // 8 voix
    Synthesizer **voices;   // Array de synthétiseurs
    int current_voice;      // Prochaine voix (round-robin)
    int sustain_pedal;      // État pédale sustain
} Piano;
```

**drums.c/h** - Batterie Virtuelle
- 4 sons: Kick, Snare, Hi-hat fermé, Hi-hat ouvert
- Paramètres ADSR optimisés par son
- Synthèse avec différentes formes d'onde

```c
typedef struct {
    int sample_rate;
    
    Synthesizer *kick;      // Onde sine ~60 Hz
    Synthesizer *snare;     // Onde square ~200 Hz
    Synthesizer *hihat_closed;   // Onde sawtooth ~8kHz
    Synthesizer *hihat_open;     // Onde triangle ~9kHz
    
    float hihat_decay;      // Facteur de décroissance
    float hihat_open_time;  // Temps hi-hat ouvert
} Drums;
```

**guitar.c/h** - Guitare Virtuelle
- 6 cordes avec accordage standard
- Synthèse onde triangulaire
- Support du vibrato
- Accordage personnalisable

```c
typedef struct {
    int sample_rate;
    int num_strings;        // 6 cordes
    Synthesizer **strings;  // Array de synthétiseurs
    int current_string;     // Prochaine corde (round-robin)
    
    float vibrato_depth;    // Profondeur du vibrato
    float vibrato_rate;     // Vitesse du vibrato (Hz)
    float vibrato_time;     // Temps du vibrato
} Guitar;
```

#### 3. Module Interface (`ui/`)

**interface.c/h** - Interface Utilisateur Interactive
- Menu principal avec 6 options
- Sous-menus par instrument
- Capture d'entrée clavier
- Boucle interactive

```c
void ui_run(struct AudioEngine *engine);
```

Fonctions internes:
- `piano_menu()` - Menu du piano
- `drums_menu()` - Menu de la batterie
- `guitar_menu()` - Menu de la guitare
- `print_menu()` - Affichage du menu principal

#### 4. Module Utilitaires (`utils/`)

**wave.c/h** - Gestion des Fichiers WAV
- Écriture fichiers WAV PCM
- Support 16 bits et 24 bits
- Lecture fichiers WAV existants
- Conversion float → int avec clipping

```c
typedef struct {
    FILE *file;
    int sample_rate;
    int num_channels;
    int bits_per_sample;
    int num_samples;
    long data_position;
} WaveFile;

typedef struct {
    FILE *file;
    int sample_rate;
    int num_channels;
    int bits_per_sample;
    int num_samples;
    float *audio_data;
} WaveFileReader;
```

#### 5. Point d'Entrée

**main.c** - Application Principale
- Initialise le moteur audio
- Lance l'interface utilisateur
- Gère le cycle de vie de l'application

## Flux de Traitement du Signal Audio

### Chemin du Signal (Signal Flow)

```
User Input (Clavier)
        ↓
UI (interface.c)
        ↓
Instruments (piano/drums/guitar)
        ├─ synthesizer_note_on()
        └─ synthesizer_generate_sample()
        ↓
Mixeur (mixer.c)
        ├─ Mélange les pistes
        ├─ Applique volume & pan
        ├─ Gère mute/solo
        └─ Anti-clipping
        ↓
Moteur Audio (audio_engine.c)
        ├─ Buffer audio
        └─ Gestion fréquence
        ↓
Export WAV ou Lecture
```

### Cycle de Traitement Audio

1. **Capture d'Entrée** (UI)
   - Détecte la touche appuyée
   - Appelle la fonction appropriée (note_on, note_off)

2. **Génération du Signal** (Instruments → Synthesizer)
   ```c
   for (chaque échantillon) {
       sample = synthesizer_generate_sample();  // Génère l'onde
       sample = apply_adsr_envelope(sample);   // Applique ADSR
       return sample * amplitude;              // Applique volume
   }
   ```

3. **Mixage** (Mixer)
   ```c
   for (chaque piste) {
       if (!muted && (all_solo || solo)) {
           output += piste[i] * volume[i];
           output = apply_pan(output);
       }
   }
   output = output * master_volume;
   output = soft_clipping(output);  // Anti-clipping
   ```

4. **Traitement du Moteur Audio** (AudioEngine)
   - Copie buffer du mixeur vers buffer engine
   - Disponible pour export ou lecture temps réel

## Paramétrisation de l'Audio

### Fréquences d'Échantillonnage
```
Sample Rate: 44100 Hz
Buffer Size: 1024 samples
Buffer Duration: 1024 / 44100 ≈ 23.2 ms
Latence: ~23 ms
```

### Amplitude des Signaux
```
Format Interne: Float 32-bit
Plage: -1.0 (minimum) à +1.0 (maximum)
Export WAV: Converti à int16 (-32768 à 32767)
```

### Enveloppes ADSR par Défaut

| Instrument | Attack | Decay | Sustain | Release |
|-----------|--------|-------|---------|---------|
| Piano     | 5ms    | 200ms | 70%     | 300ms   |
| Batterie  | 1ms    | 150ms | 0%      | 50ms    |
| Guitare   | 10ms   | 500ms | 30%     | 400ms   |

### Fréquences Prédéfinies

**Batterie:**
- Kick: 60 Hz (grave)
- Snare: 200 Hz (mid)
- Hi-hat: 8000-9000 Hz (aigu)

**Guitare (Accordage Standard):**
- Corde 1 (E4): 329.63 Hz
- Corde 2 (B3): 246.94 Hz
- Corde 3 (G3): 196.00 Hz
- Corde 4 (D3): 146.83 Hz
- Corde 5 (A2): 110.00 Hz
- Corde 6 (E2): 82.41 Hz

## Optimisations et Performance

### Mécanismes de Performance

1. **Allocation Mémoire Pré-Allouée**
   - Buffers pré-alloués au démarrage
   - Évite les allocations dynamiques en temps réel
   - Réduit la latence

2. **Round-Robin Voice Allocation**
   - Piano: distribue les notes sur 8 voix
   - Guitare: distribue sur 6 cordes
   - Équilibre la charge CPU

3. **Soft Clipping Anti-Distorsion**
   ```c
   if (sample > 1.0f) sample = 1.0f;
   if (sample < -1.0f) sample = -1.0f;
   ```

4. **Compilation Optimisée**
   - Flag `-O2` par défaut
   - Flag `-O3` pour plus d'optimisations

### Profiling CPU

La charge CPU dépend de:
- Nombre de voix actives
- Nombre de pistes mixées
- Complexité de l'enveloppe ADSR
- Type d'onde (sine < triangle < square < sawtooth)

## Dépendances

### Dépendances Externes
- **libm (math.h)**: Fonctions mathématiques (sin, cos, exp, pow)
- **stdio.h**: I/O fichiers et console
- **stdlib.h**: Allocation mémoire
- **string.h**: Manipulation de chaînes
- **unistd.h**: I/O POSIX (usleep)
- **pthread.h**: (inclus mais non utilisé actuellement)
- **ctype.h**: Classification des caractères

### Liens à la Compilation
```bash
gcc -lm  # Lien la librairie math
```

## Étapes de Compilation

```
main.c + audio/*.c + instruments/*.c + ui/*.c + utils/*.c
    ↓
Pré-processage C (includes, macros)
    ↓
Compilation → Fichiers objets (.o)
    ↓
Édition de liens (-lm pour libmath)
    ↓
Exécutable: bin/music_app
```

## Gestion Mémoire

### Allocation au Démarrage
```
AudioEngine:                    ~100 bytes
Audio Buffer (1024 * 4 bytes):  ~4 KB
Mixer (16 pistes, 1024*4):      ~65 KB
Piano (8 voix):                 ~400 bytes
Drums (4 voix):                 ~200 bytes
Guitar (6 cordes):              ~300 bytes
```

**Total estimé: ~70 KB** (très léger)

### Libération à l'Arrêt
- Tous les buffers libérés
- Synthétiseurs détruits
- Fichiers fermés
- Pas de fuite mémoire détectée

## Extensions Futures Possibles

### Court Terme
1. Enregistrement de séquences musicales
2. Effets audio (reverb, delay, EQ)
3. Support MIDI pour contrôleurs externes
4. Plus d'instruments (violon, flûte, synthé)

### Moyen Terme
1. Interface graphique (SDL2, GTK)
2. Streaming audio temps réel (PortAudio, ALSA)
3. Séquenceur 16 pistes
4. Enveloppes ADSR éditables en temps réel

### Long Terme
1. Synthèse FM avancée
2. Synthèse granulaire
3. Algorithmes de modelage physique
4. Apprentissage machine pour la synthèse

## Documentation Associée

- **README.md**: Guide complet du projet
- **GUIDE_UTILISATION.md**: Guide d'utilisation de l'application
- **Makefile**: Instructions de compilation
- **Code source**: Commentaires détaillés dans chaque fichier

---

Pour toute question sur l'architecture, consultez directement le code source avec les commentaires détaillés.
