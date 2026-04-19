
import numpy as np
import wave
import struct

# ─── Paramètres globaux ───────────────────────────────────────────────────────
SAMPLE_RATE = 44100
BPM = 70
BEAT = 60.0 / BPM          # durée d'un temps en secondes
BAR  = BEAT * 4             # durée d'une mesure

# ─── Utilitaires fréquences ───────────────────────────────────────────────────
NOTE_NAMES = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B']

def freq(note, octave):
    n = NOTE_NAMES.index(note)
    midi = (octave + 1) * 12 + n
    return 440.0 * (2 ** ((midi - 69) / 12.0))

# Accords de base (Dm, Bb, F, C, Gm, A)
CHORDS = {
    'Dm': [freq('D',4), freq('F',4), freq('A',4)],
    'Bb': [freq('A#',3), freq('D',4), freq('F',4)],
    'F':  [freq('F',3), freq('A',3), freq('C',4)],
    'C':  [freq('C',4), freq('E',4), freq('G',4)],
    'Gm': [freq('G',3), freq('A#',3), freq('D',4)],
    'A':  [freq('A',3), freq('C#',4), freq('E',4)],
}

# ─── Générateurs d'onde ───────────────────────────────────────────────────────
def sine(f, dur, amp=0.5, sr=SAMPLE_RATE):
    t = np.linspace(0, dur, int(sr*dur), False)
    return amp * np.sin(2*np.pi*f*t)

def square(f, dur, amp=0.3, sr=SAMPLE_RATE):
    t = np.linspace(0, dur, int(sr*dur), False)
    return amp * np.sign(np.sin(2*np.pi*f*t))

def sawtooth(f, dur, amp=0.3, sr=SAMPLE_RATE):
    t = np.linspace(0, dur, int(sr*dur), False)
    return amp * (2*(t*f - np.floor(t*f+0.5)))

def silence(dur, sr=SAMPLE_RATE):
    return np.zeros(int(sr*dur))

def adsr(sig, sr=SAMPLE_RATE, a=0.05, d=0.1, s=0.7, r=0.15):
    n = len(sig)
    env = np.ones(n)
    na, nd, nr = int(a*sr), int(d*sr), int(r*sr)
    ns = n - na - nd - nr
    if ns < 0: ns = 0
    env[:na] = np.linspace(0, 1, na)
    env[na:na+nd] = np.linspace(1, s, nd)
    env[na+nd:na+nd+ns] = s
    env[na+nd+ns:] = np.linspace(s, 0, n-(na+nd+ns))
    return sig * env

def fade(sig, sr=SAMPLE_RATE, fade_in=0.0, fade_out=0.5):
    n = len(sig)
    env = np.ones(n)
    if fade_in > 0:
        ni = int(fade_in*sr)
        env[:ni] = np.linspace(0,1,ni)
    if fade_out > 0:
        no = int(fade_out*sr)
        env[n-no:] = np.linspace(1,0,no)
    return sig * env

# ─── Instruments ─────────────────────────────────────────────────────────────

def piano_arpeggio(chord_name, dur, amp=0.22):
    """Piano feutré : arpège rapide continu (comme une trotteuse)"""
    notes = CHORDS[chord_name]
    # on étend sur 2 octaves
    notes_ext = [n/2 for n in notes] + notes + [n*2 for n in notes[:2]]
    note_dur = BEAT / 3   # triolets rapides
    buf = []
    t = 0
    idx = 0
    while t < dur:
        nd = min(note_dur, dur - t)
        s = sine(notes_ext[idx % len(notes_ext)], nd, amp)
        s = adsr(s, a=0.01, d=0.05, s=0.6, r=0.1)
        buf.append(s)
        t += note_dur
        idx += 1
    return np.concatenate(buf)[:int(SAMPLE_RATE*dur)]

