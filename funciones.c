#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "tdas/list.h"
#include "tdas/queue.h"
#define NUM_PROPIEDADES 40
#define FILAS 11
#define COLUMNAS 11
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
    int coordenadaX;
    int coordenadaY;
}; 

typedef struct{
    char descripcion[200]; //Descripción de la carta
    void (*efecto)(TipoJugador* jugador); // Función que define el efecto de la carta en el jugador
}TipoCarta;

// Declarar los datos de las casillas
typedef struct {
    char nombre_casilla[40];     // Nombre de la casilla
    char tipo; // "P" para propiedad, "C" para carta, "J" para carcel
    union {
        TipoPropiedad *propiedad;    
        int cantidad_impuesto;
        TipoCarta* carta;              
    };
}TipoCasilla;

struct partidaGlobal{
    TipoCasilla *tablero[40]; // Arreglo de casillas del tablero
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
//Funcion para solicitar la cantidad de jugadores
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
    {"SALIDA (COBRE $2000)", false, NULL, 0, 0, 0, 0, "NEUTRO", false, 10, 10},
    {"TORPEDERAS", true, NULL, 600, 40, 0, 500, "PLAYA ANCHA", false, 10, 9},
    {"ARCA COMUNAL", false, NULL, 0, 0, 0, 0, "CARTAS", false, 10, 8},
    {"SEDE ALIMENTOS PUCV", true, NULL, 800, 65, 0, 500, "PLAYA ANCHA", false, 10, 7},
    {"IMPUESTOS (PAGAR $2000)", false, NULL, 0, 0, 0, 0, "PAGAR", false, 10, 6},
    {"ESTACIÓN PUERTO", false, NULL, 2000, 250, 0, 0, "METRO", false, 10, 5},
    {"PARQUE ITALIA", true, NULL, 1000, 120, 0, 500, "VALPARAÍSO", false, 10, 4},
    {"FORTUNA", false, NULL, 0, 0, 0, 0, "CARTAS", false, 10, 3},
    {"IBC PUCV", true, NULL, 1100, 150, 0, 500, "VALPARAÍSO", false, 10, 2},
    {"AV. PEDRO MONT", true, NULL, 1200, 160, 0, 500, "VALPARAÍSO", false, 10, 1},
    {"CARCEL", false, NULL, 0, 0, 0, 0, "NEUTRO", false, 10, 0},
    {"LAGUNA VERDE", true, NULL, 1400, 200, 0, 1000, "CURAUMA", false, 9, 0},
    {"CAMPUS CURAUMA PUCV", true, NULL, 1500, 235, 0, 1000, "CURAUMA", false, 7, 0},
    {"ESVAL (AGUA)", false, NULL, 2000, 50, 0, 0, "COMPAÑIAS", false, 8, 0},
    {"LAGO PEÑUELAS", true, NULL, 1650, 250, 0, 1000, "CURAUMA", false, 6, 0},
    {"ESTACIÓN BARÓN", false, NULL, 2000, 250, 0, 0, "METRO", false, 5, 0},
    {"MUELLE BARÓN", true, NULL, 1800, 280, 0, 1000, "COSTA VALPO", false, 4, 0},
    {"CALETA PORTALES", true, NULL, 1900, 290, 0, 1000, "COSTA VALPO", false, 2, 0},
    {"ARCA COMUNAL", false, NULL, 0, 0, 0, 0, "CARTAS", false, 3, 0},
    {"ESCUELA ECONOMÍA PUCV", true, NULL, 2000, 320, 0, 1000, "COSTA VALPO", false, 1, 0},
    {"RELOJ DE FLORES", false, NULL, 0, 0, 0, 0, "NEUTRO", false, 0, 0},
    {"SEDE MECÁNICA PUCV", true, NULL, 2200, 375, 0, 1500, "INTERIOR", false, 0, 1},
    {"QUILPUE", true, NULL, 2400, 400, 0, 1500, "INTERIOR", false, 0, 3},
    {"FORTUNA", false, NULL, 0, 0, 0, 0, "CARTAS", false, 0, 2},
    {"VILLA ALEMANA", true, NULL, 2400, 400, 0, 1500, "INTERIOR", false, 0, 4},
    {"ESTACIÓN LIMACHE", false, NULL, 2000, 250, 0, 0, "METRO", false, 0, 5},
    {"OLMUE", true, NULL, 2600, 440, 0, 1500, "INTERIOR II", false, 0, 6},
    {"QUILLOTA", true, NULL, 2600, 440, 0, 1500, "INTERIOR II", false, 0, 7},
    {"VAYA A CARCEL", false, NULL, 0, 0, 0, 0, "NEUTRO", false, 0, 10},
    {"FACU. AGRONOMÍA PUCV", true, NULL, 2800, 480, 0, 1500, "INTERIOR II", false, 0, 9},
    {"MUELLE VERGARA", true, NULL, 3000, 520, 0, 2000, "VIÑA DEL MAR", false, 1, 10},
    {"SEDE SAUSALITO PUCV", true, NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", false, 2, 10},
    {"ARCA COMUNAL", false, NULL, 0, 0, 0, 0, "CARTAS", false, 3, 10},
    {"15 NORTE", true, NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", false, 4, 10},
    {"ESTACIÓN MIRAMAR", false, NULL, 2000, 250, 0, 0, "METRO", false, 5, 10},
    {"RENACA", true, NULL, 3500, 700, 0, 2000, "NORTE VIÑA", false, 7, 10},
    {"FORTUNA", false, NULL, 0, 0, 0, 0, "CARTAS", false, 6, 10},
    {"MC DONALD (PAGAR $1000)", false, NULL, 0, 0, 0, 0, "PAGAR", false, 8, 10},
    {"DUNAS DE CONCON", true, NULL, 4000, 1000, 0, 2000, "NORTE VIÑA", false, 9, 10},
    {"CHILQUINTA (LUZ)", false, NULL, 2000, 50, 0, 0, "COMPAÑIAS", false, 0, 8}};

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



//FUNCIONES PARA LA HIPOTECA DE PROPIEDADES
void hipotecar_propiedad(TipoJugador *jugador, TipoPropiedad *propiedad)
{
    if (propiedad->propietario == jugador)
    {
        if (!propiedad->hipotecado)
        {
            printf("¿Quieres hipotecar la propiedad %s por %d? (s/n): ", propiedad->nombre, propiedad->precio / 2);
            char respuesta;
            scanf(" %c", &respuesta);
            if (respuesta == 's' || respuesta == 'S')
            {
                propiedad->hipotecado = true;
                jugador->dinero += propiedad->precio / 2;
                printf("Felicidades %s, has hipotecado la propiedad %s y has recibido %d!\n", jugador->nombre_jugador, propiedad->nombre, propiedad->precio / 2);

                printf("Recuerda que no podrás cobrar renta por %s mientras esté hipotecada.\n", propiedad->nombre);
            }
            else
            {
                return;
            }
        }
        else
        {
            printf("La propiedad %s ya está hipotecada.\n", propiedad->nombre);
        }
    }
    else
    {
        printf("La propiedad %s no está a tu nombre.\n", propiedad->nombre);
    }


}

void recuperar_propiedad_hipotecada(TipoJugador *jugador, TipoPropiedad *propiedad)
{
    if (propiedad->hipotecado && propiedad->propietario == jugador) {
        int precio_venta = propiedad->precio / 2 + propiedad->precio * 0.1;

        // Preguntar al jugador si desea recuperar la propiedad
        printf("¿Quieres recuperar la propiedad %s por %d? (s/n): ", propiedad->nombre, precio_venta);
        char respuesta;
        scanf(" %c", &respuesta);

        if (respuesta == 's' || respuesta == 'S') {
            // Verificar si el jugador tiene suficiente dinero para recuperar la propiedad
            if (jugador->dinero >= precio_venta) {
                jugador->dinero -= precio_venta;
                propiedad->hipotecado = false;

                printf("%s, has recuperado la propiedad %s por %d!\n",
                       jugador->nombre_jugador, propiedad->nombre, precio_venta);
            } else {
                printf("%s, no tienes suficiente dinero para recuperar la propiedad %s.\n",
                       jugador->nombre_jugador, propiedad->nombre);
            }
        } else {
            printf("No se ha recuperado la propiedad %s.\n", propiedad->nombre);
        }

    } else {
        printf("No puedes recuperar esta propiedad hipotecada.\n");
    } 
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

// Función para comprar casas en una propiedad
void comprar_casas(TipoJugador *jugador, TipoPropiedad *propiedad) {
    // Verificar que la propiedad tiene dueño y que el jugador es el propietario
    if (propiedad->propietario == jugador) {
        // Verificar que la propiedad no está hipotecada
        if (!propiedad->hipotecado) {
            unsigned int num_casas;
            printf("¿Cuántas casas quieres comprar para la propiedad %s? (máximo 5): ", propiedad->nombre);
            scanf("%d", &num_casas);

            // Verificar que el número total de casas no exceda el límite
            if (propiedad->casas + num_casas <= 5 && num_casas > 0) {
                // Calcular el costo total de las casas a comprar
                unsigned int costo_total = propiedad->precio_casa * num_casas;

                // Verificar que el jugador tiene suficiente dinero para comprar las casas
                if (jugador->dinero >= costo_total) {
                    char respuesta;
                    printf("¿Quieres comprar %d casas para la propiedad %s por %d? (s/n): ", num_casas, propiedad->nombre, costo_total);
                    scanf(" %c", &respuesta);

                    if (respuesta == 's' || respuesta == 'S') {
                        // Reducir el dinero del jugador en el costo total
                        jugador->dinero -= costo_total;

                        // Incrementar el número de casas en la propiedad
                        propiedad->casas += num_casas;

                        // Incrementar la renta según la cantidad de casas compradas
                        propiedad->renta *= (2 * num_casas);

                        printf("Felicidades %s, has comprado %d casas para la propiedad %s por %d!\n",
                            jugador->nombre_jugador, num_casas, propiedad->nombre, costo_total);
                    } else {
                        printf("%s, has decidido no comprar %d casas para la propiedad %s.\n",
                            jugador->nombre_jugador, num_casas, propiedad->nombre);
                    }
                } else {
                    // El jugador no tiene suficiente dinero para comprar las casas
                    printf("%s, no tienes suficiente dinero para comprar %d casas para la propiedad %s.\n",
                        jugador->nombre_jugador, num_casas, propiedad->nombre);
                }
            } else {
                // No se puede exceder el número máximo de casas
                printf("%s, no puedes comprar %d casas para la propiedad %s porque excede el límite de 5 casas.\n",
                    jugador->nombre_jugador, num_casas, propiedad->nombre);
            }
        } else {
            // La propiedad está hipotecada
            printf("%s, no puedes comprar casas para la propiedad %s porque está hipotecada.\n",
                jugador->nombre_jugador, propiedad->nombre);
        }
    } else {
        // El jugador no es el propietario de la propiedad
        printf("%s, no eres el propietario de la propiedad %s y no puedes comprar casas para ella.\n",
            jugador->nombre_jugador, propiedad->nombre);
    }
    presioneEnter();
}

//VENDER CASAS
void vender_casas(TipoJugador *jugador, TipoPropiedad *propiedad) {
    // Verificar que la propiedad tiene dueño y que el jugador es el propietario
    if (propiedad->propietario == jugador) {
        // Verificar que la propiedad tiene casas para vender
        if (propiedad->casas > 0) {
            unsigned int num_casas;
            printf("¿Cuántas casas quieres vender para la propiedad %s? (máximo %d): ", propiedad->nombre, propiedad->casas);
            scanf("%d", &num_casas);

            // Verificar que el número de casas a vender no exceda las casas disponibles
            if (num_casas > 0 && num_casas <= propiedad->casas) {
                // Calcular el monto total de venta (mitad del costo de las casas)
                unsigned int monto_total = (propiedad->precio_casa * num_casas) / 2;

                char respuesta;
                printf("¿Quieres vender %d casas de la propiedad %s por %d? (s/n): ", num_casas, propiedad->nombre, monto_total);
                scanf(" %c", &respuesta);

                if (respuesta == 's' || respuesta == 'S') {
                    // Incrementar el dinero del jugador en el monto total
                    jugador->dinero += monto_total;

                    // Reducir el número de casas en la propiedad
                    propiedad->casas -= num_casas;

                    // Ajustar la renta según la cantidad de casas vendidas
                    for (unsigned int i = 0; i < num_casas; i++) {
                        propiedad->renta /= 2;
                    }

                    printf("Felicidades %s, has vendido %d casas de la propiedad %s por %d!\n",
                           jugador->nombre_jugador, num_casas, propiedad->nombre, monto_total);
                } else {
                    printf("%s, has decidido no vender %d casas de la propiedad %s.\n",
                           jugador->nombre_jugador, num_casas, propiedad->nombre);
                }
            } else {
                // No se puede vender más casas de las disponibles
                printf("%s, no puedes vender %d casas de la propiedad %s porque solo tienes %d casas.\n",
                       jugador->nombre_jugador, num_casas, propiedad->nombre, propiedad->casas);
            }
        } else {
            // La propiedad no tiene casas para vender
            printf("%s, no puedes vender casas de la propiedad %s porque no tiene casas.\n",
                   jugador->nombre_jugador, propiedad->nombre);
        }
    } else {
        // El jugador no es el propietario de la propiedad
        printf("%s, no eres el propietario de la propiedad %s y no puedes vender casas de ella.\n",
               jugador->nombre_jugador, propiedad->nombre);
    }
    presioneEnter();
}
// INICIALIZAR TABLERO
void inicializarTablero(TipoPropiedad *propiedades) {
    char tablero[11][11][4]; // Matriz para almacenar nombres de propiedades (limitado a 3 caracteres)

    // Inicializar todo el tablero como casillas vacías
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            strcpy(tablero[i][j], "   "); // Inicializar el nombre como tres espacios
        }
    }

    // Asignar propiedades al tablero con sus coordenadas
    for (int i = 0; i < NUM_PROPIEDADES; i++) {
        int x = propiedades[i].coordenadaX;
        int y = propiedades[i].coordenadaY;

        // Asignar el nombre de la propiedad a la casilla correspondiente
        strncpy(tablero[x][y], propiedades[i].nombre, 3);
    }

    // Imprimir el tablero para verificar la inicialización
    printf("\n== Tablero Inicializado ==\n");
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            printf("[%s] ", tablero[i][j]);
        }
        printf("\n");
    }

    presioneEnter();
}


