#include <stdio.h>
#include <stdlib.h>

//#include "tdas/queue.h"

// STRUCTS
//  Declarar los datos de los jugadores
typedef struct {
  int dinero;              // Dinero del jugador
  int penalizacion;        // Penalizacion de la carcel hacia al jugador
  char nombre_jugador[40]; // Nombre del jugador
  int posicion;            // Posicion del jugador
  //    List *propiedades;
  //    List *cartas;
} TipoJugador;

typedef struct {
  char nombre_casilla[40]; // Nombre de la casilla
  char tipo[5]; // Tipo de casilla "P" para propiedad y "C" para carta
  union {
    // TipoPropiedad *propiedad;
    int cantidad_impuesto;
    // TipoCarta* carta;
  };
} TipoCasilla;

typedef struct {
  TipoCasilla *casillas[40]; // Arreglo de casillas del tablero
} TipoTablero;

int main(void) {
  printf("HOLA MUNDO ESTO ES UN CAMBIO\n");
  return 0;
}