def piano_slow(chord_name, dur, amp=0.28):
    """Piano lent pour l'outro"""
    notes = CHORDS[chord_name]
    note_dur = BEAT
    buf = []
    t = 0
    idx = 0
    while t < dur:
        nd = min(note_dur * 0.85, dur - t)
        s = sine(notes[idx % len(notes)], nd, amp)
        s = adsr(s, a=0.02, d=0.1, s=0.65, r=0.2)
        buf.append(s)
        buf.append(silence(note_dur * 0.15))
        t += note_dur
        idx += 1
    return np.concatenate(buf)[:int(SAMPLE_RATE*dur)]

def cello(f, dur, amp=0.35, gritty=False):
    """Violoncelle : onde sinusoïdale + harmoniques, longues notes étirées"""
    t = np.linspace(0, dur, int(SAMPLE_RATE*dur), False)
    # fondamentale + harmoniques
    sig = amp * (
        0.6 * np.sin(2*np.pi*f*t) +
        0.25 * np.sin(2*np.pi*f*2*t) +
        0.1 * np.sin(2*np.pi*f*3*t) +
        0.05 * np.sin(2*np.pi*f*4*t)
    )
    if gritty:
        # légère distorsion pour le pré-refrain
        sig += 0.04 * np.sign(sig) * np.abs(sig)**0.5
    # vibrato lent
    vibrato = 1 + 0.008 * np.sin(2*np.pi*4.5*t)
    sig = sig * vibrato
    return adsr(sig, a=0.3, d=0.2, s=0.8, r=0.4)

def bass_note(f, dur, amp=0.30):
    """Basse sawtooth grave"""
    sig = sawtooth(f/2, dur, amp)
    return adsr(sig, a=0.02, d=0.08, s=0.7, r=0.1)

def pad_chord(chord_name, dur, amp=0.12, dissonant=False):
    """Nappe de synthé ambiante"""
    notes = CHORDS[chord_name]
    sig = silence(dur)
    for i, n in enumerate(notes):
        t = np.linspace(0, dur, int(SAMPLE_RATE*dur), False)
        wave_ = amp * np.sin(2*np.pi*n*t + i*0.15)
        if dissonant:
            # légère dissonance : +7 cents
            wave_ += (amp*0.3) * np.sin(2*np.pi*n*1.004*t)
        sig = sig + adsr(wave_, a=0.4, d=0.3, s=0.6, r=0.5)
    return sig

def kick(dur=0.25, amp=0.55):
    t = np.linspace(0, dur, int(SAMPLE_RATE*dur), False)
    f_env = 180 * np.exp(-30*t)
    sig = amp * np.sin(2*np.pi*f_env*t)
    return adsr(sig, a=0.002, d=0.08, s=0.0, r=0.05)

def snare(dur=0.18, amp=0.35):
    t = np.linspace(0, dur, int(SAMPLE_RATE*dur), False)
    noise = amp * np.random.randn(len(t))
    tone = 0.15 * np.sin(2*np.pi*200*t)
    sig = noise * np.exp(-18*t) + tone * np.exp(-25*t)
    return sig * amp

def hihat(dur=0.08, amp=0.15):
    t = np.linspace(0, dur, int(SAMPLE_RATE*dur), False)
    noise = np.random.randn(len(t))
    sig = noise * np.exp(-40*t)
    return sig * amp

def mix_at(buf, sig, pos):
    """Mixe sig dans buf à la position pos (en samples)"""
    end = pos + len(sig)
    if end > len(buf):
        sig = sig[:len(buf)-pos]
    buf[pos:pos+len(sig)] += sig

# ─── Construction de la chanson ──────────────────────────────────────────────

# Progression d'accords par section
PROG_VERSE   = ['Dm','Bb','F','C'] * 2   # 8 mesures
PROG_CHORUS  = ['Bb','F','C','Dm'] * 2   # 8 mesures
PROG_BRIDGE  = ['Gm','Dm','Bb','F','C','Dm','Bb','A']  # 8 mesures
PROG_PRE     = ['Bb','F','C','Dm','Bb','F','C','Dm']   # 8 mesures
PROG_OUTRO   = ['Dm','Bb','F','C','Gm','Bb','C','Dm']  # 8 mesures
PROG_INTRO   = ['Dm','Bb','F','C']       # 4 mesures

