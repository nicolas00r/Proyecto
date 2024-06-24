#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdbool.h>

typedef struct TipoCasilla TipoCasilla;
typedef struct TipoCarta TipoCarta;
typedef struct TipoJugador TipoJugador;
typedef struct partidaGlobal partidaGlobal;
typedef struct List List;
typedef struct List Queue;

// Declaraciones de funciones
TipoCarta *fortuna();
TipoCarta *arca_comunal();
void randomizar_cartas(TipoCarta *cartas, int size);
TipoCarta* crear_copia_cartas(TipoCarta *cartas, int size);
void arreglo_a_cola(TipoCarta *cartas, int size, Queue *cola);
Queue *inicializar_fortuna();
Queue *inicializar_arca_comunal();
TipoCasilla* inicializar_casillas();
void inicializarTablero(partidaGlobal *partida, TipoCasilla *casillas);
void presioneEnter();
void limpiar_pantalla();
void inicializar_aleatoriedad();
int tirar_dado();
int solicitar_jugadores();
void asignar_jugadores(partidaGlobal *partida, int num_jugadores);
void hipotecar_propiedad(TipoJugador *jugador, TipoCasilla *propiedad);
void recuperar_propiedad_hipotecada(TipoJugador *jugador, TipoCasilla *propiedad);
bool verificarRecaudacionMaxima(List *propiedades, int deuda);
void menuHipotecasYDeudas(TipoJugador *jugador, int deuda);
bool verificar_propiedades_bancarrota(TipoJugador *jugador, int deuda);
void eliminarJugador(TipoJugador **jugador, partidaGlobal *partida);
void verificar_bancarrota(TipoJugador *jugador, int dinero_a_pagar, partidaGlobal *partida);
void comprar_casas(TipoJugador* jugador, TipoCasilla* propiedad);
void vender_casas(TipoJugador *jugador, TipoCasilla *propiedad);
void inicializarCamposNecesarios(partidaGlobal *partida);
void mostrarTipo(char tipo);
void mostrar_estado_jugador(TipoJugador *jugador, partidaGlobal *partida);
void moverJugador(TipoJugador *jugador, int avance);
void menu_de_propiedades(TipoJugador *jugador, TipoCasilla *propiedad);
void casoPropiedad(TipoJugador *jugador, TipoCasilla* propiedad, partidaGlobal *partida);
void casoCarta(TipoJugador *jugador, partidaGlobal *partida, TipoCasilla *casilla);
void casoImpuestos(TipoJugador *jugador, partidaGlobal *partida, TipoCasilla *casilla);
void casoMetroOServicio(TipoJugador *jugador, TipoCasilla* propiedad, partidaGlobal *partida);
void casoCarcel(TipoJugador *jugador, partidaGlobal *partida);
void ejecutarAccionCasilla(TipoJugador *jugador, TipoCasilla *casilla, partidaGlobal *partida);
void turnoJugador(TipoJugador** jugador, partidaGlobal *partida);
TipoJugador *verificarGanador(List *jugadores);
void mostrar_reglas();
void mostrarMenuInicial();
void iniciarPartida();

#endif // FUNCIONES_H