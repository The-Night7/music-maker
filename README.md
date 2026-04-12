# Application Musicale Multi-Instruments en C

Une application complète en C pour la création musicale avec plusieurs instruments (piano, batterie, guitare) et un système de synthèse audio professionnel.

## Architecture

L'application est organisée de manière modulaire avec les composants suivants:

```
music_app/
├── main.c                    # Point d'entrée
├── audio/                    # Moteur audio
│   ├── audio_engine.c/h     # Moteur audio principal
│   ├── synthesizer.c/h      # Synthétiseur avec enveloppe ADSR
│   └── mixer.c/h            # Mixeur multi-pistes
├── instruments/              # Instruments virtuels
│   ├── piano.c/h            # Piano polyphonique
│   ├── drums.c/h            # Batterie (kick, snare, hi-hat)
│   └── guitar.c/h           # Guitare avec 6 cordes
├── ui/                       # Interface utilisateur
│   └── interface.c/h        # Interface interactive en terminal
└── utils/                    # Utilitaires
    └── wave.c/h             # Export/Import WAV
```

## Caractéristiques Principales

### Moteur Audio (Audio Engine)
- Fréquence d'échantillonnage: **44100 Hz** (qualité CD)
- Buffer size: **1024 samples**
- 2 canaux (stéréo)
- Thread-safe pour traitement audio temps réel

### Synthétiseur (Synthesizer)
- **4 types d'ondes** disponibles:
  - Onde sinusoïdale (Sine)
  - Onde carrée (Square)
  - Onde triangulaire (Triangle)
  - Onde en dents de scie (Sawtooth)

- **Enveloppe ADSR**:
  - Attack (Attaque)
  - Decay (Décroissance)
  - Sustain (Sustain)
  - Release (Relâchement)

### Mixeur Multi-Pistes
- **16 pistes audio** simultanées
- Contrôle du volume par piste (0.0 - 1.0)
- Panoramique stéréo (-1.0 gauche à +1.0 droite)
- Fonction mute (sourdine) par piste
- Fonction solo par piste
- Volume maître global
- Anti-clipping (soft clipping)

### Piano
- **8 voix polyphoniques** simultanées
- Synthèse sine wave pour un son pur
- Pédale de sustain
- Gamme chromatique complète

### Batterie
- **Grosse caisse (Kick)** - ~60 Hz
- **Caisse claire (Snare)** - ~200 Hz
- **Hi-hat fermé** - ~8000 Hz
- **Hi-hat ouvert** - ~9000 Hz avec decay
- Synthèse avec différents types d'ondes

### Guitare
- **6 cordes** avec accordage standard (E-A-D-G-B-E)
- Synthèse en onde triangulaire
- Support du vibrato
- Polyphonie jusqu'à 6 notes simultanées

### Interface Utilisateur
- Menu interactif en terminal
- Contrôle du clavier pour les instruments
- Affichage des états du mixeur
- Feedback visuel avec émojis

### Export WAV
- Support des fichiers WAV (PCM)
- 16 bits et 24 bits
- Fréquence d'échantillonnage configurable
- Nombre de canaux configurable
- Lecture et écriture de fichiers WAV

## Compilation

### Prérequis
- **gcc** ou **clang**
- **make**
- Bibliotèques standard C (math.h, stdio.h, etc.)

### Instructions

```bash
# Compiler le projet
make

# Compiler avec optimisation
make all

# Compiler avec symboles de débogage
make debug

# Lancer l'application
make run

# Nettoyer les fichiers compilés
make clean

# Afficher l'aide
make help
```

### Exécutable généré
- Chemin: `./bin/music_app`
- Taille: ~100-150 KB (selon les optimisations)

## Utilisation

### Lancer l'application
```bash
./bin/music_app
```

### Menu Principal
```
1. Jouer des notes au piano
2. Jouer la batterie
3. Jouer de la guitare
4. Afficher l'état du mixeur
5. Exporter en WAV
6. Quitter
```

### Contrôles du Piano
- **A-U**: Jouer les notes Do à Si
- **W**: Augmenter l'octave
- **X**: Diminuer l'octave
- **Q**: Quitter

### Contrôles de la Batterie
- **K**: Kick (Grosse caisse)
- **S**: Snare (Caisse claire)
- **H**: Hi-hat fermé
- **O**: Hi-hat ouvert
- **Q**: Quitter

### Contrôles de la Guitare
- **1-6**: Jouer sur la corde 1-6
- **W**: Augmenter la fréquence
- **S**: Diminuer la fréquence
- **Q**: Quitter

## Structure des Fichiers

### main.c
Point d'entrée de l'application. Initialise le moteur audio et lance l'interface utilisateur.

### audio/audio_engine.c/h
Moteur audio principal qui:
- Gère l'allocation mémoire du buffer audio
- Crée et gère le mixeur
- Fournit les fonctions de démarrage/arrêt