def bars_to_samples(n_bars):
    return int(SAMPLE_RATE * BAR * n_bars)

# Durée totale estimée
sections = {
    'intro':    4,
    'verse1':   8,
    'chorus1':  8,
    'verse2':   8,
    'pre':      8,
    'chorus2':  8,
    'bridge':   8,
    'chorus3':  8,
    'outro':    8,
}
total_bars = sum(sections.values())
total_samples = bars_to_samples(total_bars)

print(f"Durée totale : {total_bars} mesures = {total_bars*BAR:.1f}s = {total_bars*BAR/60:.1f} min")

song = np.zeros(total_samples)

# ─── Curseur de position ──────────────────────────────────────────────────────
pos = 0  # en samples

def write_section(prog, n_bars, with_piano_arp=True, with_piano_slow=False,
                  with_cello=True, cello_gritty=False,
                  with_bass=False, with_pad=True, pad_dissonant=False,
                  with_drums=False, drums_chaotic=False,
                  piano_amp=0.22, cello_amp=0.35, bass_amp=0.30, pad_amp=0.12):
    global pos
    for i, chord in enumerate(prog[:n_bars]):
        bar_samples = bars_to_samples(1)
        bar_start = pos

        # Piano arpège
        if with_piano_arp:
            p = piano_arpeggio(chord, BAR, amp=piano_amp)
            mix_at(song, p, bar_start)

        # Piano lent
        if with_piano_slow:
            p = piano_slow(chord, BAR, amp=piano_amp)
            mix_at(song, p, bar_start)

        # Violoncelle (note fondamentale de l'accord)
        if with_cello:
            root_f = CHORDS[chord][0] / 2  # une octave en dessous
            c = cello(root_f, BAR, amp=cello_amp, gritty=cello_gritty)
            mix_at(song, c, bar_start)

        # Basse
        if with_bass:
            root_f = CHORDS[chord][0] / 2
            b = bass_note(root_f, BAR * 0.9, amp=bass_amp)
            mix_at(song, b, bar_start)

        # Pad
        if with_pad:
            p = pad_chord(chord, BAR, amp=pad_amp, dissonant=pad_dissonant)
            mix_at(song, p, bar_start)

        # Batterie
        if with_drums:
            # Kick sur les temps 1 et 3
            for beat_i in [0, 2]:
                bp = bar_start + int(beat_i * BEAT * SAMPLE_RATE)
                k = kick()
                mix_at(song, k, bp)
            # Snare sur les temps 2 et 4
            for beat_i in [1, 3]:
                bp = bar_start + int(beat_i * BEAT * SAMPLE_RATE)
                s = snare()
                mix_at(song, s, bp)
            # Hihat sur les 8ths
            for beat_i in [0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5]:
                bp = bar_start + int(beat_i * BEAT * SAMPLE_RATE)
                h = hihat()
                mix_at(song, h, bp)

            if drums_chaotic:
                # Kicks supplémentaires irréguliers
                for beat_i in [0.25, 1.75, 2.25, 3.75]:
                    bp = bar_start + int(beat_i * BEAT * SAMPLE_RATE)
                    k = kick(amp=0.35)
                    mix_at(song, k, bp)

        pos += bar_samples

# ─── INTRO (4 mesures) ────────────────────────────────────────────────────────
# Piano seul, puis violoncelle entre
write_section(PROG_INTRO, 4,
              with_piano_arp=True, piano_amp=0.18,
              with_cello=True, cello_amp=0.20,
              with_bass=False, with_pad=True, pad_amp=0.07,
              with_drums=False)

# ─── COUPLET 1 (8 mesures) ────────────────────────────────────────────────────
write_section(PROG_VERSE, 8,
              with_piano_arp=True, piano_amp=0.22,
              with_cello=True, cello_amp=0.28,
              with_bass=False, with_pad=True, pad_amp=0.09,
              with_drums=False)

