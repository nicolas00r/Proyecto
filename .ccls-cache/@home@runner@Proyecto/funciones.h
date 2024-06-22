#ifndef FUNCIONES_H
#define FUNCIONES_H

typedef struct TipoPropiedad TipoPropiedad;
typedef struct TipoJugador TipoJugador;
typedef struct partidaGlobal partidaGlobal;

// Declaraciones de funciones
void presioneEnter();
void limpiar_pantalla();
int solicitar_jugadores();
void asignar_jugadores(partidaGlobal *partida, int num_jugadores);
void inicializar_aleatoriedad();
int tirar_dados();
TipoPropiedad* inicializar_propiedades();
TipoPropiedad* buscar_propiedad_por_nombre(TipoPropiedad* propiedades, const char* nombre);
void reinicializar_propiedades(TipoPropiedad* propiedades);
void comprar_propiedad(TipoJugador* jugador, TipoPropiedad* propiedad, partidaGlobal* partida);
void comprar_casas(TipoJugador* jugador, TipoPropiedad* propiedad);
void mostrar_reglas();
void guardar_partida(partidaGlobal *partida, const char *filename);
void mostrarMenuInicial();
void iniciarPartida();

#endif // FUNCIONES_H