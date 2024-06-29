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
Queue* inicializar_fortuna();
Queue* inicializar_arca_comunal();
TipoCasilla *inicializar_casillas();
void inicializarTablero(PartidaGlobal *partida, TipoCasilla *casillas);
void presioneEnter();
void limpiar_pantalla();
void inicializar_aleatoriedad();
int tirar_dado();
void inicializar_contador(TipoJugador *jugador);
int solicitar_jugadores();
void asignar_jugadores(PartidaGlobal *partida, int num_jugadores);
void hipotecar_propiedad(TipoJugador *jugador, TipoCasilla *propiedad);
void recuperar_propiedad_hipotecada(TipoJugador *jugador, TipoCasilla *propiedad);
bool verificarRecaudacionMaxima(List *propiedades, int deuda);
void menuHipotecasYDeudas(TipoJugador *jugador, int deuda);
bool verificar_propiedades_bancarrota(TipoJugador *jugador, int deuda);
void eliminarJugador(TipoJugador **jugador, PartidaGlobal *partida);
void verificar_bancarrota(TipoJugador *jugador, int dinero_a_pagar, PartidaGlobal *partida);
bool es_propietario_de_sector(TipoJugador *jugador, TipoCasilla *propiedad);
void comprar_casas(TipoJugador *jugador, TipoCasilla *propiedad);
void vender_casas(TipoJugador *jugador, TipoCasilla *propiedad);
void inicializarCamposNecesarios(PartidaGlobal *partida);
void mostrarTipo(char tipo);
void mostrar_estado_jugador(TipoJugador *jugador, PartidaGlobal *partida);
void moverJugador(TipoJugador *jugador, int avance);
void imprimirDetallesPropiedad(TipoCasilla* propiedad);
void rellenar_lista_turnos_subasta(List *turnos_subasta, List *jugadores);
void mostrarDetalles(TipoCasilla *propiedad);
void subasta_de_propiedades(TipoCasilla *propiedad, PartidaGlobal *partida);
void casoPropiedad(TipoJugador *jugador, TipoCasilla* propiedad, PartidaGlobal *partida);
void imprimirDetallesMetro(TipoCasilla* propiedad);
void casoMetro(TipoJugador *jugador, TipoCasilla* propiedad, PartidaGlobal *partida);
void imprimirDetallesServicio(TipoCasilla* propiedad);
void casoServicio(TipoJugador* jugador, TipoCasilla* propiedad, PartidaGlobal* partida, int dados);
void casoCarta(TipoJugador *jugador, PartidaGlobal *partida, TipoCasilla *casilla, int dados);
void casoImpuestos(TipoJugador *jugador, PartidaGlobal *partida, TipoCasilla *casilla);
void casoCarcel(TipoJugador *jugador, PartidaGlobal *partida);
void ejecutarAccionCasilla(TipoJugador *jugador, TipoCasilla *casilla, PartidaGlobal *partida, int dados);
void mostrarMenuDeTurno();
void mostrar_opciones_menu_propiedades(TipoCasilla *propiedad);
TipoCasilla *elegir_propiedad(TipoJugador *jugador);
void menu_de_propiedades(TipoJugador *jugador);
void rellenar_lista_opciones_intercambio(List *opciones, List *jugadores);
void imprimirDetallesParaIntercambio(TipoJugador *jugador);
TipoJugador *elegir_jugador_para_intercambio(List *opciones);
void eleccion_de_propiedades(List *propiedadesPosibles, List *propiedadesPedidas);
void fase_de_eleccion(TipoJugador *elegido, int *dineroPedido, List *propiedadesPedidas);
void fase_de_ofrecer(TipoJugador *jugador, int *dineroOfrecido, List *propiedadesOfrecidas);
void mostrar_detalles_intercambio(int dineroPedido, List *propiedadesPedidas, int dineroOfrecido, List *propiedadesOfrecidas);
void cambios_de_dinero(TipoJugador *jugador, TipoJugador *elegido, int dineroPedido, int dineroOfrecido);
void cambios_de_propiedad(TipoJugador *jugador, TipoJugador *elegido, List *propiedadesPedidas, List *propiedadesOfrecidas);
void actualizar_listas_propiedades(TipoJugador *jugador, TipoJugador *elegido);
void ejecutarIntercambio(TipoJugador *jugador, TipoJugador *elegido, int dineroPedido, int dineroOfrecido, List *propiedadesPedidas, List *propiedadesOfrecidas);
void menu_de_intercambio(TipoJugador *jugador, PartidaGlobal *partida);
void inicializarMapa(int mapa[11][11]);
void visualizarMapa(int mapa[11][11]);
void pagarFianza(TipoJugador* jugador, int monto_fianza);
void turnoJugador(TipoJugador** jugador, PartidaGlobal *partida, int mapa[11][11]);
TipoJugador *verificarGanador(List *jugadores);
void mostrar_reglas();
void mostrarMenuInicial();
void liberarMemoria(PartidaGlobal *partida);
void iniciarPartida();

#endif // FUNCIONES_H