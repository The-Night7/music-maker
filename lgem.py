
import numpy as np
import wave
import struct

SAMPLE_RATE = 44100
BPM = 70
BEAT = 60.0 / BPM  # durée d'un temps en secondes

def adsr(length, sr, attack=0.08, decay=0.1, sustain=0.7, release=0.15):
    n = int(length * sr)
    env = np.zeros(n)
    a = int(attack * sr)
    d = int(decay * sr)
    r = int(release * sr)
    s_end = n - r
    if a > 0:
        env[:a] = np.linspace(0, 1, a)
    if d > 0 and a + d < n:
        env[a:a+d] = np.linspace(1, sustain, d)
    if s_end > a + d:
        env[a+d:s_end] = sustain
    if r > 0:
        env[s_end:] = np.linspace(sustain, 0, n - s_end)
    return env

def piano_tone(freq, duration, sr=SAMPLE_RATE, velocity=0.6):
    """Son de piano acoustique simulé avec harmoniques et decay naturel"""
    t = np.linspace(0, duration, int(sr * duration), False)
    # Harmoniques du piano
    wave_out = (
        np.sin(2 * np.pi * freq * t) * 1.0 +
        np.sin(2 * np.pi * freq * 2 * t) * 0.5 +
        np.sin(2 * np.pi * freq * 3 * t) * 0.25 +
        np.sin(2 * np.pi * freq * 4 * t) * 0.12 +
        np.sin(2 * np.pi * freq * 5 * t) * 0.06 +
        np.sin(2 * np.pi * freq * 6 * t) * 0.03
    )
    # Decay exponentiel naturel du piano
    decay_env = np.exp(-t * (2.5 + freq / 800))
    # Légère attaque percussive
    attack_env = 1 - np.exp(-t * 80)
    env = attack_env * decay_env
    # Légère variation aléatoire pour humaniser
    noise = np.random.normal(0, 0.002, len(t))
    return (wave_out * env * velocity + noise) * 0.3

def cello_tone(freq, duration, sr=SAMPLE_RATE, velocity=0.5):
    """Son de violoncelle avec vibrato et harmoniques riches"""
    t = np.linspace(0, duration, int(sr * duration), False)
    # Vibrato naturel (légèrement irrégulier)
    vibrato_rate = 4.5 + np.random.uniform(-0.3, 0.3)
    vibrato_depth = 0.012
    vibrato = 1 + vibrato_depth * np.sin(2 * np.pi * vibrato_rate * t)
    # Harmoniques du violoncelle (riches en harmoniques impaires)
    wave_out = (
        np.sin(2 * np.pi * freq * vibrato * t) * 1.0 +
        np.sin(2 * np.pi * freq * 2 * vibrato * t) * 0.6 +
        np.sin(2 * np.pi * freq * 3 * vibrato * t) * 0.4 +
        np.sin(2 * np.pi * freq * 4 * vibrato * t) * 0.2 +
        np.sin(2 * np.pi * freq * 5 * vibrato * t) * 0.1
    )
    # Envelope ADSR lente (archet)
    env = adsr(duration, sr, attack=0.3, decay=0.1, sustain=0.8, release=0.4)
    noise = np.random.normal(0, 0.003, len(t))
    return (wave_out * env * velocity + noise) * 0.25

def soft_bass(freq, duration, sr=SAMPLE_RATE, velocity=0.4):
    """Basse acoustique douce"""
    t = np.linspace(0, duration, int(sr * duration), False)
    wave_out = (
        np.sin(2 * np.pi * freq * t) * 1.0 +
        np.sin(2 * np.pi * freq * 2 * t) * 0.4 +
        np.sin(2 * np.pi * freq * 3 * t) * 0.15
    )
    decay_env = np.exp(-t * 1.8)
    attack_env = 1 - np.exp(-t * 40)
    env = attack_env * decay_env
    noise = np.random.normal(0, 0.001, len(t))
    return (wave_out * env * velocity + noise) * 0.35

