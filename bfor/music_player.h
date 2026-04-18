#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include "parser.h"
#include "audio/audio_engine.h"
#include "instruments/piano.h"
#include "instruments/drums.h"
#include "instruments/guitar.h"

// État du lecteur
typedef enum {
    PLAYER_STOPPED,
    PLAYER_PLAYING,
    PLAYER_PAUSED
} PlayerState;

// Structure du lecteur musical
typedef struct {
    Composition *composition;
    
    Piano *piano;
    Drums *drums;
    Guitar *guitar;
    
    PlayerState state;
    int current_note_index[3];      // Index pour chaque instrument
    float elapsed_time;              // Temps écoulé en secondes
    float note_start_time;           // Temps de début de la note actuelle
    float current_note_duration;     // Durée de la note actuelle
    int active_instruments[3];       // Instruments actifs
    
    AudioEngine *engine;
} MusicPlayer;

// Fonctions du lecteur
MusicPlayer *music_player_create(Composition *comp, AudioEngine *engine);
void music_player_destroy(MusicPlayer *player);
int music_player_start(MusicPlayer *player);
int music_player_stop(MusicPlayer *player);
int music_player_pause(MusicPlayer *player);
int music_player_resume(MusicPlayer *player);
void music_player_update(MusicPlayer *player, float delta_time);
int music_player_is_finished(MusicPlayer *player);
void music_player_print_status(MusicPlayer *player);

#endif // MUSIC_PLAYER_H
