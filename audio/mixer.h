#ifndef MIXER_H
#define MIXER_H

#include <stdint.h>

// Structure pour une piste du mixeur
typedef struct {
    float volume;       // 0.0 à 1.0
    float pan;          // -1.0 (gauche) à +1.0 (droite), 0.0 = centre
    int muted;          // 1 = muette, 0 = active
    int solo;           // 1 = solo, 0 = normal
    void *instrument;   // Pointeur vers l'instrument
    float *buffer;      // Buffer audio de la piste
} MixerTrack;

// Structure du mixeur
typedef struct {
    int num_tracks;
    int buffer_size;
    int sample_rate;
    float master_volume;
    MixerTrack *tracks;
    float *output_buffer;
} Mixer;

// Fonctions du mixeur
Mixer *mixer_create(int num_tracks, int buffer_size, int sample_rate);
void mixer_destroy(Mixer *mixer);
int mixer_add_instrument(Mixer *mixer, int track_index, void *instrument);
void mixer_set_track_volume(Mixer *mixer, int track_index, float volume);
void mixer_set_track_pan(Mixer *mixer, int track_index, float pan);
void mixer_mute_track(Mixer *mixer, int track_index, int mute);
void mixer_solo_track(Mixer *mixer, int track_index, int solo);
void mixer_set_master_volume(Mixer *mixer, float volume);
void mixer_process(Mixer *mixer);
float *mixer_get_output(Mixer *mixer);
int mixer_get_num_tracks(Mixer *mixer);
float mixer_get_track_volume(Mixer *mixer, int track_index);
void mixer_print_status(Mixer *mixer);

#endif // MIXER_H