def warm_pad(freq, duration, sr=SAMPLE_RATE, velocity=0.3, dissonant=False):
    """Nappe de synthé chaleureuse (ou légèrement dissonante)"""
    t = np.linspace(0, duration, int(sr * duration), False)
    detune = 1.003 if not dissonant else 1.012
    wave_out = (
        np.sin(2 * np.pi * freq * t) * 1.0 +
        np.sin(2 * np.pi * freq * detune * t) * 0.8 +
        np.sin(2 * np.pi * freq * 2 * t) * 0.3
    )
    env = adsr(duration, sr, attack=0.6, decay=0.2, sustain=0.7, release=0.8)
    return wave_out * env * velocity * 0.2

def soft_kick(duration=0.4, sr=SAMPLE_RATE):
    """Kick de batterie acoustique doux"""
    t = np.linspace(0, duration, int(sr * duration), False)
    freq_env = 120 * np.exp(-t * 18) + 40
    wave_out = np.sin(2 * np.pi * freq_env * t)
    amp_env = np.exp(-t * 12)
    noise = np.random.normal(0, 0.01, len(t)) * np.exp(-t * 30)
    return (wave_out * amp_env + noise) * 0.5

def soft_snare(duration=0.25, sr=SAMPLE_RATE):
    """Caisse claire acoustique douce"""
    t = np.linspace(0, duration, int(sr * duration), False)
    tone = np.sin(2 * np.pi * 200 * t) * np.exp(-t * 20)
    noise = np.random.normal(0, 1, len(t)) * np.exp(-t * 15)
    env = np.exp(-t * 10)
    return (tone * 0.3 + noise * 0.7) * env * 0.25

def brush_hihat(duration=0.1, sr=SAMPLE_RATE):
    """Hihat brossé (très doux, humain)"""
    t = np.linspace(0, duration, int(sr * duration), False)
    noise = np.random.normal(0, 1, len(t))
    # Filtre passe-haut simulé
    env = np.exp(-t * 40)
    return noise * env * 0.08

def note_freq(note, octave):
    notes = {'C': 0, 'C#': 1, 'D': 2, 'D#': 3, 'E': 4, 'F': 5,
             'F#': 6, 'G': 7, 'G#': 8, 'A': 9, 'A#': 10, 'B': 11}
    semitones = (octave - 4) * 12 + notes[note] - 9
    return 440.0 * (2 ** (semitones / 12))

# Accords Dm, Bb, F, C, Gm, A
CHORDS = {
    'Dm': [note_freq('D', 4), note_freq('F', 4), note_freq('A', 4)],
    'Bb': [note_freq('A#', 3), note_freq('D', 4), note_freq('F', 4)],
    'F':  [note_freq('F', 3), note_freq('A', 3), note_freq('C', 4)],
    'C':  [note_freq('C', 3), note_freq('E', 3), note_freq('G', 3)],
    'Gm': [note_freq('G', 3), note_freq('A#', 3), note_freq('D', 4)],
    'A':  [note_freq('A', 3), note_freq('C#', 4), note_freq('E', 4)],
}

def mix_at(base, signal, pos):
    end = pos + len(signal)
    if end > len(base):
        signal = signal[:len(base) - pos]
    base[pos:pos + len(signal)] += signal

def arpeggio(chord_name, duration_beats, sr=SAMPLE_RATE, velocity=0.5, dissonant=False):
    """Arpège de piano sur un accord"""
    freqs = CHORDS[chord_name]
    total = int(duration_beats * BEAT * sr)
    out = np.zeros(total)
    note_dur = BEAT * 0.5
    step = int(BEAT * 0.25 * sr)
    for i, freq in enumerate(freqs * 4):
        pos = i * step
        if pos >= total:
            break
        n = piano_tone(freq, note_dur, sr, velocity)
        mix_at(out, n, pos)
    return out

def chord_piano(chord_name, duration_beats, sr=SAMPLE_RATE, velocity=0.45):
    """Accord plaqué au piano"""
    freqs = CHORDS[chord_name]
    total = int(duration_beats * BEAT * sr)
    out = np.zeros(total)
    for i, freq in enumerate(freqs):
        offset = int(i * 0.03 * sr)  # léger étalement humain
        n = piano_tone(freq, duration_beats * BEAT, sr, velocity)
        mix_at(out, n, offset)
    return out

