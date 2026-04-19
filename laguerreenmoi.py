
import numpy as np
import wave

SAMPLE_RATE = 44100
BPM = 70
BEAT = 60.0 / BPM
BAR  = BEAT * 4

NOTE_NAMES = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B']

def freq(note, octave):
    n = NOTE_NAMES.index(note)
    midi = (octave + 1) * 12 + n
    return 440.0 * (2 ** ((midi - 69) / 12.0))

CHORDS = {
    'Dm': [freq('D',4), freq('F',4), freq('A',4)],
    'Bb': [freq('A#',3), freq('D',4), freq('F',4)],
    'F':  [freq('F',3), freq('A',3), freq('C',4)],
    'C':  [freq('C',4), freq('E',4), freq('G',4)],
    'Gm': [freq('G',3), freq('A#',3), freq('D',4)],
    'A':  [freq('A',3), freq('C#',4), freq('E',4)],
}

def sine(f, dur, amp=0.5):
    n = max(1, int(SAMPLE_RATE * dur))
    t = np.linspace(0, dur, n, False)
    return amp * np.sin(2*np.pi*f*t)

def sawtooth(f, dur, amp=0.3):
    n = max(1, int(SAMPLE_RATE * dur))
    t = np.linspace(0, dur, n, False)
    return amp * (2*(t*f - np.floor(t*f + 0.5)))

def silence(dur):
    return np.zeros(max(1, int(SAMPLE_RATE * dur)))

def adsr(sig, a=0.05, d=0.1, s_level=0.7, r=0.15):
    n = len(sig)
    if n == 0:
        return sig
    na = max(1, min(int(a * SAMPLE_RATE), n))
    nd = max(1, min(int(d * SAMPLE_RATE), n - na))
    nr = max(1, min(int(r * SAMPLE_RATE), n - na - nd))
    ns = max(0, n - na - nd - nr)
    env = np.concatenate([
        np.linspace(0, 1, na),
        np.linspace(1, s_level, nd),
        np.full(ns, s_level),
        np.linspace(s_level, 0, nr)
    ])
    env = env[:n]
    if len(env) < n:
        env = np.concatenate([env, np.zeros(n - len(env))])
    return sig * env

def mix_at(buf, sig, pos):
    end = pos + len(sig)
    if pos >= len(buf):
        return
    if end > len(buf):
        sig = sig[:len(buf) - pos]
    buf[pos:pos + len(sig)] += sig

# ── Instruments ──────────────────────────────────────────────────────────────

def piano_arpeggio(chord_name, dur, amp=0.22):
    notes = CHORDS[chord_name]
    notes_ext = [n / 2 for n in notes] + notes + [n * 2 for n in notes[:2]]
    note_dur = BEAT / 3
    buf = []
    t = 0.0
    idx = 0
    while t < dur - 0.001:
        nd = min(note_dur, dur - t)
        if nd < 0.005:
            break
        s = sine(notes_ext[idx % len(notes_ext)], nd, amp)
        s = adsr(s, a=0.01, d=0.04, s_level=0.6, r=0.08)
        buf.append(s)
        t += note_dur
        idx += 1
    if not buf:
        return silence(dur)
    return np.concatenate(buf)[:int(SAMPLE_RATE * dur)]

def piano_slow(chord_name, dur, amp=0.28):
    notes = CHORDS[chord_name]
    note_dur = BEAT
    buf = []
    t = 0.0
    idx = 0
    while t < dur - 0.001:
        nd = min(note_dur * 0.85, dur - t)
        if nd < 0.01:
            break
        s = sine(notes[idx % len(notes)], nd, amp)
        s = adsr(s, a=0.03, d=0.1, s_level=0.65, r=0.2)
        buf.append(s)
        gap = min(note_dur * 0.15, dur - t - nd)
        if gap > 0:
            buf.append(silence(gap))
        t += note_dur
        idx += 1
    if not buf:
        return silence(dur)
    return np.concatenate(buf)[:int(SAMPLE_RATE * dur)]

