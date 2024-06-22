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
#define NUM_CARTAS 12

// STRUCTS

// Struct para cada jugador
struct TipoJugador{
    int dinero;                    // Dinero del jugador
    int penalizacion;              // Penalizacion de la carcel hacia al jugador
    char nombre_jugador[40];       // Nombre del jugador
    int posicion;                  // Posicion del jugador
    List *propiedades;             // Lista de las propiedades del jugador
};

// Struct para cada propiedad
struct TipoPropiedad{
    char nombre[50];               // Nombre de la propiedad
    bool esPropiedad;              // Comprobar si es una propiedad que se puede comprar
    TipoJugador* propietario;      // Jugador dueño de la propiedad
    int precio;                    // Precio de la propiedad
    int renta;                     // Renta de la propiedad
    int casas;                     // Cantidad de casas de la propiedad
    int precio_casa;               // Precio de cada casa
    char sector[50];               // Sector de la propiedad
    bool hipotecado;               // Comprobar si la propiedad esta hipotecada
    int coordenadaX;               // Coordenada X de la propiedad
}; 

// Struct para cada carta
typedef struct{
    char descripcion[200]; //Descripción de la carta
    int cambio_dinero;
    int cambio_posicion;
    int cambio_penalizacion;
}TipoCarta;

// Struct para cada casilla
typedef struct {
    char nombre_casilla[40];     // Nombre de la casilla
    char tipo;                   // "P" para propiedad, "C" para carta, "J" para carcel
                                 // "S" para servicio "E" para salida, "M" para metro. 
    TipoPropiedad *propiedad;    // Propiedad que se encuentra en la casilla
}TipoCasilla;

// Struct de la partida
struct partidaGlobal{
    TipoCasilla **tablero; // Arreglo de casillas del tablero
    List *jugadores;       // Lista de jugadores de la partida
    List *turnos;          // Lista para controlar los turnos (se maneja de manera circular)
    Queue* fortuna;        // Cola para controlar las cartas de la fortuna
    Queue* arca_comunal;   // Cola para controlar las cartas de la arca comunal
};

// Definición de structs
typedef TipoPropiedad TipoPropiedad;
typedef TipoJugador TipoJugador;
typedef partidaGlobal partidaGlobal;

// INICIACIÓN DEL JUEGO

TipoCarta *fortuna()
{
    static TipoCarta fortuna[NUM_CARTAS] = {
    {"AVANCE HASTA LA CASILLA DE SALIDA, COBRE $2000", 0, 0, -1},
    {"AVANCE A DUNAS DE CONCON", 0, 38, -1},
    {"EL BANCO PAGA DIVIDENDOS DE $500", 500, -1, -1},
    {"AVANCE A ESCUELA ECONOMÍA PUCV", 0, 19, -1},
    {"VAYA DIRECTAMENTE A LA CÁRCEL", 0, 10, 3},
    {"USTED HA SIDO ELEGIDO PRESIDENTE DEL CENTRO DE ALUMNOS, PAGUE $100", -100, -1, -1},
    {"ENHORABUENA, HAN CARGADO LA BAES! RECIBA $1500.", 1500, -1, -1},
    {"AVANCE A ESTACIÓN MIRAMAR", 0, 34, -1},
    {"AVANCE A ESTACIÓN LIMACHE",0, 25, -1},
    {"AVANCE A ESTACIÓN PUERTO",0, 5, -1},
    {"AVANCE A ESTACIÓN BARÓN",0, 15, -1},
    {"SALGA DE LA CÁRCEL GRATIS", 0, -1, 0}
    };
    return fortuna;
}

