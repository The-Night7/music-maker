
import numpy as np
import wave
import struct

# ── paramètres globaux ──────────────────────────────────────────────────────
SR   = 44100   # sample rate
BPM  = 76      # tempo lent, introspectif
BEAT = 60 / BPM

def freq(note, octave=4):
    """Convertit une note en fréquence Hz (tempérament égal, A4=440)."""
    notes = {'C':0,'C#':1,'Db':1,'D':2,'D#':3,'Eb':3,'E':4,'F':5,
             'F#':6,'Gb':6,'G':7,'G#':8,'Ab':8,'A':9,'A#':10,'Bb':10,'B':11}
    semitones = (octave - 4) * 12 + notes[note] - 9
    return 440.0 * (2 ** (semitones / 12))

def sine(f, dur, vol=0.5):
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    return (vol * np.sin(2 * np.pi * f * t)).astype(np.float32)

def square(f, dur, vol=0.3):
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    return (vol * np.sign(np.sin(2 * np.pi * f * t))).astype(np.float32)

def sawtooth(f, dur, vol=0.25):
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    return (vol * (2 * (t * f - np.floor(t * f + 0.5)))).astype(np.float32)

def pad(f, dur, vol=0.18):
    """Pad doux : somme de sinus légèrement désaccordés."""
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    w = (np.sin(2*np.pi*f*t) + 0.6*np.sin(2*np.pi*f*1.005*t)
         + 0.4*np.sin(2*np.pi*f*0.995*t))
    return (vol * w / 2).astype(np.float32)

def adsr(sig, a=0.08, d=0.1, s=0.75, r=0.15):
    n = len(sig)
    env = np.ones(n, dtype=np.float32)
    ai = int(a * SR); di = int(d * SR); ri = int(r * SR)
    si = n - ai - di - ri
    if ai > 0: env[:ai] = np.linspace(0, 1, ai)
    if di > 0: env[ai:ai+di] = np.linspace(1, s, di)
    if si > 0: env[ai+di:ai+di+si] = s
    if ri > 0: env[ai+di+si:] = np.linspace(s, 0, len(env[ai+di+si:]))
    return sig * env

def silence(dur):
    return np.zeros(int(SR * dur), dtype=np.float32)

def mix(*tracks):
    """Mixe plusieurs pistes de même longueur."""
    length = max(len(t) for t in tracks)
    out = np.zeros(length, dtype=np.float32)
    for t in tracks:
        out[:len(t)] += t
    return np.clip(out, -1, 1)

def overlay(base, layer, start_sec):
    """Superpose `layer` sur `base` à partir de `start_sec`."""
    s = int(start_sec * SR)
    end = s + len(layer)
    if end > len(base):
        base = np.concatenate([base, np.zeros(end - len(base), dtype=np.float32)])
    base[s:end] += layer
    return base

# ── Gamme : Ré mineur naturel (D E F G A Bb C) ─────────────────────────────
# Accords (notes jouées ensemble comme pad)
def chord(notes_list, dur, vol=0.12):
    sig = silence(dur)
    for n, o in notes_list:
        sig = mix(sig, adsr(pad(freq(n, o), dur, vol)))
    return sig

Dm  = [('D',3),('F',3),('A',3)]
Bb  = [('Bb',2),('D',3),('F',3)]
F   = [('F',2),('A',2),('C',3)]
C   = [('C',3),('E',3),('G',3)]
Am  = [('A',2),('C',3),('E',3)]
Gm  = [('G',2),('Bb',2),('D',3)]

# Progression principale : Dm - Bb - F - C  (×2 = 8 mesures)
def prog_chords(reps=2):
    seq = []
    for _ in range(reps):
        for ch in [Dm, Bb, F, C]:
            seq.append(chord(ch, BEAT * 4, vol=0.13))
    return np.concatenate(seq)

