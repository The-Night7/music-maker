#ifndef INTERFACE_H
#define INTERFACE_H

// Déclaration forward - évite les conflits si audio_engine.h est inclus
struct AudioEngine;

// Fonction principale de l'interface
void ui_run(struct AudioEngine *engine);

#endif // INTERFACE_H
