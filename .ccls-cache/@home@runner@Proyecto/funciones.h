#ifndef FUNCIONES_H
#define FUNCIONES_H

typedef struct TipoPropiedad TipoPropiedad;
typedef struct TipoJugador TipoJugador;
typedef struct partidaGlobal partidaGlobal;

// Declaraciones de funciones
void presioneEnter();
void limpiar_pantalla();
void inicializar_aleatoriedad();
int tirar_dados();
TipoPropiedad* inicializar_propiedades();
TipoPropiedad* buscar_propiedad_por_nombre(TipoPropiedad* propiedades, const char* nombre);
void reinicializar_propiedades(TipoPropiedad* propiedades);
void comprar_propiedad(TipoJugador* jugador, TipoPropiedad* propiedad);
void comprar_casas(TipoJugador* jugador, TipoPropiedad* propiedad);
void testear_funciones();
void mostrar_reglas();
void guardar_partida(partidaGlobal *partida, const char *filename);
void mostrarMenuInicial();

#endif // FUNCIONES_H