# ── Mélodie (piano/lead) ────────────────────────────────────────────────────
def melody_phrase(pattern):
    """pattern = liste de (note, octave, beats) ou ('R', _, beats) pour silence."""
    out = []
    for item in pattern:
        n, o, b = item
        dur = BEAT * b
        if n == 'R':
            out.append(silence(dur))
        else:
            s = adsr(sine(freq(n, o), dur, vol=0.35), a=0.04, d=0.08, s=0.7, r=0.12)
            out.append(s)
    return np.concatenate(out)

# Couplet 1 – mélodie douce, descendante, introspective
verse1_mel = melody_phrase([
    ('D',5,1),('C',5,1),('A',4,1),('F',4,1),
    ('G',4,1),('F',4,1),('E',4,1),('R','_',1),
    ('D',5,1),('C',5,0.5),('Bb',4,0.5),('A',4,1),('G',4,1),
    ('F',4,1.5),('E',4,0.5),('R','_',1),
    ('A',4,1),('Bb',4,1),('C',5,1),('D',5,1),
    ('C',5,1),('A',4,1),('G',4,1),('R','_',1),
    ('F',4,1),('G',4,1),('A',4,1),('C',5,1),
    ('Bb',4,1.5),('A',4,0.5),('G',4,1),('R','_',1),
])

# Refrain – plus intense, montée émotionnelle
chorus_mel = melody_phrase([
    ('F',5,1),('E',5,0.5),('D',5,0.5),('C',5,1),('Bb',4,1),
    ('A',4,1),('R','_',1),
    ('G',4,1),('A',4,1),('Bb',4,1),('C',5,1),
    ('D',5,1),('C',5,1),('Bb',4,1),('R','_',1),
    ('A',4,1),('Bb',4,1),('C',5,1),('D',5,1),
    ('E',5,1),('D',5,1),('C',5,1),('R','_',1),
    ('D',5,2),('C',5,1),('Bb',4,1),
    ('A',4,2),('G',4,1),('F',4,1),
])

# Pont – tendu, haché
bridge_mel = melody_phrase([
    ('D',5,0.5),('R','_',0.5),('D',5,0.5),('R','_',0.5),
    ('C',5,1),('Bb',4,1),('A',4,1),
    ('G',4,0.5),('R','_',0.5),('G',4,0.5),('R','_',0.5),
    ('A',4,1),('Bb',4,1),('C',5,1),
    ('D',5,1),('C',5,1),('Bb',4,1),('A',4,1),
    ('G',4,2),('R','_',2),
    ('F',4,1),('G',4,1),('A',4,1),('Bb',4,1),
    ('C',5,2),('D',5,2),
])

# Outro – épuisé, descendant
outro_mel = melody_phrase([
    ('D',5,1.5),('C',5,0.5),('Bb',4,1),('A',4,1),
    ('G',4,2),('R','_',2),
    ('F',4,1),('G',4,1),('A',4,1),('Bb',4,1),
    ('A',4,2),('G',4,2),
    ('F',4,1),('E',4,1),('D',4,1),('C',4,1),
    ('D',4,4),
])

# ── Basse ───────────────────────────────────────────────────────────────────
def bass_line(pattern):
    out = []
    for n, o, b in pattern:
        dur = BEAT * b
        if n == 'R':
            out.append(silence(dur))
        else:
            s = adsr(sawtooth(freq(n, o), dur, vol=0.28), a=0.02, d=0.05, s=0.8, r=0.1)
            out.append(s)
    return np.concatenate(out)

bass_v = bass_line([
    ('D',2,2),('R','_',0.5),('D',2,0.5),('D',2,1),
    ('Bb',1,2),('R','_',0.5),('Bb',1,0.5),('F',2,1),
    ('F',2,2),('R','_',0.5),('F',2,0.5),('C',2,1),
    ('C',2,2),('R','_',0.5),('C',2,0.5),('G',2,1),
    ('D',2,2),('R','_',0.5),('D',2,0.5),('D',2,1),
    ('Bb',1,2),('R','_',0.5),('Bb',1,0.5),('F',2,1),
    ('F',2,2),('R','_',0.5),('F',2,0.5),('C',2,1),
    ('C',2,2),('R','_',0.5),('C',2,0.5),('G',2,1),
])