TipoCarta *arca_comunal()
{
    static TipoCarta arca_comunal[NUM_CARTAS] = {
    {"AVANCE HASTA LA CASILLA DE SALIDA. COBRE $200", 0, 0, -1},
    {"HOSPITALIZACIÓN. PAGUE $1000.", -1000, -1, -1},
    {"CUOTA DEL SEGURO DE VIDA VENCE. COBRE $1000.", 1000, -1, -1},
    {"EL BANCO TE PAGA DIVIDENDOS DE $500.", 500, -1, -1},
    {"HAS GANADO EL SEGUNDO PREMIO EN UN CONCURSO DE BELLEZA. COBRE $300.", 300, -1, -1},
    {"RECIBA $250 POR SERVICIOS.", 250, -1, -1},
    {"ERROR BANCARIO A TU FAVOR. COBRE $2000.", 2000, -1, -1},
    {"PAGUE UNA MULTA DE $500.", -500, -1, -1},
    {"PAGUE LA MATRÍCULA DE LA PUCV. PAGUE $1500.", -1500, -1 , -1},
    {"VAYA DIRECTAMENTE A LA CÁRCEL", 0, 10, 3},
    {"SALGA DE LA CÁRCEL GRATIS", 0, -1, 0},
    {"EL METRO SE QUEDA DETENIDO EN ESTACION VALENCIA Y LLEGAS TARDE AL TRABAJO, PAGUE 500", -500, -1, -1}
    };
    return arca_comunal;
}

void randomizar_cartas(TipoCarta *cartas, int size)
{
    for (int pos_original = size - 1; pos_original > 0; pos_original--) 
    {
        int nueva_pos = rand() % (pos_original + 1);
        TipoCarta aux = cartas[pos_original];
        cartas[pos_original] = cartas[nueva_pos];
        cartas[nueva_pos] = aux;
    }
}

TipoCarta* crear_copia_cartas(TipoCarta *cartas, int size)
{
    TipoCarta *cartas_copia = malloc(sizeof(TipoCarta) * size);
    for (int i = 0; i < size; i++)
        cartas_copia[i] = cartas[i];
    return cartas_copia;
}

void arreglo_a_cola(TipoCarta *cartas, int size, Queue *cola)
{
    for(int i = 0; i < size; i++)
        queue_insert(cola, &cartas[i]);
}

Queue* inicializar_fortuna()
{
    TipoCarta *cartas_originales = fortuna();
    TipoCarta *cartas_copia = crear_copia_cartas(cartas_originales, NUM_CARTAS);
    randomizar_cartas(cartas_copia, NUM_CARTAS);
    Queue *fortuna = queue_create();
    arreglo_a_cola(cartas_copia, NUM_CARTAS, fortuna);
    return fortuna;
}

Queue* inicializar_arca_comunal()
{
    TipoCarta *cartas_originales = arca_comunal();
    TipoCarta *cartas_copia = crear_copia_cartas(cartas_originales, NUM_CARTAS);
    randomizar_cartas(cartas_copia, NUM_CARTAS);
    Queue *arca_comunal = queue_create();
    arreglo_a_cola(cartas_copia, NUM_CARTAS, arca_comunal);
    return arca_comunal;
}

