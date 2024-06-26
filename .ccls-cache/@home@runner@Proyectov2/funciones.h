#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdbool.h>

typedef struct TipoCasilla TipoCasilla;
typedef struct TipoCarta TipoCarta;
typedef struct TipoJugador TipoJugador;
typedef struct PartidaGlobal PartidaGlobal;
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
void inicializarTablero(PartidaGlobal *partida, TipoCasilla *casillas);
void presioneEnter();
void limpiar_pantalla();
void inicializar_aleatoriedad();
int tirar_dado();
int solicitar_jugadores();
void asignar_jugadores(PartidaGlobal *partida, int num_jugadores);
void hipotecar_propiedad(TipoJugador *jugador, TipoCasilla *propiedad);
void recuperar_propiedad_hipotecada(TipoJugador *jugador, TipoCasilla *propiedad);
bool verificarRecaudacionMaxima(List *propiedades, int deuda);
void menuHipotecasYDeudas(TipoJugador *jugador, int deuda);
bool verificar_propiedades_bancarrota(TipoJugador *jugador, int deuda);
void eliminarJugador(TipoJugador **jugador, PartidaGlobal *partida);
void verificar_bancarrota(TipoJugador *jugador, int dinero_a_pagar, PartidaGlobal *partida);
void comprar_casas(TipoJugador* jugador, TipoCasilla* propiedad);
void vender_casas(TipoJugador *jugador, TipoCasilla *propiedad);
void inicializarCamposNecesarios(PartidaGlobal *partida);
void mostrarTipo(char tipo);
void mostrar_estado_jugador(TipoJugador *jugador, PartidaGlobal *partida);
void moverJugador(TipoJugador *jugador, int avance);
void menu_de_propiedades(TipoJugador *jugador);
void casoPropiedad(TipoJugador *jugador, TipoCasilla* propiedad, PartidaGlobal *partida);
void casoCarta(TipoJugador *jugador, PartidaGlobal *partida, TipoCasilla *casilla, int dados);
void casoImpuestos(TipoJugador *jugador, PartidaGlobal *partida, TipoCasilla *casilla);
void casoMetro(TipoJugador *jugador, TipoCasilla* propiedad, PartidaGlobal *partida);
void casoCarcel(TipoJugador *jugador, PartidaGlobal *partida);
void ejecutarAccionCasilla(TipoJugador *jugador, TipoCasilla *casilla, PartidaGlobal *partida, int dados);
void turnoJugador(TipoJugador** jugador, PartidaGlobal *partida);
TipoJugador *verificarGanador(List *jugadores);
void mostrar_reglas();
void mostrarMenuInicial();
void iniciarPartida();

#endif // FUNCIONES_H