# ─── REFRAIN 1 (8 mesures) ────────────────────────────────────────────────────
write_section(PROG_CHORUS, 8,
              with_piano_arp=True, piano_amp=0.20,
              with_cello=True, cello_amp=0.32,
              with_bass=True, bass_amp=0.28,
              with_pad=True, pad_amp=0.11,
              with_drums=True)

# ─── COUPLET 2 (8 mesures) ────────────────────────────────────────────────────
# Redescend : piano + basse discrète
write_section(PROG_VERSE, 8,
              with_piano_arp=True, piano_amp=0.20,
              with_cello=True, cello_amp=0.22,
              with_bass=True, bass_amp=0.16,
              with_pad=True, pad_amp=0.08,
              with_drums=False)

# ─── PRÉ-REFRAIN (8 mesures) ──────────────────────────────────────────────────
# Violoncelle grinçant, tension monte
write_section(PROG_PRE, 8,
              with_piano_arp=True, piano_amp=0.21,
              with_cello=True, cello_gritty=True, cello_amp=0.30,
              with_bass=True, bass_amp=0.20,
              with_pad=True, pad_dissonant=True, pad_amp=0.10,
              with_drums=False)

# ─── REFRAIN 2 (8 mesures) ────────────────────────────────────────────────────
# Tous les instruments, massif et tragique
write_section(PROG_CHORUS, 8,
              with_piano_arp=True, piano_amp=0.22,
              with_cello=True, cello_amp=0.38,
              with_bass=True, bass_amp=0.32,
              with_pad=True, pad_dissonant=True, pad_amp=0.13,
              with_drums=True)

# ─── PONT (8 mesures) ─────────────────────────────────────────────────────────
# Rythme chaotique, dissonance, sons superposés
write_section(PROG_BRIDGE, 8,
              with_piano_arp=True, piano_amp=0.19,
              with_cello=True, cello_gritty=True, cello_amp=0.35,
              with_bass=True, bass_amp=0.28,
              with_pad=True, pad_dissonant=True, pad_amp=0.12,
              with_drums=True, drums_chaotic=True)

# ─── REFRAIN FINAL (8 mesures) ────────────────────────────────────────────────
write_section(PROG_CHORUS, 8,
              with_piano_arp=True, piano_amp=0.22,
              with_cello=True, cello_amp=0.38,
              with_bass=True, bass_amp=0.32,
              with_pad=True, pad_dissonant=False, pad_amp=0.13,
              with_drums=True)

# ─── OUTRO (8 mesures) ────────────────────────────────────────────────────────
# Tempête s'arrête : piano lent hésitant + violoncelle qui s'éteint
write_section(PROG_OUTRO, 8,
              with_piano_arp=False, with_piano_slow=True, piano_amp=0.25,
              with_cello=True, cello_amp=0.28,
              with_bass=False,
              with_pad=True, pad_amp=0.06,
              with_drums=False)

# ─── Fade out final (dernières 8 mesures) ─────────────────────────────────────
fade_start = bars_to_samples(total_bars - 8)
fade_len = bars_to_samples(8)
fade_env = np.linspace(1, 0, fade_len)
song[fade_start:fade_start+fade_len] *= fade_env

# ─── Normalisation ────────────────────────────────────────────────────────────
peak = np.max(np.abs(song))
if peak > 0:
    song = song / peak * 0.92

# ─── Export WAV ───────────────────────────────────────────────────────────────
output_path = "/home/user/la_guerre_en_moi.wav"
song_int = (song * 32767).astype(np.int16)

with wave.open(output_path, 'w') as wf:
    wf.setnchannels(1)
    wf.setsampwidth(2)
    wf.setframerate(SAMPLE_RATE)
    wf.writeframes(song_int.tobytes())

print(f"✅ Fichier sauvegardé : {output_path}")
print(f"⏱️  Durée : {len(song)/SAMPLE_RATE:.1f}s ({len(song)/SAMPLE_RATE/60:.2f} min)")