bass_c = bass_line([
    ('D',2,1.5),('R','_',0.5),('D',2,1),('F',2,1),
    ('Bb',1,1.5),('R','_',0.5),('Bb',1,1),('D',2,1),
    ('F',2,1.5),('R','_',0.5),('F',2,1),('A',2,1),
    ('C',2,1.5),('R','_',0.5),('C',2,1),('E',2,1),
    ('D',2,1.5),('R','_',0.5),('D',2,1),('F',2,1),
    ('Bb',1,1.5),('R','_',0.5),('Bb',1,1),('D',2,1),
    ('F',2,1.5),('R','_',0.5),('F',2,1),('A',2,1),
    ('C',2,2),('G',2,2),
])

# ── Batterie simple ─────────────────────────────────────────────────────────
def kick(dur=0.15, vol=0.6):
    t = np.linspace(0, dur, int(SR * dur))
    f0 = 150 * np.exp(-30 * t)
    env = np.exp(-20 * t)
    return (vol * env * np.sin(2 * np.pi * f0 * t)).astype(np.float32)

def snare(dur=0.2, vol=0.4):
    t = np.linspace(0, dur, int(SR * dur))
    noise = np.random.randn(len(t)).astype(np.float32)
    env = np.exp(-18 * t)
    tone = np.sin(2 * np.pi * 200 * t)
    return (vol * env * (0.5 * noise + 0.5 * tone)).astype(np.float32)

def hihat(dur=0.05, vol=0.15):
    t = np.linspace(0, dur, int(SR * dur))
    noise = np.random.randn(len(t)).astype(np.float32)
    env = np.exp(-60 * t)
    return (vol * env * noise).astype(np.float32)

def build_drums(measures=8, pattern='verse'):
    """Construit une piste de batterie sur `measures` mesures de 4 temps."""
    total = int(SR * BEAT * 4 * measures)
    track = np.zeros(total, dtype=np.float32)
    b = BEAT
    for m in range(measures):
        off = m * 4 * b
        if pattern == 'verse':
            # kick sur 1 et 3, snare sur 2 et 4, hihat sur chaque temps
            for beat_pos in [0, 2]:
                s = int((off + beat_pos * b) * SR)
                k = kick(); track[s:s+len(k)] += k
            for beat_pos in [1, 3]:
                s = int((off + beat_pos * b) * SR)
                sn = snare(); track[s:s+len(sn)] += sn
            for beat_pos in [0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5]:
                s = int((off + beat_pos * b) * SR)
                hh = hihat(); track[s:s+len(hh)] += hh
        elif pattern == 'chorus':
            # plus énergique : kick sur 1, 2.5, 3
            for beat_pos in [0, 2.5, 3]:
                s = int((off + beat_pos * b) * SR)
                k = kick(vol=0.7); track[s:s+len(k)] += k
            for beat_pos in [1, 3]:
                s = int((off + beat_pos * b) * SR)
                sn = snare(vol=0.5); track[s:s+len(sn)] += sn
            for beat_pos in [0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75,
                              2, 2.25, 2.5, 2.75, 3, 3.25, 3.5, 3.75]:
                s = int((off + beat_pos * b) * SR)
                hh = hihat(vol=0.12); track[s:s+len(hh)] += hh
        elif pattern == 'bridge':
            for beat_pos in [0, 1.5, 2, 3.5]:
                s = int((off + beat_pos * b) * SR)
                k = kick(vol=0.65); track[s:s+len(k)] += k
            for beat_pos in [1, 3]:
                s = int((off + beat_pos * b) * SR)
                sn = snare(vol=0.45); track[s:s+len(sn)] += sn
    return track

# ── Assemblage de la chanson ─────────────────────────────────────────────────
# Structure : Intro | Couplet1 | Refrain | Couplet2 | Pré-refrain | Refrain | Pont | Outro

def pad_to(sig, length):
    if len(sig) >= length: return sig[:length]
    return np.concatenate([sig, np.zeros(length - len(sig), dtype=np.float32)])

