
import numpy as np
import wave
import struct
import os

# Paramètres audio
SAMPLE_RATE = 44100
AMPLITUDE = 32767  # Max pour 16-bit audio

def note_to_freq(note, octave=4):
    """Convertit une note en fréquence (Hz)"""
    notes = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']
    semitones = notes.index(note) - notes.index('A') + (octave - 4) * 12
    return 440.0 * (2 ** (semitones / 12.0))

def generate_wave(freq, duration, wave_type='sine', volume=0.5):
    """Génère une onde sonore"""
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration), endpoint=False)
    
    if wave_type == 'sine':
        wave = np.sin(2 * np.pi * freq * t)
    elif wave_type == 'square':
        wave = np.sign(np.sin(2 * np.pi * freq * t))
    elif wave_type == 'sawtooth':
        wave = 2 * (t * freq - np.floor(t * freq + 0.5))
    elif wave_type == 'triangle':
        wave = 2 * np.abs(2 * (t * freq - np.floor(t * freq + 0.5))) - 1
    
    # Envelope ADSR simple
    attack = int(0.05 * len(wave))
    decay = int(0.1 * len(wave))
    sustain_level = 0.7
    release = int(0.15 * len(wave))
    
    envelope = np.ones(len(wave))
    envelope[:attack] = np.linspace(0, 1, attack)
    envelope[attack:attack+decay] = np.linspace(1, sustain_level, decay)
    envelope[-release:] = np.linspace(sustain_level, 0, release)
    
    return (wave * envelope * volume * AMPLITUDE).astype(np.int16)

def silence(duration):
    """Génère du silence"""
    return np.zeros(int(SAMPLE_RATE * duration), dtype=np.int16)

def save_wav(filename, audio_data):
    """Sauvegarde en fichier WAV"""
    with wave.open(filename, 'w') as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(SAMPLE_RATE)
        f.writeframes(struct.pack(f'<{len(audio_data)}h', *audio_data))

# ============================================================
# 🎵 Composition : "Pixel Dreams" - Mélodie style chiptune
# ============================================================

# Mélodie principale (notes, octave, durée)
melody = [
    ('E', 5, 0.25), ('E', 5, 0.25), ('R', 0, 0.125), ('E', 5, 0.25),
    ('R', 0, 0.125), ('C', 5, 0.25), ('E', 5, 0.25), ('R', 0, 0.125),
    ('G', 5, 0.5), ('R', 0, 0.25), ('G', 4, 0.5), ('R', 0, 0.25),
    ('C', 5, 0.375), ('G', 4, 0.25), ('R', 0, 0.125), ('E', 4, 0.375),
    ('A', 4, 0.25), ('R', 0, 0.125), ('B', 4, 0.25), ('R', 0, 0.125),
    ('A#', 4, 0.125), ('A', 4, 0.25), ('R', 0, 0.125),
    ('G', 4, 0.333), ('E', 5, 0.333), ('G', 5, 0.333),
    ('A', 5, 0.5), ('F', 5, 0.25), ('G', 5, 0.25),
    ('R', 0, 0.125), ('E', 5, 0.25), ('R', 0, 0.125),
    ('C', 5, 0.25), ('D', 5, 0.25), ('B', 4, 0.375), ('R', 0, 0.25),
]

# Basse (notes, octave, durée)
bass = [
    ('C', 3, 0.5), ('C', 3, 0.5),
    ('G', 3, 0.5), ('G', 3, 0.5),
    ('A', 3, 0.5), ('A', 3, 0.5),
    ('F', 3, 0.5), ('F', 3, 0.5),
    ('C', 3, 0.5), ('C', 3, 0.5),
    ('G', 3, 0.5), ('G', 3, 0.5),
    ('A', 3, 0.5), ('A', 3, 0.5),
    ('F', 3, 0.5), ('F', 3, 0.5),
]

# Génération de la mélodie
melody_audio = np.array([], dtype=np.int16)
for note, octave, dur in melody:
    if note == 'R':
        melody_audio = np.concatenate([melody_audio, silence(dur)])
    else:
        freq = note_to_freq(note, octave)
        melody_audio = np.concatenate([melody_audio, generate_wave(freq, dur, 'square', 0.4)])

# Génération de la basse
bass_audio = np.array([], dtype=np.int16)
for note, octave, dur in bass:
    if note == 'R':
        bass_audio = np.concatenate([bass_audio, silence(dur)])
    else:
        freq = note_to_freq(note, octave)
        bass_audio = np.concatenate([bass_audio, generate_wave(freq, dur, 'sawtooth', 0.3)])

# Ajustement de la longueur
min_len = min(len(melody_audio), len(bass_audio))
melody_audio = melody_audio[:min_len]
bass_audio = bass_audio[:min_len]

# Mix des deux pistes
mixed = np.clip(melody_audio.astype(np.int32) + bass_audio.astype(np.int32), -32768, 32767).astype(np.int16)

# Sauvegarde
output_path = 'pixel_dreams.wav'
save_wav(output_path, mixed)
print(f"✅ Musique générée : {output_path}")
print(f"⏱️  Durée : {len(mixed)/SAMPLE_RATE:.2f} secondes")
print(f"🎵 Pistes : Mélodie (square wave) + Basse (sawtooth)")