def cello_chord(chord_name, duration_beats, sr=SAMPLE_RATE, velocity=0.4, dissonant=False):
    """Violoncelle sur la note fondamentale + quinte"""
    freqs = CHORDS[chord_name]
    total = int(duration_beats * BEAT * sr)
    out = np.zeros(total)
    # Fondamentale une octave en dessous
    c = cello_tone(freqs[0] / 2, duration_beats * BEAT, sr, velocity)
    mix_at(out, c, 0)
    if len(freqs) > 2:
        c2 = cello_tone(freqs[2] / 2, duration_beats * BEAT, sr, velocity * 0.6)
        mix_at(out, c2, 0)
    return out

def bass_note(chord_name, duration_beats, sr=SAMPLE_RATE, velocity=0.5):
    freq = CHORDS[chord_name][0] / 2
    return soft_bass(freq, duration_beats * BEAT, sr, velocity)

def pad_chord(chord_name, duration_beats, sr=SAMPLE_RATE, velocity=0.3, dissonant=False):
    freqs = CHORDS[chord_name]
    total = int(duration_beats * BEAT * sr)
    out = np.zeros(total)
    for freq in freqs:
        p = warm_pad(freq / 2, duration_beats * BEAT, sr, velocity, dissonant)
        mix_at(out, p, 0)
    return out

def human_beat(pattern_beats, sr=SAMPLE_RATE, chaotic=False):
    """Batterie acoustique humaine avec micro-variations de timing"""
    total = int(pattern_beats * BEAT * sr)
    out = np.zeros(total)
    beat_samples = int(BEAT * sr)
    half_beat = beat_samples // 2
    
    for b in range(pattern_beats):
        # Micro-variation humaine de timing
        human_offset = int(np.random.uniform(-0.015, 0.015) * sr)
        
        # Kick sur temps 1 et 3
        if b % 4 == 0 or b % 4 == 2:
            pos = b * beat_samples + human_offset
            if 0 <= pos < total:
                mix_at(out, soft_kick(), pos)
        
        # Snare sur temps 2 et 4
        if b % 4 == 1 or b % 4 == 3:
            pos = b * beat_samples + human_offset
            if 0 <= pos < total:
                mix_at(out, soft_snare(), pos)
        
        # Hihat brossé sur chaque demi-temps
        for h in range(2):
            if chaotic and np.random.random() > 0.5:
                continue
            pos = b * beat_samples + h * half_beat + int(np.random.uniform(-0.01, 0.01) * sr)
            if 0 <= pos < total:
                mix_at(out, brush_hihat(), pos)
    
    return out

# ============================================================
# COMPOSITION COMPLÈTE
# ============================================================

sr = SAMPLE_RATE
progression_verse = ['Dm', 'Bb', 'F', 'C']
progression_chorus = ['Bb', 'F', 'C', 'Dm']
progression_bridge = ['Gm', 'Dm', 'Bb', 'A']

def section(chords_list, beats_each=4, with_bass=False, with_cello=True,
            with_beat=False, arp=True, dissonant=False, chaotic=False, velocity_scale=1.0):
    total_beats = len(chords_list) * beats_each
    total_samples = int(total_beats * BEAT * sr)
    out = np.zeros(total_samples)
    
    for i, chord in enumerate(chords_list):
        pos = int(i * beats_each * BEAT * sr)
        
        # Piano arpège ou accord
        if arp:
            p = arpeggio(chord, beats_each, sr, 0.45 * velocity_scale)
        else:
            p = chord_piano(chord, beats_each, sr, 0.4 * velocity_scale)
        mix_at(out, p, pos)
        
        # Violoncelle
        if with_cello:
            c = cello_chord(chord, beats_each, sr, 0.4 * velocity_scale, dissonant)
            mix_at(out, c, pos)
        
        # Basse
        if with_bass:
            b = bass_note(chord, beats_each, sr, 0.45 * velocity_scale)
            mix_at(out, b, pos)
        
        # Pad
        p2 = pad_chord(chord, beats_each, sr, 0.25 * velocity_scale, dissonant)
        mix_at(out, p2, pos)
    
    # Batterie
    if with_beat:
        beat_track = human_beat(total_beats, sr, chaotic)
        mix_at(out, beat_track, 0)
    
    return out

