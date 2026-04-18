# Guide d'Utilisation - Application Musicale Multi-Instruments

## Table des Matières
1. [Installation](#installation)
2. [Lancement](#lancement)
3. [Menu Principal](#menu-principal)
4. [Instruments](#instruments)
5. [Contrôles](#contrôles)
6. [Astuces](#astuces)

## Installation

### Prérequis Système
- Système Linux/Unix (Ubuntu, Debian, Fedora, macOS)
- GCC ou Clang
- Make
- 50 MB d'espace disque libre

### Étapes d'Installation

1. **Extraire les fichiers**
```bash
tar xzf music_app.tar.gz
cd music_app
```

2. **Compiler l'application**
```bash
make
```

Ou pour une compilation optimisée:
```bash
make clean && make
```

3. **Vérifier l'installation**
```bash
./bin/music_app
```

### Dépannage de la Compilation

**Erreur: gcc: command not found**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential

# Fedora
sudo dnf install gcc make
```

**Erreur: file not found**
Assurez-vous d'être dans le répertoire `music_app`:
```bash
pwd  # Doit montrer: /chemin/vers/music_app
ls -la Makefile  # Vérifie la présence du Makefile
```

## Lancement

### Première Utilisation

```bash
cd music_app
./bin/music_app
```

L'application affichera:
```
=== Application Musicale Multi-Instruments ===

[INFO] Initialisation du moteur audio...
[OK] Moteur audio démarré
[INFO] Lancement de l'interface utilisateur...

=== Application Musicale Multi-Instruments ===
Instruments chargés: Piano, Batterie, Guitare

=== Menu Principal ===
1. Jouer des notes au piano
2. Jouer la batterie
3. Jouer de la guitare
4. Afficher l'état du mixeur
5. Exporter en WAV
6. Quitter
Choix: 
```

## Menu Principal

Après le lancement, vous verrez le menu principal avec 6 options:

| Option | Description |
|--------|-------------|
| 1 | Accès au piano avec clavier QWERTY |
| 2 | Accès à la batterie avec contrôles simples |
| 3 | Accès à la guitare avec 6 cordes |
| 4 | Affichage de l'état du mixeur audio |
| 5 | Exporte les performances en WAV |
| 6 | Ferme l'application |

### Retour au Menu Principal
Pour revenir au menu principal depuis n'importe quel instrument, appuyez sur **Q**.

## Instruments

### 1. Piano

Le piano virtuel offre **8 voix polyphoniques** pour une polyphonie riche.

#### Clavier du Piano
```
Octave 4 (par défaut):
┌─ Note ─┬─ Touche ─┬─ Fréquence ─┐
│ Do     │    A     │   262 Hz    │
│ Ré     │    Z     │   294 Hz    │
│ Mi     │    E     │   330 Hz    │
│ Fa     │    R     │   349 Hz    │
│ Sol    │    T     │   392 Hz    │
│ La     │    Y     │   440 Hz    │
│ Si     │    U     │   494 Hz    │
└────────┴──────────┴─────────────┘

Contrôles Spéciaux:
- W : Augmenter l'octave
- X : Diminuer l'octave
- Q : Retour au menu
```

#### Exemple d'Utilisation

1. Sélectionnez l'option 1 du menu
2. Appuyez sur les touches pour jouer
3. Appuyez sur **W** pour montrer d'une octave
4. Continuez à jouer
5. Appuyez sur **Q** pour revenir au menu

#### Paramètres ADSR du Piano

| Paramètre | Valeur | Signification |
|-----------|--------|---------------|
| Attack    | 5ms    | Montée rapide |
| Decay     | 200ms  | Décroissance progressive |
| Sustain   | 70%    | Volume sustain |
| Release   | 300ms  | Relâchement long |

### 2. Batterie

La batterie propose **4 sons différents** pour créer des rythmes.

#### Contrôles de la Batterie
```
┌─ Son ─────────┬─ Touche ─┬─ Fréquence ─┐
│ Kick          │    K     │   60 Hz     │
│ Snare         │    S     │  200 Hz     │
│ Hi-hat fermé  │    H     │ 8000 Hz     │
│ Hi-hat ouvert │    O     │ 9000 Hz     │
└───────────────┴──────────┴─────────────┘

- Q : Retour au menu
```

#### Exemple de Rythme

Créons un rythme simple (4 temps):
```
Temps 1: K (Kick)
Temps 2: H (Hi-hat)
Temps 3: S (Snare)
Temps 4: H (Hi-hat)
```

Répétez pour créer un groove!

#### Caractéristiques des Tambours

- **Kick (K)**: Grosse caisse grave et percussive
- **Snare (S)**: Caisse claire avec attaque sharp
- **Hi-hat Fermé (H)**: Son métallique court et sec
- **Hi-hat Ouvert (O)**: Son métallique prolongé avec decay

### 3. Guitare

La guitare propose **6 cordes** avec accordage standard.

#### Accordage Standard des Cordes
```
┌─ Corde ─┬─ Touche ─┬─ Note ─┬─ Fréquence ─┐
│    1    │    1     │   E    │   330 Hz    │
│    2    │    2     │   B    │   247 Hz    │
│    3    │    3     │   G    │   196 Hz    │
│    4    │    4     │   D    │   147 Hz    │
│    5    │    5     │   A    │   110 Hz    │
│    6    │    6     │   E    │   82 Hz     │
└─────────┴──────────┴────────┴─────────────┘

Contrôles Spéciaux:
- W : Augmenter la fréquence
- S : Diminuer la fréquence
- Q : Retour au menu
```

#### Exemple d'Utilisation

1. Appuyez sur 1-6 pour jouer les cordes
2. Utilisez **W** et **S** pour modifier la fréquence
3. Créez des mélodies simples
4. Retournez au menu avec **Q**

## Contrôles

### Clavier Global

```
┌─ Action ────────────────┬─ Méthode ─────────┐
│ Naviguer dans le menu   │ Appuyer 1-6       │
│ Quitter une section     │ Appuyer Q         │
│ Augmenter un paramètre  │ Appuyer W (piano) │
│ Diminuer un paramètre   │ Appuyer X (piano) │
│ Créer un son            │ Appuyer note      │
└─────────────────────────┴───────────────────┘
```

### Combinaisons Utiles

#### Piano Polyphonique
```
Appuyez simultanément sur:
- A + Z        → Do + Ré (intervalle)
- A + E + Y    → Do + Mi + La (accord de Do majeur)
- E + Z + T    → Mi + Ré + Sol (accord complexe)
```

#### Batterie Syncopée
```
Alternez rapidement:
- K + H -> S + H  → Groove hip-hop simple
- K -> S -> K -> S → Beat 4/4 classique
```

## Astuces

### Astuce 1: Créer un Accord

Pour jouer un accord majeur au piano:
1. Appuyez sur la fondamentale (ex: A pour Do)
2. Tout en maintenant, appuyez sur la tierce (E pour Mi)
3. Puis la quinte (T pour Sol)

**Exemple d'Accord de Do Majeur:**
```
Appuyez rapidement: A → E → T
```

### Astuce 2: Rythme Basique à la Batterie

```
Temps:     1    2    3    4
Beat:      K    H    S    H
Touche:    K    H    S    H

Répétez ce motif pour créer un groove!
```

### Astuce 3: Melody à la Guitare

Utilisez les cordes comme une gamme:
```
Corde 1 (La 4):      Y
Corde 2 (Si 3):      E
Corde 3 (Sol 3):     T
Corde 4 (Ré 3):      Z
Corde 5 (La 2):      A
Corde 6 (Mi 2):      3
```

### Astuce 4: Augmenter le Volume

Utilisez l'option 4 du menu pour voir l'état du mixeur:
```
=== État du Mixeur ===
Nombre de pistes: 4
Volume maître: 0.80 (80%)
```

### Astuce 5: Expérimenter avec les Octaves

Au piano, utilisez **W** et **X** pour naviguer entre octaves:
```
Octave 2: Notes graves (bas du clavier)
Octave 4: Notes médium (Son agréable)
Octave 6: Notes aigus (Sons brillants)
```

## Dépannage

### Problème: L'application n'émet aucun son

**Causes possibles:**
1. Les paramètres ADSR sont peut-être mal réglés
2. Le volume maître est peut-être à 0%
3. Une piste est peut-être en mute

**Solutions:**
1. Vérifiez l'option 4 (État du mixeur)
2. Augmentez le volume maître
3. Assurez-vous qu'aucune piste n'est muette

### Problème: Compilation échouée

**Erreur typique:**
```
error: conflicting types for 'Synthesizer'
```

**Solution:**
```bash
make clean
make
```

### Problème: Performances faibles

**Solutions:**
1. Réduisez le nombre de voix actives
2. Compilez avec optimisation: `make clean && CFLAGS="-O3" make`
3. Fermez les autres applications

## Raccourcis Clavier Résumé

| Contexte | Touches | Action |
|----------|---------|--------|
| Menu | 1-6 | Sélectionner option |
| Piano | A-U | Jouer notes |
| Piano | W/X | Changer octave |
| Batterie | K/S/H/O | Jouer sons |
| Guitare | 1-6 | Jouer cordes |
| Guitare | W/S | Changer fréquence |
| Global | Q | Retour au menu |

## Ressources Additionnelles

- **Fichier README.md**: Documentation technique complète
- **Fichier Makefile**: Instructions de compilation
- **Code Source**: Consultez les fichiers `.c` et `.h`

## Soporte et Feedback

Pour les questions sur l'utilisation:
1. Consultez ce guide
2. Relisez le fichier README.md
3. Vérifiez les messages d'erreur à l'écran

## Version de l'Application

**Version**: 1.0.0  
**Date**: 2026-04-12  
**Auteur**: Application Musicale Multi-Instruments en C

---

Bonne musique! 🎵
