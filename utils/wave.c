#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "wave.h"

// Structure du header WAV
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t size;          // Taille du fichier - 8
    char wave[4];           // "WAVE"
} RiffHeader;

typedef struct {
    char fmt[4];            // "fmt "
    uint32_t subchunk1_size; // 16 pour PCM
    uint16_t audio_format;   // 1 pour PCM
    uint16_t num_channels;   // 1 ou 2
    uint32_t sample_rate;    // Fréquence d'échantillonnage
    uint32_t byte_rate;      // SampleRate * NumChannels * BitsPerSample/8
    uint16_t block_align;    // NumChannels * BitsPerSample/8
    uint16_t bits_per_sample; // 16 ou 24
} FmtSubchunk;

typedef struct {
    char data[4];           // "data"
    uint32_t subchunk2_size; // Taille des données audio
} DataSubchunk;

// Créer un fichier WAV
WaveFile *wave_create(const char *filename, int sample_rate, int num_channels, int bits_per_sample) {
    if (!filename || sample_rate <= 0 || num_channels <= 0) {
        fprintf(stderr, "[ERREUR] Paramètres WAV invalides\n");
        return NULL;
    }
    
    WaveFile *wave = (WaveFile *)malloc(sizeof(WaveFile));
    if (!wave) return NULL;
    
    wave->file = fopen(filename, "wb");
    if (!wave->file) {
        fprintf(stderr, "[ERREUR] Impossible d'ouvrir le fichier %s\n", filename);
        free(wave);
        return NULL;
    }
    
    wave->sample_rate = sample_rate;
    wave->num_channels = num_channels;
    wave->bits_per_sample = bits_per_sample;
    wave->num_samples = 0;
    
    // Réserver de l'espace pour les headers (seront mis à jour plus tard)
    RiffHeader riff = {
        .riff = {'R', 'I', 'F', 'F'},
        .size = 0, // Sera mis à jour
        .wave = {'W', 'A', 'V', 'E'}
    };
    
    fwrite(&riff, sizeof(RiffHeader), 1, wave->file);
    
    // Écrire le subchunk "fmt"
    FmtSubchunk fmt = {
        .fmt = {'f', 'm', 't', ' '},
        .subchunk1_size = 16,
        .audio_format = 1, // PCM
        .num_channels = num_channels,
        .sample_rate = sample_rate,
        .byte_rate = sample_rate * num_channels * (bits_per_sample / 8),
        .block_align = num_channels * (bits_per_sample / 8),
        .bits_per_sample = bits_per_sample
    };
    
    fwrite(&fmt, sizeof(FmtSubchunk), 1, wave->file);
    
    // Écrire le header du subchunk "data" (la taille sera mise à jour)
    DataSubchunk data = {
        .data = {'d', 'a', 't', 'a'},
        .subchunk2_size = 0 // Sera mis à jour
    };
    
    wave->data_position = ftell(wave->file);
    fwrite(&data, sizeof(DataSubchunk), 1, wave->file);
    
    return wave;
}

// Écrire des échantillons dans le fichier WAV
int wave_write_samples(WaveFile *wave, float *samples, int num_samples) {
    if (!wave || !wave->file || !samples || num_samples <= 0) {
        return -1;
    }
    
    int bytes_per_sample = wave->bits_per_sample / 8;
    
    for (int i = 0; i < num_samples; i++) {
        // Convertir de float (-1.0 à 1.0) en entier signé
        float sample = samples[i];
        
        // Clipping pour éviter le débordement
        if (sample > 1.0f) sample = 1.0f;
        if (sample < -1.0f) sample = -1.0f;
        
        if (wave->bits_per_sample == 16) {
            int16_t sample_int = (int16_t)(sample * 32767);
            fwrite(&sample_int, sizeof(int16_t), 1, wave->file);
        } else if (wave->bits_per_sample == 24) {
            int32_t sample_int = (int32_t)(sample * 8388607);
            // Écrire 24 bits en petit-endian
            uint8_t bytes[3];
            bytes[0] = sample_int & 0xFF;
            bytes[1] = (sample_int >> 8) & 0xFF;
            bytes[2] = (sample_int >> 16) & 0xFF;
            fwrite(bytes, 3, 1, wave->file);
        }
    }
    
    wave->num_samples += num_samples;
    return 0;
}