TipoPropiedad *inicializar_propiedades() {
  // Crear un array estático de TipoPropiedad con todas las propiedades
  // iniciales
  static TipoPropiedad propiedades[NUM_PROPIEDADES] = {
    {"SALIDA (COBRE $2000)", false, NULL, 0, 0, 0, 0, "SALIDA", false, 0},
    {"TORPEDERAS", true, NULL, 600, 40, 0, 500, "PLAYA ANCHA", false, 1},
    {"ARCA COMUNAL", false, NULL, 0, 0, 0, 0, "CARTAS", false, 2},
    {"SEDE ALIMENTOS PUCV", true, NULL, 800, 65, 0, 500, "PLAYA ANCHA", false, 3},
    {"IMPUESTOS (PAGAR $2000)", false, NULL, 2000, 0, 0, 0, "PAGAR", false, 4},
    {"ESTACIÓN PUERTO", false, NULL, 2000, 250, 0, 0, "METRO", false, 5},
    {"PARQUE ITALIA", true, NULL, 1000, 120, 0, 500, "VALPARAÍSO", false, 6},
    {"FORTUNA", false, NULL, 0, 0, 0, 0, "CARTAS", false, 7},
    {"IBC PUCV", true, NULL, 1100, 150, 0, 500, "VALPARAÍSO", false, 8},
    {"AV. PEDRO MONT", true, NULL, 1200, 160, 0, 500, "VALPARAÍSO", false, 9},
    {"CARCEL", false, NULL, 0, 0, 0, 0, "NEUTRO", false, 10},
    {"LAGUNA VERDE", true, NULL, 1400, 200, 0, 1000, "CURAUMA", false, 11},
    {"CAMPUS CURAUMA PUCV", true, NULL, 1500, 235, 0, 1000, "CURAUMA", false, 12},
    {"ESVAL (AGUA)", false, NULL, 2000, 50, 0, 0, "COMPAÑIAS", false, 13},
    {"LAGO PEÑUELAS", true, NULL, 1650, 250, 0, 1000, "CURAUMA", false, 14},
    {"ESTACIÓN BARÓN", false, NULL, 2000, 250, 0, 0, "METRO", false, 15},
    {"MUELLE BARÓN", true, NULL, 1800, 280, 0, 1000, "COSTA VALPO", false, 16},
    {"CALETA PORTALES", true, NULL, 1900, 290, 0, 1000, "COSTA VALPO", false, 17},
    {"ARCA COMUNAL", false, NULL, 0, 0, 0, 0, "CARTAS", false, 18},
    {"ESCUELA ECONOMÍA PUCV", true, NULL, 2000, 320, 0, 1000, "COSTA VALPO", false, 19},
    {"RELOJ DE FLORES", false, NULL, 0, 0, 0, 0, "NEUTRO", false, 20},
    {"SEDE MECÁNICA PUCV", true, NULL, 2200, 375, 0, 1500, "INTERIOR", false, 21},
    {"QUILPUE", true, NULL, 2400, 400, 0, 1500, "INTERIOR", false, 22},
    {"FORTUNA", false, NULL, 0, 0, 0, 0, "CARTAS", false, 23},
    {"VILLA ALEMANA", true, NULL, 2400, 400, 0, 1500, "INTERIOR", false, 24},
    {"ESTACIÓN LIMACHE", false, NULL, 2000, 250, 0, 0, "METRO", false, 25},
    {"OLMUE", true, NULL, 2600, 440, 0, 1500, "INTERIOR II", false, 26},
    {"QUILLOTA", true, NULL, 2600, 440, 0, 1500, "INTERIOR II", false, 27},
    {"VAYA A CARCEL", false, NULL, 0, 0, 0, 0, "CARCEL", false, 28},
    {"FACU. AGRONOMÍA PUCV", true, NULL, 2800, 480, 0, 1500, "INTERIOR II", false, 29},
    {"MUELLE VERGARA", true, NULL, 3000, 520, 0, 2000, "VIÑA DEL MAR", false, 30},
    {"SEDE SAUSALITO PUCV", true, NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", false, 31},
    {"ARCA COMUNAL", false, NULL, 0, 0, 0, 0, "CARTAS", false, 32},
    {"15 NORTE", true, NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", false, 33},
    {"ESTACIÓN MIRAMAR", false, NULL, 2000, 250, 0, 0, "METRO", false, 34},
    {"RENACA", true, NULL, 3500, 700, 0, 2000, "NORTE VIÑA", false, 35},
    {"FORTUNA", false, NULL, 0, 0, 0, 0, "CARTAS", false, 36},
    {"MC DONALD (PAGAR $1000)", false, NULL, 1000, 0, 0, 0, "PAGAR", false, 37},
    {"DUNAS DE CONCON", true, NULL, 4000, 1000, 0, 2000, "NORTE VIÑA", false, 38},
    {"CHILQUINTA (LUZ)", false, NULL, 2000, 50, 0, 0, "COMPAÑIAS", false, 39}};

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

void inicializarTablero(partidaGlobal *partida, TipoPropiedad propiedades[NUM_PROPIEDADES]) {
    partida->tablero = malloc(NUM_PROPIEDADES * sizeof(TipoCasilla));
    if (partida->tablero == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el tablero.\n");
        return;
    }

    for (int i = 0; i < NUM_PROPIEDADES; i++) {
        TipoCasilla *casilla = (TipoCasilla *)malloc(sizeof(TipoCasilla));
        strcpy(casilla->nombre_casilla, propiedades[i].nombre);

        if (strcmp(propiedades[i].sector, "CARTAS") == 0) {
            casilla->tipo = 'C';
            casilla->propiedad = &propiedades[i];
        } else if (strcmp(propiedades[i].sector, "PAGAR") == 0) {
            casilla->tipo = 'I';
            casilla->propiedad = &propiedades[i];
        } else if (strcmp(propiedades[i].sector, "NEUTRO") == 0) {
            casilla->tipo = 'N';
            casilla->propiedad = &propiedades[i];
        } else if (strcmp(propiedades[i].sector, "SALIDA") == 0) {
            casilla->tipo = 'E';  // Tipo "E" para salida
            casilla->propiedad = &propiedades[i];
        } else if (strcmp(propiedades[i].sector, "METRO") == 0) {
            casilla->tipo = 'M';  // Tipo "M" para metro
            casilla->propiedad = &propiedades[i];
        } else if (strcmp(propiedades[i].sector, "CARCEL") == 0) {
            casilla->tipo = 'J';  // Tipo "J" para cárcel
            casilla->propiedad = &propiedades[i];
        } else if (strcmp(propiedades[i].sector, "COMPAÑIAS") == 0) {
            casilla->tipo = 'S';  // Tipo "S" para servicio
            casilla->propiedad = &propiedades[i];
        } else {
            casilla->tipo = 'P';  // Tipo "P" para propiedad
            casilla->propiedad = &propiedades[i];
        }

        int index = propiedades[i].coordenadaX; // Coordenada ya enumerada de 0 a 39
        partida->tablero[index] = casilla;
    }
}

void presioneEnter() {
    //printf("Presione Enter para continuar...");
    // Limpiar BUFFER
    while (getchar() != '\n');
}

//Función para limpiar la pantalla
void limpiar_pantalla() {
    system("clear");
}

// Inicializar semilla aleatoria, esto permitirá que cada vez que se corra
// el programa, los resultados sean diferentes
void inicializar_aleatoriedad(){
    srand(time(NULL));
}

// Función para tirar un dado
int tirar_dado(){
    int dado = (rand() % 6) + 1; // Genera un número aleatorio entre 1 y 6

    return dado;
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

//FUNCIONES PARA LA HIPOTECA DE PROPIEDADES
void hipotecar_propiedad(TipoJugador *jugador, TipoPropiedad *propiedad)
{
    if (propiedad->propietario == jugador)
    {
        if (!propiedad->hipotecado)
        {
            int valor_hipoteca = (propiedad->precio / 2) + ((propiedad->precio_casa * propiedad->casas) /2);
            printf("¿Quieres hipotecar la propiedad %s por %d? (s/n): ", propiedad->nombre, valor_hipoteca);
            char respuesta;
            scanf(" %c", &respuesta);
            if (respuesta == 's' || respuesta == 'S')
            {
                propiedad->hipotecado = true;
                jugador->dinero += valor_hipoteca;
                printf("Felicidades %s, has hipotecado la propiedad %s y has recibido %d!\n", jugador->nombre_jugador, propiedad->nombre, valor_hipoteca);

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
        int precio_venta = ((propiedad->precio / 2) + ((propiedad->precio_casa * propiedad->casas) /2)) + (precio_venta * 0.1);
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

bool verificarHipotecas(List *propiedades, int deuda){
    int recaudacion = 0;
    TipoPropiedad *propiedad = list_first(propiedades);
    while(propiedad != NULL){
        if(propiedad->hipotecado == false) 
            recaudacion += (propiedad->precio / 2) + ((propiedad->precio_casa * propiedad->casas) /2);
        propiedad = list_next(propiedades);
    }

    if(recaudacion < deuda){
        printf("No es posible pagar tu deuda\n");
        printf("%d \n", recaudacion);
        return true;
    }
    return false;
}

void menuHipotecas(TipoJugador *jugador, int deuda){
    printf("Bienvenido al menú de hipotecas\n");

    TipoPropiedad *propiedad = list_first(jugador->propiedades);
    while(jugador->dinero < deuda){
        printf("Debes hipotecar, no tienes dinero suficiente para tu deuda\n");
        hipotecar_propiedad(jugador, propiedad);
        propiedad = next_circular(jugador->propiedades);
    }
}

bool verificar_propiedades_bancaRota(TipoJugador *jugador, int deuda){
    if(!list_isEmpty(jugador->propiedades)){
        if(verificarHipotecas(jugador->propiedades, deuda)){
            return true;
        } else{
            menuHipotecas(jugador, deuda);
            return false;
        }
    } else return true;
}

void eliminarJugador(TipoJugador *jugador, partidaGlobal *partida){
    TipoPropiedad *propiedad = list_first(jugador->propiedades);
    while(propiedad != NULL){
        propiedad->propietario = NULL;
        propiedad = list_next(jugador->propiedades);
    }
    list_popCurrent(partida->turnos);
}

void verificar_bancaRota(TipoJugador *jugador, int dinero_a_pagar, partidaGlobal *partida){
    if(jugador->dinero - dinero_a_pagar < 0){
        if(verificar_propiedades_bancaRota(jugador, dinero_a_pagar)){
            printf("Perdiste, estas en bancarrota\n");
            eliminarJugador(jugador, partida);
        }
    }
}

// FUNCIONES DE COMPRA Y VENTA DE PROPIEDADES

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

// TESTEO DEL JUEGO FUNCIONES
//  Función exclusiva para programadores para probar comprar_propiedad y
//  comprar_casas
void inicializarListasYColas(partidaGlobal *partida){
    partida->jugadores = list_create();
    partida->turnos = list_create();
    partida->fortuna = inicializar_fortuna();
    partida->arca_comunal = inicializar_arca_comunal();
    TipoPropiedad *propiedades = inicializar_propiedades();
    inicializarTablero(partida, propiedades);
}

void mostrarTipo(char tipo){
    printf("Tipo: ");

    if(tipo == 'P') printf("Propiedad\n");
    else if(tipo == 'S') printf("Servicio\n");
    else if(tipo == 'J') printf("Carcel\n");
    else if(tipo == 'M') printf("Metro\n");
    else if(tipo == 'E') printf("Salida\n");
    else if(tipo == 'N') printf("Neutro\n");
    else if(tipo == 'I') printf("Impuesto\n");
    else if(tipo == 'C') printf("Carta\n");
}

void mostrar_estado_jugador(TipoJugador *jugador, partidaGlobal *partida){
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║               ESTADO DEL JUGADOR                 ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("════════════════════════════════════════════════════");
    printf("\nJugador: %s\n", jugador->nombre_jugador);
    printf("Dinero actual: %d\n", jugador->dinero);
    printf("Turnos de penalización en cárcel: %d\n", jugador->penalizacion);
    printf("Posición en el tablero: %d\n", jugador->posicion);
    printf("Te encuentras en: %s\n", partida->tablero[jugador->posicion]->nombre_casilla);
    mostrarTipo(partida->tablero[jugador->posicion]->tipo);
    printf("Propietario: ");
    if(partida->tablero[jugador->posicion]->propiedad->propietario != NULL)
        printf("%s\n", partida->tablero[jugador->posicion]->propiedad->propietario->nombre_jugador);
    else printf("-\n");
    printf("════════════════════════════════════════════════════\n\n");
}

void moverJugador(TipoJugador *jugador, int avance){
    int nuevaPosicion = (jugador->posicion + avance) % 40;
        if (nuevaPosicion < jugador->posicion) {  // Pasó por la casilla de salida
            jugador->dinero += 2000;
            printf("Pasaste por la salida y recibiste $2000!. Dinero actual: $%d\n", jugador->dinero);
        }
        jugador->posicion = nuevaPosicion;
        printf("¡Te moviste a la casilla %d!\n", jugador->posicion);
}

void menu_de_propiedades(TipoJugador *jugador, TipoPropiedad *propiedad){
    printf("Haz caido en tu propiedad\n");
    int opcion;

    do{
        printf("¿Qué acción deseas realizar?\n");
        printf("1. Comprar casas\n");
        printf("2. Vender casas\n");
        printf("3. Continuar\n");
        scanf("%d", &opcion);
        switch(opcion){
            case 1:
                comprar_casas(jugador, propiedad);
                break;
            case 2:
                vender_casas(jugador, propiedad);
                break;
            case 3:
                return;
            default:
                printf("Opción ingresada no es válida\n");
                break;
        }
    } while(opcion != 3);
}

void casoPropiedad(TipoJugador *jugador, TipoPropiedad* propiedad, partidaGlobal *partida){
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
            printf("Haz decidido no comprar la propiedad %s.\n", propiedad->nombre);
          }
        } else {
          // El jugador no tiene suficiente dinero para comprar la propiedad
          printf("%s, no tienes suficiente dinero para comprar la propiedad %s.\n",
                 jugador->nombre_jugador, propiedad->nombre);
        }
      } else {
        // La propiedad ya tiene un dueño
        if(strcmp(propiedad->propietario->nombre_jugador,jugador->nombre_jugador) == 0){
            menu_de_propiedades(jugador, propiedad);
        } else {
            printf("La propiedad %s tiene un dueño, debes pagar renta.\n", propiedad->nombre);
            verificar_bancaRota(jugador, propiedad->renta, partida);
            if(jugador != NULL){
                jugador->dinero -= propiedad->renta;
                propiedad->propietario->dinero += propiedad->renta;
            }
        }
      }
      presioneEnter();
}

void casoCarta(TipoJugador *jugador, partidaGlobal *partida, TipoCasilla *casilla)
{
    TipoCarta *carta_sacada = malloc(sizeof(TipoCarta));
    if(strcmp(casilla->nombre_casilla, "FORTUNA") == 0)
    {
        carta_sacada = queue_remove(partida->fortuna);
        queue_insert(partida->fortuna, carta_sacada);
        printf("¡CARTA DE LA FORTUNA!\n");
    }
    else
    {
        carta_sacada = queue_remove(partida->arca_comunal);
        queue_insert(partida->arca_comunal, carta_sacada);
        printf("¡CARTA DE LA ARCA COMÚNAL!\n");
    }
    printf("Carta sacada: %s\n", carta_sacada->descripcion);
    
    jugador->dinero += carta_sacada->cambio_dinero;

    if(carta_sacada->cambio_posicion != -1)
        jugador->posicion = carta_sacada->cambio_posicion;
    if(carta_sacada->cambio_penalizacion != -1)
        jugador->penalizacion = carta_sacada->cambio_penalizacion;
    free(carta_sacada);
}

void casoPagar(TipoJugador *jugador, partidaGlobal *partida, TipoCasilla *casilla){
    printf("Haz caido en una casilla de impuestos\n");
    int impuesto = casilla->propiedad->precio;
    verificar_bancaRota(jugador, impuesto, partida);
    if(jugador != NULL) jugador->dinero -= impuesto;
}

void casoNoPropiedad(TipoJugador *jugador, TipoPropiedad* propiedad, partidaGlobal *partida){
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
            printf("Haz decidido no comprar la propiedad %s.\n", propiedad->nombre);
          }
        } else {
          // El jugador no tiene suficiente dinero para comprar la propiedad
          printf("%s, no tienes suficiente dinero para comprar la propiedad %s.\n",
                 jugador->nombre_jugador, propiedad->nombre);
        }
      } else {
        // La propiedad ya tiene un dueño
        if(strcmp(propiedad->propietario->nombre_jugador,jugador->nombre_jugador) == 0){
            printf("Esta propiedad es tuya\n");
        } else {
            printf("La propiedad %s tiene un dueño, debes pagar renta.\n", propiedad->nombre);
            verificar_bancaRota(jugador, propiedad->renta, partida);
            if(jugador != NULL){
                jugador->dinero -= propiedad->renta;
                propiedad->propietario->dinero += propiedad->renta;
            }
        }
      }
      presioneEnter();
}

void casoCarcel(TipoJugador *jugador, partidaGlobal *partida, TipoCasilla *casilla){
    printf("Haz sido detenido, vas a la carcel\n");
    jugador->posicion = 10;
    jugador->penalizacion = 3;
}


void ejecutarAccionCasilla(TipoJugador *jugador, TipoCasilla *casilla, partidaGlobal *partida){
    if(casilla->tipo == 'P') casoPropiedad(jugador, casilla->propiedad, partida);
    else if(casilla->tipo == 'C') casoCarta(jugador, partida, casilla);
    else if(casilla->tipo == 'I') casoPagar(jugador, partida, casilla);
    else if(casilla->tipo == 'M' || casilla->tipo == 'S')  casoNoPropiedad(jugador, casilla->propiedad, partida);
    else if(casilla->tipo == 'J')  casoCarcel(jugador, partida, casilla);
}


void turnoJugador(TipoJugador* jugador, partidaGlobal *partida){

    limpiar_pantalla();
    printf("%s ES TU TURNO!\n", jugador->nombre_jugador);

    if(jugador->penalizacion > 0){
        printf("Estas en la cárcel, tienes %d turnos de penalización restantes\n", jugador->penalizacion);
        jugador->penalizacion --;
        return;
    }
    
    printf("Presiona enter para lanzar los dados!\n");
    presioneEnter();

    int dado1 = tirar_dado();
    int dado2 = tirar_dado();
    int totalDados = dado1 + dado2;

    printf("¡Haz lanzado los dados: %d y %d (Total: %d)!\n", dado1, dado2, totalDados);

    moverJugador(jugador, totalDados);

    TipoCasilla *casillaActual = partida->tablero[jugador->posicion];

    mostrar_estado_jugador(jugador, partida);
    
    ejecutarAccionCasilla(jugador, casillaActual, partida);

    mostrar_estado_jugador(jugador, partida);
}

TipoJugador *verificarGanador(List *jugadores){
    if(list_size(jugadores) != 1){
        return NULL;
    }
    return list_first(jugadores);
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

void iniciarPartida(){
    partidaGlobal *partida = (partidaGlobal*)malloc(sizeof(partidaGlobal));
    if(partida == NULL){
        printf("Error al inicializar la partida\n");
        return;
    }

    inicializarListasYColas(partida);
    int num_jugadores = solicitar_jugadores();
    asignar_jugadores(partida, num_jugadores);

    for (void* jugador = list_first(partida->jugadores); jugador != NULL; jugador = list_next(partida->jugadores)) {
        list_pushBack(partida->turnos, jugador);
    }

    inicializar_aleatoriedad();
    TipoJugador* jugador_actual = (TipoJugador*)list_first(partida->turnos);

    while(true)
    {
        turnoJugador(jugador_actual, partida);
        printf("Presione enter para continuar\n");
        presioneEnter();
        jugador_actual = next_circular(partida->turnos);

        TipoJugador* ganador = verificarGanador(partida->turnos);
        if (ganador != NULL) {
           printf("¡El jugador %s es el ganador del juego!\n", ganador->nombre_jugador);
            break;
        }
    }
    //liberarMemoria
}