def section(mel, bass, chords_sig, drums):
    length = max(len(mel), len(bass), len(chords_sig), len(drums))
    return mix(pad_to(mel, length),
               pad_to(bass, length),
               pad_to(chords_sig, length),
               pad_to(drums, length))

# Intro (8 mesures, accords + batterie douce)
intro_dur = BEAT * 4 * 8
intro_chords = prog_chords(2)
intro_drums  = build_drums(8, 'verse') * 0.5
intro_mel    = silence(intro_dur)
intro_bass   = bass_v
intro_sec    = section(intro_mel, intro_bass, intro_chords, intro_drums)

# Couplet 1 (8 mesures)
v1_chords = prog_chords(2)
v1_drums  = build_drums(8, 'verse')
v1_sec    = section(verse1_mel, bass_v, v1_chords, v1_drums)

# Refrain (8 mesures)
c_chords  = prog_chords(2)
c_drums   = build_drums(8, 'chorus')
c_sec     = section(chorus_mel, bass_c, c_chords, c_drums)

# Couplet 2 (même mélodie que v1 légèrement plus forte)
v2_sec = section(verse1_mel * 1.05, bass_v, prog_chords(2), build_drums(8, 'verse'))

# Pré-refrain (4 mesures, accords Gm-Am-Bb-C)
pre_dur = BEAT * 4 * 4
pre_chords = np.concatenate([chord(Gm, BEAT*4), chord(Am, BEAT*4),
                              chord(Bb, BEAT*4), chord(C,  BEAT*4)])
pre_bass = bass_line([
    ('G',2,2),('R','_',0.5),('G',2,1.5),
    ('A',2,2),('R','_',0.5),('A',2,1.5),
    ('Bb',2,2),('R','_',0.5),('Bb',2,1.5),
    ('C',3,2),('R','_',0.5),('C',3,1.5),
])
pre_mel = melody_phrase([
    ('G',4,1),('A',4,1),('Bb',4,1),('C',5,1),
    ('A',4,1),('G',4,1),('F',4,1),('R','_',1),
    ('Bb',4,1),('C',5,1),('D',5,1),('Eb',5,1),
    ('D',5,1),('C',5,1),('Bb',4,1),('R','_',1),
])
pre_drums = build_drums(4, 'verse')
pre_sec   = section(pre_mel, pre_bass, pre_chords, pre_drums)

# Pont (8 mesures)
br_chords = prog_chords(2)
br_drums  = build_drums(8, 'bridge')
br_sec    = section(bridge_mel, bass_v, br_chords, br_drums)

# Outro (8 mesures, s'estompe)
out_dur    = BEAT * 4 * 8
out_chords = prog_chords(2) * 0.8
out_drums  = build_drums(8, 'verse') * 0.4
out_sec    = section(outro_mel, bass_v * 0.7, out_chords, out_drums)
# Fade out progressif
fade = np.linspace(1, 0, len(out_sec))
out_sec = (out_sec * fade).astype(np.float32)

# ── Concaténation finale ─────────────────────────────────────────────────────
song = np.concatenate([
    intro_sec,   # Intro
    v1_sec,      # Couplet 1
    c_sec,       # Refrain
    v2_sec,      # Couplet 2
    pre_sec,     # Pré-refrain
    c_sec,       # Refrain
    br_sec,      # Pont
    out_sec,     # Outro
])

# Normalisation finale
song = song / np.max(np.abs(song) + 1e-9) * 0.92

# ── Sauvegarde WAV ───────────────────────────────────────────────────────────
filename = 'la_guerre_en_moi.wav'
with wave.open(filename, 'w') as wf:
    wf.setnchannels(1)
    wf.setsampwidth(2)
    wf.setframerate(SR)
    data = (song * 32767).astype(np.int16)
    wf.writeframes(data.tobytes())

duration = len(song) / SR
print(f"✅ Fichier généré : {filename}")
print(f"⏱️  Durée totale  : {duration:.1f} secondes ({duration/60:.1f} minutes)")
print(f"🎵  Structure     : Intro → Couplet 1 → Refrain → Couplet 2 → Pré-refrain → Refrain → Pont → Outro")

