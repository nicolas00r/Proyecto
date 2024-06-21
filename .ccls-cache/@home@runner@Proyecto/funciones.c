#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "tdas/list.h"
#include "tdas/queue.h"
#define NUM_PROPIEDADES 28
//STRUCTS
// Declarar los datos de los jugadores
struct TipoJugador{
    int dinero;                    // Dinero del jugador
    int penalizacion;              // Penalizacion de la carcel hacia al jugador
    char nombre_jugador[40];       // Nombre del jugador
    int posicion;                  // Posicion del jugador
    List *propiedades;    
    List *cartas;
};

struct TipoPropiedad{
    char nombre[50];
    bool esPropiedad;
    TipoJugador* propietario; //REEMPLAZAR VOID* POR TIPO JUGADOR
    int precio;
    int renta;
    int casas;
    int precio_casa;
    char sector[50];
    bool hipotecado;
}; 

typedef struct{
    char descripcion[200]; //Descripción de la carta
    void (*efecto)(TipoJugador* jugador); // Función que define el efecto de la carta en el jugador
}TipoCarta;

// Declarar los datos de las casillas
typedef struct {
    char nombre_casilla[40];     // Nombre de la casilla
    char tipo[5]; // "P" para propiedad, "C" para carta
    union {
        TipoPropiedad *propiedad;    
        int cantidad_impuesto;
        TipoCarta* carta;              
    };
}TipoCasilla;

typedef struct {
     TipoCasilla *casillas[40]; // Arreglo de casillas del tablero
}TipoTablero;

struct partidaGlobal{
    TipoTablero *tablero;
    List *jugadores;
    List *turnos;
    Queue* fortuna;
    Queue* arca_comunal;
};

typedef TipoPropiedad TipoPropiedad;
typedef TipoJugador TipoJugador;
typedef partidaGlobal partidaGlobal;

void presioneEnter() {
    printf("Presione Enter para continuar...");
    getchar();
    // Limpiar BUFFER
    while (getchar() != '\n');

}

//Función para limpiar la pantalla
void limpiar_pantalla() {
    system("clear");
}

int solicitar_jugadores()
{
    int num_jugadores;
    do {
        printf("Ingrese el número de jugadores (2 a 4): ");
        scanf("%d", &num_jugadores);
        while (getchar() != '\n'); // Limpiar el buffer de entrada  
        if (num_jugadores < 2 || num_jugadores > 4)
            printf("!! El número de jugadores debe estar entre 2 y 4. !!\n");
    } while (num_jugadores < 2 || num_jugadores > 4);
    return num_jugadores;
}

void asignar_jugadores(partidaGlobal *partida, int num_jugadores) {
    // Inicializar la lista de jugadores si aún no está inicializada
    if (partida->jugadores == NULL) {
        partida->jugadores = list_create();
        if (partida->jugadores == NULL) {
            printf("No se pudo crear la lista de jugadores\n");
            return;
        }
    } else {
        // Limpiar la lista de jugadores existente
        list_clean(partida->jugadores);
    }

    // Asignar valores iniciales y nombres a los jugadores
    for (int i = 0; i < num_jugadores; i++) {
        TipoJugador *jugador = (TipoJugador *)malloc(sizeof(TipoJugador));
        if (jugador == NULL) {
            printf("Error al asignar memoria para el jugador %d\n", i+1);
            return;
        }

        printf("Ingrese el nombre del jugador %d: ", i+1);
        fgets(jugador->nombre_jugador, sizeof(jugador->nombre_jugador), stdin);
        jugador->nombre_jugador[strcspn(jugador->nombre_jugador, "\n")] = '\0'; // Eliminar el salto de línea

        jugador->dinero = 15000;
        jugador->penalizacion = 0;
        jugador->posicion = 0;
        jugador->propiedades = list_create();
        jugador->cartas = list_create();

        // Agregar el jugador a la lista de jugadores de la partida
        list_pushBack(partida->jugadores, jugador);


    }
    //verificar que se añadieron correctamente BORRAR FINAL CODIGO
    int tamano = list_size(partida->jugadores);
    if (tamano == num_jugadores) {
        printf("Se han asignado correctamente los jugadores\n");
    } else {
        printf("No se han asignado correctamente los jugadores\n");
    }

}







// Inicializar semilla aleatoria, esto permitirá que cada vez que se corra
// el programa, los resultados sean diferentes
void inicializar_aleatoriedad(){
    srand(time(NULL));
}

// Función para tirar los dados
int tirar_dados(){
    int dado1 = (rand() % 6) + 1; // Genera un número aleatorio entre 1 y 6
    int dado2 = (rand() % 6) + 1; // Genera un número aleatorio entre 1 y 6

    return dado1 + dado2;
}