def cello_note(f, dur, amp=0.35, gritty=False):
    n = max(1, int(SAMPLE_RATE * dur))
    t = np.linspace(0, dur, n, False)
    sig = amp * (
        0.6 * np.sin(2*np.pi*f*t) +
        0.25 * np.sin(2*np.pi*f*2*t) +
        0.10 * np.sin(2*np.pi*f*3*t) +
        0.05 * np.sin(2*np.pi*f*4*t)
    )
    if gritty:
        sig += 0.04 * np.sign(sig) * np.abs(sig)**0.5
    vibrato = 1 + 0.008 * np.sin(2*np.pi*4.5*t)
    sig = sig * vibrato
    return adsr(sig, a=0.3, d=0.2, s_level=0.8, r=0.4)

def bass_note(f, dur, amp=0.30):
    sig = sawtooth(f / 2, dur, amp)
    return adsr(sig, a=0.02, d=0.08, s_level=0.7, r=0.1)

def pad_chord(chord_name, dur, amp=0.12, dissonant=False):
    n = max(1, int(SAMPLE_RATE * dur))
    t = np.linspace(0, dur, n, False)
    sig = np.zeros(n)
    for i, nf in enumerate(CHORDS[chord_name]):
        wave_ = amp * np.sin(2*np.pi*nf*t + i*0.15)
        if dissonant:
            wave_ += (amp * 0.3) * np.sin(2*np.pi*nf*1.004*t)
        sig += wave_
    return adsr(sig, a=0.4, d=0.3, s_level=0.6, r=0.5)

def kick(amp=0.55):
    dur = 0.25
    n = int(SAMPLE_RATE * dur)
    t = np.linspace(0, dur, n, False)
    f_env = 180 * np.exp(-30*t)
    sig = amp * np.sin(2*np.pi*f_env*t)
    return adsr(sig, a=0.002, d=0.08, s_level=0.0, r=0.05)

def snare(amp=0.35):
    dur = 0.18
    n = int(SAMPLE_RATE * dur)
    t = np.linspace(0, dur, n, False)
    noise = np.random.randn(n) * amp * np.exp(-18*t)
    tone  = 0.15 * np.sin(2*np.pi*200*t) * np.exp(-25*t)
    return noise + tone

def hihat(amp=0.15):
    dur = 0.08
    n = int(SAMPLE_RATE * dur)
    t = np.linspace(0, dur, n, False)
    return np.random.randn(n) * amp * np.exp(-40*t)

# ── Construction ─────────────────────────────────────────────────────────────

PROG_INTRO  = ['Dm','Bb','F','C']
PROG_VERSE  = ['Dm','Bb','F','C','Dm','Bb','Gm','C']
PROG_CHORUS = ['Bb','F','C','Dm','Bb','F','Gm','C']
PROG_PRE    = ['Bb','F','C','Dm','Bb','F','Gm','C']
PROG_BRIDGE = ['Gm','Dm','Bb','F','C','Dm','Bb','A']
PROG_OUTRO  = ['Dm','Bb','F','C','Gm','Bb','C','Dm']

sections_bars = [4, 8, 8, 8, 8, 8, 8, 8, 8]
total_bars = sum(sections_bars)
total_samples = int(SAMPLE_RATE * BAR * total_bars)
song = np.zeros(total_samples)

print(f"Durée : {total_bars * BAR:.1f}s = {total_bars * BAR / 60:.2f} min")

pos = 0

