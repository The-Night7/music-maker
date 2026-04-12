#ifndef INTERFACE_H
#define INTERFACE_H

// Forward declaration uniquement - la vraie déclaration est dans audio_engine.h
typedef struct AudioEngine AudioEngine;

// Fonction principale de l'interface
void ui_run(AudioEngine *engine);

#endif // INTERFACE_H