// INICIACIÓN DEL JUEGO
TipoPropiedad *inicializar_propiedades() {
  // Crear un array estático de TipoPropiedad con todas las propiedades
  // iniciales
  static TipoPropiedad propiedades[NUM_PROPIEDADES] = {
      {"TORPEDERAS", true, NULL, 600, 40, 0, 500, "PLAYA ANCHA", false},
      {"PUCV SEDE ALIMENTOS", true, NULL, 650, 75, 0, 500, "PLAYA ANCHA",
       false},
      {"ESTACIÓN PUERTO", false, NULL, 2000, 250, 0, 0, "METRO", false},
      {"PLAZA VICTORIA", true, NULL, 1000, 120, 0, 500, "VALPARAÍSO", false},
      {"PUCV IBC", true, NULL, 1100, 150, 0, 500, "VALPARAÍSO", false},
      {"AV. PEDRO MONT", true, NULL, 1200, 160, 0, 500, "VALPARAÍSO", false},
      {"LAGUNA VERDE", true, NULL, 1400, 200, 0, 1000, "CURAUMA", false},
      {"PUCV CAMPUS CURAUMA", true, NULL, 1500, 235, 0, 1000, "CURAUMA", false},
      {"LAGO PEÑUELAS", true, NULL, 1650, 250, 0, 1000, "CURAUMA", false},
      {"ESTACIÓN BARÓN", false, NULL, 2000, 250, 0, 0, "METRO", false},
      {"MUELLE BARÓN", true, NULL, 1800, 280, 0, 1000, "COSTA VALPO", false},
      {"CALETA PORTALES", true, NULL, 1900, 290, 0, 1000, "COSTA VALPO", false},
      {"PUCV ESCUELA ECONOMÍA", true, NULL, 2000, 320, 0, 1000, "COSTA VALPO",
       false},
      {"PUCV SEDE MECÁNICA", true, NULL, 2200, 375, 0, 1500, "INTERIOR", false},
      {"QUILPUE", true, NULL, 2400, 400, 0, 1500, "INTERIOR", false},
      {"VILLA ALEMANA", true, NULL, 2400, 400, 0, 1500, "INTERIOR", false},
      {"ESTACIÓN LIMACHE", false, NULL, 2000, 250, 0, 0, "METRO", false},
      {"OLMUE", true, NULL, 2600, 440, 0, 1500, "INTERIOR II", false},
      {"QUILLOTA", true, NULL, 2600, 440, 0, 1500, "INTERIOR II", false},
      {"PUCV FACU. AGRONOMÍA", true, NULL, 2800, 480, 0, 1500, "INTERIOR II",
       false},
      {"MUELLE VERGARA", true, NULL, 3000, 520, 0, 2000, "VIÑA DEL MAR", false},
      {"PUCV SEDE SAUSALITO", true, NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR",
       false},
      {"15 NORTE", true, NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", false},
      {"ESTACIÓN MIRAMAR", false, NULL, 2000, 250, 0, 0, "METRO", false},
      {"REÑACA", true, NULL, 3500, 700, 0, 2000, "NORTE VIÑA", false},
      {"DUNAS DE CONCON", true, NULL, 4000, 1000, 0, 2000, "NORTE VIÑA", false},
      {"ESVAL (AGUA)", false, NULL, 2000, 50, 0, 0, "COMPAÑIAS", false},
      {"CHILQUINTA (LUZ)", false, NULL, 2000, 50, 0, 0, "COMPAÑIAS", false}};

  return propiedades;
}

void reinicializar_propiedades(TipoPropiedad* propiedades) {
    for (int i = 0; i < NUM_PROPIEDADES; i++) { // Asume que tienes una constante NUM_PROPIEDADES
        propiedades[i].propietario = NULL;
        propiedades[i].casas = 0;
        propiedades[i].hipotecado = false;
    }
}

TipoPropiedad *buscar_propiedad_por_nombre(TipoPropiedad *propiedades,
                                           const char *nombre) {
  for (int i = 0; i < NUM_PROPIEDADES; ++i) {
    if (strcmp(propiedades[i].nombre, nombre) == 0) {
      return &propiedades[i];
    }
  }
  return NULL; // Si no se encuentra la propiedad
}
// FUNCIONES DE COMPRA Y VENTA DE PROPIEDADES

void comprar_propiedad(TipoJugador *jugador, TipoPropiedad *propiedad) {
  // Verificar que la propiedad no tiene dueño
  if (propiedad->propietario == NULL) {
    // Verificar que el jugador tiene suficiente dinero para comprar la
    // propiedad
    if (jugador->dinero >= propiedad->precio) {
      char respuesta;
      printf("¿Quieres comprar %s por %d? (s/n): ", propiedad->nombre,
             propiedad->precio);
      scanf(" %c", &respuesta);

      if (respuesta == 's' || respuesta == 'S') {
        // Reducir el dinero del jugador en el precio de la propiedad
        jugador->dinero -= propiedad->precio;

        // Asignar la propiedad al jugador
        propiedad->propietario = jugador;

        // Agregar la propiedad a la lista de propiedades del jugador
        list_pushFront(jugador->propiedades, propiedad);

        printf("Felicidades %s, has comprado la propiedad %s por %d!\n",
               jugador->nombre_jugador, propiedad->nombre, propiedad->precio);
      } else {
        return;
      }
    } else {
      // El jugador no tiene suficiente dinero para comprar la propiedad
      printf("%s, no tienes suficiente dinero para comprar la propiedad %s.\n",
             jugador->nombre_jugador, propiedad->nombre);
    }
  } else {
    // La propiedad ya tiene un dueño
    printf("La propiedad %s ya tiene un dueño y no puede ser comprada.\n",
           propiedad->nombre);
  }
  presioneEnter();
}

void comprar_casas(TipoJugador *jugador, TipoPropiedad *propiedad) {
  // Verificar que la propiedad tiene dueño y que el jugador es el propietario
  if (propiedad->propietario == jugador) {
    // Verificar que la propiedad no está hipotecada
    if (!propiedad->hipotecado) {
      unsigned int num_casas;
      printf(
          "¿Cuántas casas quieres comprar para la propiedad %s? (máximo 5): ",
          propiedad->nombre);
      scanf("%d", &num_casas);

      // Verificar que el número total de casas no exceda el límite
      if (propiedad->casas + num_casas <= 5 && num_casas > 0) {
        // Calcular el costo total de las casas a comprar
          unsigned int costo_total = propiedad->precio_casa * num_casas;

        // Verificar que el jugador tiene suficiente dinero para comprar las
        // casas
        if (jugador->dinero >= costo_total) {
          char respuesta;
          printf(
              "¿Quieres comprar %d casas para la propiedad %s por %d? (s/n): ",
              num_casas, propiedad->nombre, costo_total);
          scanf(" %c", &respuesta);

          if (respuesta == 's' || respuesta == 'S') {
            // Reducir el dinero del jugador en el costo total
            jugador->dinero -= costo_total;

            // Incrementar el número de casas en la propiedad
            propiedad->casas += num_casas;

            printf("Felicidades %s, has comprado %d casas para la propiedad %s "
                   "por %d!\n",
                   jugador->nombre_jugador, num_casas, propiedad->nombre,
                   costo_total);
          } else {
            printf(
                "%s, has decidido no comprar %d casas para la propiedad %s.\n",
                jugador->nombre_jugador, num_casas, propiedad->nombre);
          }
        } else {
          // El jugador no tiene suficiente dinero para comprar las casas
          printf("%s, no tienes suficiente dinero para comprar %d casas para "
                 "la propiedad %s.\n",
                 jugador->nombre_jugador, num_casas, propiedad->nombre);
        }
      } else {
        // No se puede exceder el número máximo de casas
        printf("%s, no puedes comprar %d casas para la propiedad %s porque "
               "excede el límite de 5 casas.\n",
               jugador->nombre_jugador, num_casas, propiedad->nombre);
      }
    } else {
      // La propiedad está hipotecada
      printf("%s, no puedes comprar casas para la propiedad %s porque está "
             "hipotecada.\n",
             jugador->nombre_jugador, propiedad->nombre);
    }
  } else {
    // El jugador no es el propietario de la propiedad
    printf("%s, no eres el propietario de la propiedad %s y no puedes comprar "
           "casas para ella.\n",
           jugador->nombre_jugador, propiedad->nombre);
  }
  presioneEnter();
}

// TESTEO DEL JUEGO FUNCIONES
//  Función exclusiva para programadores para probar comprar_propiedad y
//  comprar_casas
void testear_funciones() {
  // Crear jugadores de prueba
  TipoJugador jugador1 = {1500, 0,   "Manuel", 0,
                          NULL, NULL}; // Inicializar las listas con NULL o
                                       // funciones de creación de listas
  TipoJugador jugador2 = {2000, 0, "No Manuel", 0, NULL, NULL};

  // Inicializar propiedades
  TipoPropiedad *propiedades = inicializar_propiedades();

  // Buscar propiedades específicas por nombre (ejemplo)


  // Simular compra de propiedad por jugador1
  printf("\n== Jugador 1 intenta comprar Propiedad 1 ==\n");
  comprar_propiedad(&jugador1, buscar_propiedad_por_nombre(propiedades, "TORPEDERAS"));

  // Simular intento de jugador2 de comprar propiedad ya comprada
  printf("\n== Jugador 2 intenta comprar Propiedad 1 ==\n");
  comprar_propiedad(&jugador2, buscar_propiedad_por_nombre(propiedades, "TORPEDERAS"));


  printf("\n== Jugador 2 intenta comprar Propiedad 2 ==\n");
  comprar_propiedad(&jugador2, buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS"));

  // Simular compra de casas por jugador1 en Propiedad 1
  printf("\n== Jugador 1 intenta comprar casas para Propiedad 1 ==\n");
  comprar_casas(&jugador1, buscar_propiedad_por_nombre(propiedades, "TORPEDERAS"));

  // Simular compra de casas por jugador1 en Propiedad 2
  printf("\n== Jugador 1 intenta comprar  casas para Propiedad 2 ==\n");
  comprar_casas(&jugador1, buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS"));

  // Simular intento de jugador2 de comprar casas en Propiedad 1 sin ser
  // propietario
  printf("\n== Jugador 2 intenta comprar casas para Propiedad 2 ==\n");
  comprar_casas(&jugador2, buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS"));

  // Mostrar estado final de las propiedades y jugadores
  printf("\n== Estado final de las propiedades y jugadores ==\n");
  printf("Propiedad 1: %s, Propietario: %s, Casas: %d\n", buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->nombre,
      buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->propietario ? buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->propietario->nombre_jugador
                                 : "Sin dueño",
      buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->casas);
  printf("Propiedad 2: %s, Propietario: %s, Casas: %d\n", buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->nombre,
      buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->propietario ? buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->propietario->nombre_jugador
                                 : "Sin dueño",
      buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->casas);
  printf("Jugador 1: %s, Dinero: %d\n", jugador1.nombre_jugador,
         jugador1.dinero);
  printf("Jugador 2: %s, Dinero: %d\n", jugador2.nombre_jugador,
         jugador2.dinero);

  // Liberar memoria de las listas de propiedades (si aplica)
  list_clean(jugador1.propiedades);
  list_clean(jugador2.propiedades);

  // Reinicializar propiedades
  reinicializar_propiedades(propiedades);

  presioneEnter();
}


//REGLAS
void mostrar_reglas() {
    printf("Reglas del juego:\n");
    printf("\n=== Reglas del Monopoly ===\n");
    printf("1. El objetivo del juego es ser el jugador con más cantidad de dinero acumulado.\n");
    printf("2. Los jugadores se mueven por el tablero comprando propiedades y cobrando renta.\n");
    printf("3. Se pueden construir casas en propiedades para aumentar la renta que generan.\n");
    printf("4. Si un jugador cae en una propiedad sin dueño, puede comprarla. Si ya tiene dueño, paga renta.\n");
    printf("5. Hay casillas especiales como fortuna y arca comunal, la cual tienen efectos variados que pueden afectar positiva o negativamente al jugador.\n");
    printf("6. Si un jugador acaba en la carcel, deberá esperar 3 turnos o pagar una fianza para salir.\n");
    printf("7. El juego termina cuando solo queda un jugador con dinero y propiedades, es decir, cuando todo el resto este en bancarrota!.\n");
    printf("===========================\n");

    presioneEnter();
    return;
}

//Funcion para guardar partida
void guardar_partida(partidaGlobal *partida, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error al abrir el archivo para guardar la partida.\n");
        return;
    }

    // Guardar datos del tablero
    for (int i = 0; i < 40; i++) 
    {
        TipoCasilla *casilla = partida->tablero->casillas[i];
        fprintf(file, "%s,%s,", casilla->nombre_casilla, casilla->tipo);
        if (strcmp(casilla->tipo, "P") == 0) {
            TipoPropiedad *propiedad = casilla->propiedad;
            fprintf(file, "%s,%d,%s,%d,%d,%s,%d\n",
                    propiedad->nombre,
                    propiedad->esPropiedad,
                    propiedad->propietario->nombre_jugador,
                    propiedad->precio,
                    propiedad->renta,
                    propiedad->sector,
                    propiedad->hipotecado);
        } else if (strcmp(casilla->tipo, "C") == 0) {
            fprintf(file, "\n");
        } else {
            fprintf(file, "%d\n", casilla->cantidad_impuesto);
        }
    }

    //Guardar datos de los jugadores
    List *jugadores = partida->jugadores;
    TipoJugador *jugador = list_first(jugadores);
    while(jugador != NULL)
    {
        fprintf(file, "%d,%d,%s,%d\n", jugador->dinero, jugador->penalizacion, jugador->nombre_jugador, jugador->posicion);
        jugador = list_next(jugadores);
    }

    fclose(file);
}

// Función para mostrar el menú inicial
void mostrarMenuInicial() {

    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║        ＭＯＮＯＰＯＬY    ＶＡＬＰＡＲＡÍＳＯ    ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("=== MENÚ INICIAL ===\n");
    printf("1. Nueva partida\n");
    printf("2. Cargar partida\n");
    printf("3. Reglas\n");
    printf("4. Salir del juego\n");
    printf("Seleccione una opción: ");
}