def write_section(prog, n_bars,
                  piano_arp=False, piano_slow_mode=False,
                  cello=True, cello_gritty=False,
                  bass=False, pad=True, pad_dis=False,
                  drums=False, drums_chaos=False,
                  p_amp=0.22, c_amp=0.35, b_amp=0.30, pad_amp=0.12):
    global pos
    bar_samples = int(SAMPLE_RATE * BAR)
    for i in range(n_bars):
        chord = prog[i % len(prog)]
        bs = pos

        if piano_arp:
            mix_at(song, piano_arpeggio(chord, BAR, p_amp), bs)
        if piano_slow_mode:
            mix_at(song, piano_slow(chord, BAR, p_amp), bs)
        if cello:
            rf = CHORDS[chord][0] / 2
            mix_at(song, cello_note(rf, BAR, c_amp, cello_gritty), bs)
        if bass:
            rf = CHORDS[chord][0] / 2
            mix_at(song, bass_note(rf, BAR * 0.9, b_amp), bs)
        if pad:
            mix_at(song, pad_chord(chord, BAR, pad_amp, pad_dis), bs)
        if drums:
            for beat_i in [0, 2]:
                mix_at(song, kick(), bs + int(beat_i * BEAT * SAMPLE_RATE))
            for beat_i in [1, 3]:
                mix_at(song, snare(), bs + int(beat_i * BEAT * SAMPLE_RATE))
            for beat_i in [0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5]:
                mix_at(song, hihat(), bs + int(beat_i * BEAT * SAMPLE_RATE))
            if drums_chaos:
                for beat_i in [0.25, 1.75, 2.25, 3.75]:
                    mix_at(song, kick(0.30), bs + int(beat_i * BEAT * SAMPLE_RATE))

        pos += bar_samples

# INTRO
write_section(PROG_INTRO, 4, piano_arp=True, cello=True, pad=True,
              p_amp=0.18, c_amp=0.18, pad_amp=0.07)
# COUPLET 1
write_section(PROG_VERSE, 8, piano_arp=True, cello=True, pad=True,
              p_amp=0.22, c_amp=0.28, pad_amp=0.09)
# REFRAIN 1
write_section(PROG_CHORUS, 8, piano_arp=True, cello=True, bass=True, pad=True, drums=True,
              p_amp=0.20, c_amp=0.32, b_amp=0.28, pad_amp=0.11)
# COUPLET 2
write_section(PROG_VERSE, 8, piano_arp=True, cello=True, bass=True, pad=True,
              p_amp=0.20, c_amp=0.22, b_amp=0.16, pad_amp=0.08)
# PRÉ-REFRAIN
write_section(PROG_PRE, 8, piano_arp=True, cello=True, cello_gritty=True, bass=True,
              pad=True, pad_dis=True,
              p_amp=0.21, c_amp=0.30, b_amp=0.20, pad_amp=0.10)
# REFRAIN 2
write_section(PROG_CHORUS, 8, piano_arp=True, cello=True, bass=True, pad=True,
              pad_dis=True, drums=True,
              p_amp=0.22, c_amp=0.38, b_amp=0.32, pad_amp=0.13)
# PONT
write_section(PROG_BRIDGE, 8, piano_arp=True, cello=True, cello_gritty=True,
              bass=True, pad=True, pad_dis=True, drums=True, drums_chaos=True,
              p_amp=0.19, c_amp=0.35, b_amp=0.28, pad_amp=0.12)
# REFRAIN FINAL
write_section(PROG_CHORUS, 8, piano_arp=True, cello=True, bass=True, pad=True,
              drums=True,
              p_amp=0.22, c_amp=0.38, b_amp=0.32, pad_amp=0.13)
# OUTRO
write_section(PROG_OUTRO, 8, piano_slow_mode=True, cello=True, pad=True,
              p_amp=0.25, c_amp=0.28, pad_amp=0.06)

# Fade out sur l'outro
fade_start = int(SAMPLE_RATE * BAR * (total_bars - 8))
fade_len   = int(SAMPLE_RATE * BAR * 8)
song[fade_start:fade_start + fade_len] *= np.linspace(1, 0, fade_len)

# Normalisation
peak = np.max(np.abs(song))
if peak > 0:
    song = song / peak * 0.90

# Export WAV
out = "la_guerre_en_moi.wav"
song_int = (song * 32767).astype(np.int16)
with wave.open(out, 'w') as wf:
    wf.setnchannels(1)
    wf.setsampwidth(2)
    wf.setframerate(SAMPLE_RATE)
    wf.writeframes(song_int.tobytes())

print(f"✅ Sauvegardé : {out}")
print(f"⏱️  Durée : {len(song)/SAMPLE_RATE:.1f}s ({len(song)/SAMPLE_RATE/60:.2f} min)")

