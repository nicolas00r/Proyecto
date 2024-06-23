#ifndef FUNCIONES_H
#define FUNCIONES_H

typedef struct TipoCasilla TipoCasilla;
typedef struct TipoJugador TipoJugador;
typedef struct partidaGlobal partidaGlobal;

// Declaraciones de funciones
void presioneEnter();
void limpiar_pantalla();
int solicitar_jugadores();
void asignar_jugadores(partidaGlobal *partida, int num_jugadores);
void inicializar_aleatoriedad();
int tirar_dados();
TipoCasilla* inicializar_casillas();
void comprar_propiedad(TipoJugador* jugador, TipoCasilla* propiedad, partidaGlobal* partida);
void comprar_casas(TipoJugador* jugador, TipoCasilla* propiedad);
void mostrar_reglas();
void guardar_partida(partidaGlobal *partida, const char *filename);
void mostrarMenuInicial();
void iniciarPartida();

#endif // FUNCIONES_H