// Finir d'écrire et fermer le fichier WAV
int wave_close(WaveFile *wave) {
    if (!wave || !wave->file) {
        return -1;
    }
    
    // Mettre à jour la taille du subchunk "data"
    uint32_t data_size = wave->num_samples * (wave->bits_per_sample / 8) * wave->num_channels;
    
    fseek(wave->file, wave->data_position + 4, SEEK_SET);
    fwrite(&data_size, sizeof(uint32_t), 1, wave->file);
    
    // Mettre à jour la taille totale du fichier RIFF
    uint32_t file_size = 36 + data_size;
    
    fseek(wave->file, 4, SEEK_SET);
    fwrite(&file_size, sizeof(uint32_t), 1, wave->file);
    
    fclose(wave->file);
    free(wave);
    
    return 0;
}

// Détruire le fichier WAV sans le sauvegarder
void wave_destroy(WaveFile *wave) {
    if (!wave) return;
    
    if (wave->file) {
        fclose(wave->file);
    }
    
    free(wave);
}

// Lire un fichier WAV
WaveFileReader *wave_read_file(const char *filename) {
    if (!filename) return NULL;
    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "[ERREUR] Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }
    
    WaveFileReader *reader = (WaveFileReader *)malloc(sizeof(WaveFileReader));
    if (!reader) {
        fclose(file);
        return NULL;
    }
    
    // Lire le header RIFF
    RiffHeader riff;
    if (fread(&riff, sizeof(RiffHeader), 1, file) != 1) {
        fprintf(stderr, "[ERREUR] Erreur de lecture du header RIFF\n");
        fclose(file);
        free(reader);
        return NULL;
    }
    
    // Vérifier la signature
    if (strncmp(riff.riff, "RIFF", 4) != 0 || strncmp(riff.wave, "WAVE", 4) != 0) {
        fprintf(stderr, "[ERREUR] Format WAV invalide\n");
        fclose(file);
        free(reader);
        return NULL;
    }
    
    // Lire le subchunk "fmt"
    FmtSubchunk fmt;
    if (fread(&fmt, sizeof(FmtSubchunk), 1, file) != 1) {
        fprintf(stderr, "[ERREUR] Erreur de lecture du subchunk fmt\n");
        fclose(file);
        free(reader);
        return NULL;
    }
    
    reader->sample_rate = fmt.sample_rate;
    reader->num_channels = fmt.num_channels;
    reader->bits_per_sample = fmt.bits_per_sample;
    
    // Trouver et lire le subchunk "data"
    DataSubchunk data;
    while (fread(&data, sizeof(DataSubchunk), 1, file) == 1) {
        if (strncmp(data.data, "data", 4) == 0) {
            reader->num_samples = data.subchunk2_size / (fmt.bits_per_sample / 8) / fmt.num_channels;
            reader->audio_data = (float *)malloc(sizeof(float) * reader->num_samples);
            
            if (!reader->audio_data) {
                fclose(file);
                free(reader);
                return NULL;
            }
            
            // Lire les données audio
            for (int i = 0; i < reader->num_samples; i++) {
                if (fmt.bits_per_sample == 16) {
                    int16_t sample_int;
                    if (fread(&sample_int, sizeof(int16_t), 1, file) == 1) {
                        reader->audio_data[i] = (float)sample_int / 32768.0f;
                    }
                }
            }
            
            break;
        }
    }
    
    fclose(file);
    reader->file = NULL;
    
    return reader;
}

// Détruire le lecteur WAV
void wave_reader_destroy(WaveFileReader *reader) {
    if (!reader) return;
    
    if (reader->audio_data) {
        free(reader->audio_data);
    }
    
    free(reader);
}
