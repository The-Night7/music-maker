#ifndef WAVE_H
#define WAVE_H

#include <stdio.h>
#include <stdint.h>

// Structure pour l'écriture de fichiers WAV
typedef struct {
    FILE *file;
    int sample_rate;
    int num_channels;
    int bits_per_sample;
    int num_samples;
    long data_position;
} WaveFile;

// Structure pour la lecture de fichiers WAV
typedef struct {
    FILE *file;
    int sample_rate;
    int num_channels;
    int bits_per_sample;
    int num_samples;
    float *audio_data;
} WaveFileReader;

// Fonctions d'écriture WAV
WaveFile *wave_create(const char *filename, int sample_rate, int num_channels, int bits_per_sample);
int wave_write_samples(WaveFile *wave, float *samples, int num_samples);
int wave_close(WaveFile *wave);
void wave_destroy(WaveFile *wave);

// Fonctions de lecture WAV
WaveFileReader *wave_read_file(const char *filename);
void wave_reader_destroy(WaveFileReader *reader);

#endif // WAVE_H