# --- INTRO (8 beats) : piano seul + violoncelle ---
intro = section(['Dm', 'Bb'], beats_each=4, with_cello=True, arp=True, velocity_scale=0.7)

# --- COUPLET 1 (32 beats) : piano arpège + violoncelle discret ---
verse1_chords = ['Dm','Bb','F','C', 'Dm','Bb','F','C']
verse1 = section(verse1_chords, beats_each=4, with_cello=True, arp=True, velocity_scale=0.8)

# --- REFRAIN 1 (32 beats) : tout entre, plus lourd ---
chorus1_chords = ['Bb','F','C','Dm', 'Bb','F','C','Dm']
chorus1 = section(chorus1_chords, beats_each=4, with_bass=True, with_cello=True,
                  with_beat=True, arp=False, velocity_scale=1.0)

# --- COUPLET 2 (32 beats) : piano + basse discrète ---
verse2_chords = ['Dm','Bb','F','C', 'Dm','Bb','F','C']
verse2 = section(verse2_chords, beats_each=4, with_bass=True, with_cello=True,
                 arp=True, velocity_scale=0.85)

# --- PRÉ-REFRAIN (20 beats) : violoncelle grinçant, tension ---
pre_chorus_chords = ['Bb','F','C','Dm', 'Gm']
pre_chorus = section(pre_chorus_chords, beats_each=4, with_bass=True, with_cello=True,
                     arp=True, dissonant=True, velocity_scale=0.9)

# --- REFRAIN 2 (32 beats) : massif et tragique ---
chorus2_chords = ['Bb','F','C','Dm', 'Bb','F','C','Dm']
chorus2 = section(chorus2_chords, beats_each=4, with_bass=True, with_cello=True,
                  with_beat=True, arp=False, dissonant=False, velocity_scale=1.15)

# --- PONT (32 beats) : chaotique, dissonant ---
bridge_chords = ['Gm','Dm','Bb','A', 'Gm','Dm','Bb','A']
bridge = section(bridge_chords, beats_each=4, with_bass=True, with_cello=True,
                 with_beat=True, arp=True, dissonant=True, chaotic=True, velocity_scale=1.1)

# --- REFRAIN FINAL (32 beats) ---
chorus3_chords = ['Bb','F','C','Dm', 'Bb','F','C','Dm']
chorus3 = section(chorus3_chords, beats_each=4, with_bass=True, with_cello=True,
                  with_beat=True, arp=False, velocity_scale=1.1)

# --- OUTRO (24 beats) : piano seul, hésitant, fade out ---
outro_chords = ['Dm','Bb','F','C', 'Gm','C']
outro_raw = section(outro_chords, beats_each=4, with_cello=True, arp=True, velocity_scale=0.6)
# Fade out progressif
fade = np.linspace(1.0, 0.0, len(outro_raw)) ** 1.5
outro = outro_raw * fade

# ============================================================
# ASSEMBLAGE FINAL
# ============================================================
full_song = np.concatenate([intro, verse1, chorus1, verse2, pre_chorus,
                             chorus2, bridge, chorus3, outro])

# Normalisation douce (pas de clipping)
max_val = np.max(np.abs(full_song))
if max_val > 0:
    full_song = full_song / max_val * 0.88

# Légère compression douce pour humaniser
threshold = 0.6
ratio = 3.0
mask = np.abs(full_song) > threshold
full_song[mask] = np.sign(full_song[mask]) * (threshold + (np.abs(full_song[mask]) - threshold) / ratio)

# Ré-normalisation
full_song = full_song / np.max(np.abs(full_song)) * 0.9

# Conversion en 16-bit
audio_int = (full_song * 32767).astype(np.int16)

# Sauvegarde WAV
with wave.open('la_guerre_en_moi.wav', 'w') as wf:
    wf.setnchannels(1)
    wf.setsampwidth(2)
    wf.setframerate(SAMPLE_RATE)
    wf.writeframes(audio_int.tobytes())

duration_sec = len(full_song) / SAMPLE_RATE
print(f"✅ Fichier généré : {duration_sec:.1f} secondes ({duration_sec/60:.1f} min)")
print(f"   Sections : Intro + V1 + Ch1 + V2 + Pre + Ch2 + Bridge + Ch3 + Outro")