// TESTEO DEL JUEGO FUNCIONES
//  Función exclusiva para programadores para probar comprar_propiedad y
//  comprar_casas
void testear_funciones() {
    
    

    // Crear jugadores de prueba
    TipoJugador jugador1 = {5000, 0, "Manuel", 0, NULL, NULL}; // Inicializar las listas con NULL o funciones de creación de listas
    TipoJugador jugador2 = {5000, 0, "No Manuel", 0, NULL, NULL};

    // Inicializar propiedades
    TipoPropiedad *propiedades = inicializar_propiedades();
    
    // Inicializar tablero
    inicializarTablero(propiedades);
    // Simular compra de propiedad por jugador1
    printf("\n== Jugador 1 intenta comprar Propiedad 1 ==\n");
    comprar_propiedad(&jugador1, buscar_propiedad_por_nombre(propiedades, "TORPEDERAS"));

    // Simular intento de jugador2 de comprar propiedad ya comprada
    printf("\n== Jugador 2 intenta comprar Propiedad 1 ==\n");
    comprar_propiedad(&jugador2, buscar_propiedad_por_nombre(propiedades, "TORPEDERAS"));

    // Simular compra de otra propiedad por jugador2
    printf("\n== Jugador 2 intenta comprar Propiedad 2 ==\n");
    comprar_propiedad(&jugador2, buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS"));

    // Simular compra de casas por jugador1 en Propiedad 1
    printf("\n== Jugador 1 intenta comprar casas para Propiedad 1 ==\n");
    comprar_casas(&jugador1, buscar_propiedad_por_nombre(propiedades, "TORPEDERAS"));

    // Simular compra de casas por jugador1 en Propiedad 2
    printf("\n== Jugador 1 intenta comprar casas para Propiedad 2 ==\n");
    comprar_casas(&jugador1, buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS"));

    // Simular intento de jugador2 de comprar casas en Propiedad 2
    printf("\n== Jugador 2 intenta comprar casas para Propiedad 2 ==\n");
    comprar_casas(&jugador2, buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS"));

    // Simular venta de casas por jugador1 en Propiedad 1
    printf("\n== Jugador 1 intenta vender casas de Propiedad 1 ==\n");
    vender_casas(&jugador1, buscar_propiedad_por_nombre(propiedades, "TORPEDERAS"));

    // Simular venta de casas por jugador1 en Propiedad 2
    printf("\n== Jugador 1 intenta vender casas de Propiedad 2 ==\n");
    vender_casas(&jugador1, buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS"));

    // Mostrar estado final de las propiedades y jugadores
    printf("\n== Estado final de las propiedades y jugadores ==\n");
    printf("Propiedad 1: %s, Propietario: %s, Casas: %d, Renta: %d\n",
           buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->nombre,
           buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->propietario ? buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->propietario->nombre_jugador : "Sin dueño",
           buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->casas,
           buscar_propiedad_por_nombre(propiedades, "TORPEDERAS")->renta);
    printf("Propiedad 2: %s, Propietario: %s, Casas: %d, Renta: %d\n",
           buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->nombre,
           buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->propietario ? buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->propietario->nombre_jugador : "Sin dueño",
           buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->casas,
           buscar_propiedad_por_nombre(propiedades, "PUCV SEDE ALIMENTOS")->renta);
    printf("Jugador 1: %s, Dinero: %d\n", jugador1.nombre_jugador, jugador1.dinero);
    printf("Jugador 2: %s, Dinero: %d\n", jugador2.nombre_jugador, jugador2.dinero);

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
        TipoCasilla *casilla = partida->tablero[i];
        fprintf(file, "%s,%c,", casilla->nombre_casilla, casilla->tipo);
        if (casilla->tipo == 'P') {
            TipoPropiedad *propiedad = casilla->propiedad;
            fprintf(file, "%s,%d,%s,%d,%d,%s,%d\n",
                    propiedad->nombre,
                    propiedad->esPropiedad,
                    propiedad->propietario->nombre_jugador,
                    propiedad->precio,
                    propiedad->renta,
                    propiedad->sector,
                    propiedad->hipotecado);
        } else if (casilla->tipo == 'C') {
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

void verificar_bancarrota(TipoJugador *jugador){
    
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