### audio/synthesizer.c/h
Synthétiseur audio qui:
- Génère les ondes selon le type spécifié
- Applique l'enveloppe ADSR
- Gère les paramètres de fréquence et amplitude

### audio/mixer.c/h
Mixeur multi-pistes qui:
- Mélange tous les signaux audio
- Applique volume et panoramique
- Gère le mute et solo
- Prévient le clipping

### instruments/piano.c/h
Piano virtuel qui:
- Crée 8 voix polyphoniques
- Gère les notes appuyées/relâchées
- Adapte les paramètres ADSR pour le piano

### instruments/drums.c/h
Batterie virtuelle avec:
- 4 synthétiseurs pour différents tambours
- Paramètres ADSR optimisés pour chaque son
- Fréquences prédéfinies

### instruments/guitar.c/h
Guitare virtuelle avec:
- 6 cordes avec accordage standard
- Support du vibrato
- Polyphonie à 6 voix

### ui/interface.c/h
Interface utilisateur interactive qui:
- Affiche les menus
- Capture les entrées clavier
- Gère les différents modes d'instruments

### utils/wave.c/h
Utilitaires WAV pour:
- Créer et écrire des fichiers WAV
- Lire des fichiers WAV existants
- Gérer les headers et les données audio

## Détails Techniques

### Fréquences Prédéfinies
- **Piano**: Gamme chromatique de Do0 (16 Hz) à Do8 (4186 Hz)
- **Batterie Kick**: 60 Hz (grave)
- **Batterie Snare**: 200 Hz (médium)
- **Batterie Hi-hat**: 8000-9000 Hz (aigu)
- **Guitare**: Accordage standard EADGBE

### Paramètres ADSR par Défaut

| Instrument | Attack | Decay | Sustain | Release |
|-----------|--------|-------|---------|---------|
| Piano     | 5ms    | 200ms | 70%     | 300ms   |
| Batterie  | 1ms    | 150ms | 0%      | 50ms    |
| Guitare   | 10ms   | 500ms | 30%     | 400ms   |

### Traitement Audio
- Format interne: **float 32-bit** (-1.0 à +1.0)
- Conversion: float → int16/24 avec clipping lors de l'export WAV
- Fréquence d'échantillonnage: **44100 Hz** (CD quality)
- Sample rate: 44100 samples/seconde = ~22ms par buffer

## Limitations et Améliorations Futures

### Limitations Actuelles
- Interface en terminal uniquement (pas de GUI graphique)
- Export WAV en temps différé uniquement (pas de streaming)
- Pas de traitement d'effets audio avancés (reverb, delay, EQ)
- Polyphonie limitée par instrument
- Pas de sauvegarde de séquences musicales

### Améliorations Possibles
1. **Interface Graphique**: GTK+ ou SDL2 pour une interface visuelle
2. **Effets Audio**: Reverb, Delay, Chorus, Distortion
3. **Séquenceur**: Enregistrement et lecture de séquences
4. **Plus d'Instruments**: Violon, Flûte, Synthé, Cordes
5. **Support MIDI**: Entrée MIDI pour les contrôleurs externes
6. **Streaming Audio**: Utiliser PortAudio ou ALSA pour l'audio temps réel
7. **Édition de Paramètres**: Interface pour modifier les enveloppes ADSR
8. **Présets**: Sauvegarde/Chargement de configurations d'instruments

## Compilation Détaillée

Le Makefile compile automatiquement tous les fichiers dans le bon ordre:

```
main.c
├── audio_engine.c/h
│   ├── mixer.c/h
│   └── synthesizer.c/h
├── piano.c/h → synthesizer.h
├── drums.c/h → synthesizer.h
├── guitar.c/h → synthesizer.h
├── interface.c/h → tous les autres
└── wave.c/h
```

## Dépannage

### Erreur de compilation
```bash
gcc: command not found
```
**Solution**: Installer GCC
```bash
sudo apt-get install build-essential
```

### L'application ne produit pas de son
- Vérifiez que le moteur audio est bien initié
- Vérifiez les paramètres du mixeur
- Consultez les messages d'erreur

### Problème de performance
- Augmentez la taille du buffer (buffer_size)
- Réduisez le nombre de pistes actives
- Compilez avec des optimisations: `gcc -O3`

## License
Ce projet est fourni à titre d'exemple éducatif.

## Auteur
Créé comme démonstration d'architecture logicielle modulaire en C pour la synthèse audio.

## Ressources Références

- **Wave File Format**: https://en.wikipedia.org/wiki/WAV
- **ADSR Envelope**: https://en.wikipedia.org/wiki/Synthesizer
- **Audio Processing**: https://en.wikipedia.org/wiki/Digital_audio
- **C Programming**: https://en.wikipedia.org/wiki/C_(programming_language)

---

Pour des questions ou des contributions, consultez la documentation du code source.
