#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "tdas/list.h"
#include "tdas/queue.h" 
// Definir numero de casillas y numero de cartas
#define NUM_CASILLAS 40     
#define NUM_CARTAS 12
#define SIZE 11

// STRUCTS

// Struct para cada jugador
struct TipoJugador{
    int dinero;                    // Dinero del jugador
    int penalizacion;              // Penalizacion de la carcel hacia al jugador
    char nombre_jugador[40];       // Nombre del jugador
    int posicion;                  // Posicion del jugador
    List *propiedades;             // Lista de las propiedades del jugador
    int cont[10];                  // Contador de metro, servicios y propiedades por sector
    // Para el contador se distribuye de la siguiente forma:
            // 0: Metros
            // 1: Servicios
            // 2: Playa Ancha
            // 3: Valparaíso
            // 4: Curauma
            // 5: Costa Valpo
            // 6: Interior
            // 7: Interior II
            // 8: Viña del Mar
            // 9: Norte Viña
};

// Struct para cada casilla
struct TipoCasilla{
    char nombre[50];               // Nombre de la casilla
    char tipo;                     // Tipo de casilla
    TipoJugador* propietario;      // Jugador dueño de la propiedad (Si es propiedad)
    int precio;                    // Precio de la propiedad
    int renta;                     // Renta de la propiedad
    int casas;                     // Cantidad de casas de la propiedad 
    int precio_casa;               // Precio de cada casa
    char sector[50];               // Sector de la propiedad
    int indiceSector;              // Indice del sector de la propiedad para el contador
    int cant_por_sector;           // Cantidad de propiedades del sector
    bool hipotecado;               // Comprobar si la propiedad esta hipotecada
    int num_casilla;               // numero de casilla en el tablero
}; 

// Struct para cada carta
struct TipoCarta{
    char descripcion[200];         // Descripción de la carta
    int cambio_dinero;             // Cambio de dinero que provoca la carta
    int cambio_posicion;           // Cambio de posicion que provoca la carta
    int cambio_penalizacion;       // Penalización que entrega la carta
};

// Struct de la partida
struct PartidaGlobal{
    TipoCasilla **tablero; // Arreglo de casillas del tablero
    List *jugadores;       // Lista de jugadores de la partida, utilizada
                           // para controlar los turnos de manera circular
    Queue* fortuna;        // Cola para controlar las cartas de la fortuna
    Queue* arca_comunal;   // Cola para controlar las cartas de la arca comunal
};

// Definición de structs
typedef TipoCasilla TipoCasilla;
typedef TipoCarta TipoCarta;
typedef TipoJugador TipoJugador;
typedef PartidaGlobal PartidaGlobal;

// INICIACIÓN DEL JUEGO

// Función que define todas las cartas de tipo Fortuna
TipoCarta *fortuna()
{
    static TipoCarta fortuna[NUM_CARTAS] = {
    {"AVANCE HASTA LA CASILLA DE SALIDA, COBRE $2000", 0, 0, -1},
    {"AVANCE A DUNAS DE CONCON", 0, 39, -1},
    {"EL BANCO PAGA DIVIDENDOS DE $500", 500, -1, -1},
    {"AVANCE A ESCUELA ECONOMÍA PUCV", 0, 19, -1},
    {"VAYA DIRECTAMENTE A LA CÁRCEL", 0, 10, 3},
    {"USTED HA SIDO ELEGIDO PRESIDENTE DEL CENTRO DE ALUMNOS, PAGUE $1000", -1000, -1, -1},
    {"ENHORABUENA, HAN CARGADO LA BAES! RECIBA $1500.", 1500, -1, -1},
    {"AVANCE A ESTACIÓN MIRAMAR", 0, 35, -1},
    {"AVANCE A ESTACIÓN LIMACHE",0, 25, -1},
    {"AVANCE A ESTACIÓN PUERTO",0, 5, -1},
    {"AVANCE A ESTACIÓN BARÓN",0, 15, -1},
    {"PISASTE UN CHARCO Y NECESITAS CALCETINES NUEVOS, PAGUE $1000 ", -1000, -1, -1}
    };
    return fortuna;
}

// Función que define todas las cartas del arca comunal
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
    {"NECESITAS COMPRAR CAFE. PAGUE $1200", -1200, -1, -1},
    {"EL METRO SE QUEDA DETENIDO EN ESTACION VALENCIA Y LLEGAS TARDE AL TRABAJO, PAGUE 500", -500, -1, -1}
    };
    return arca_comunal;
}

// Función para randomizar las cartas, asegurando un orden aleatorio
void randomizar_cartas(TipoCarta *cartas, int size)
{
    // Se recorre el arreglo desde el final al inicio
    for (int pos_original = size - 1; pos_original > 0; pos_original--) 
    {
        // Se genera un indice aleatorio entre 0 y pos_original
        int nueva_pos = rand() % (pos_original + 1);

        // Se intercambian los elementos en las posiciones pos_original y nueva_pos
        TipoCarta aux = cartas[pos_original];
        cartas[pos_original] = cartas[nueva_pos];
        cartas[nueva_pos] = aux;
    }
}

// Función para crear las cartas con memoria dinamica
TipoCarta* crear_copia_cartas(TipoCarta *cartas, int size)
{
    // Se reserva memoria
    TipoCarta *cartas_copia = malloc(sizeof(TipoCarta) * size);
    // Se copia el arreglo de cartas en la copia
    for (int i = 0; i < size; i++)
        cartas_copia[i] = cartas[i];
    // Se retorna la copia
    return cartas_copia;
}

// Función que transforma un arreglo en una cola
void arreglo_a_cola(TipoCarta *cartas, int size, Queue *cola)
{
    // Se recorre el arreglo y se inserta en la cola
    for(int i = 0; i < size; i++)
        queue_insert(cola, &cartas[i]);
}

// Función para inicializar las cartas de la fortuna
Queue* inicializar_fortuna()
{
    // Se crea el arreglo de cartas
    TipoCarta *cartas_originales = fortuna();
    // Se realiza la copia con memoria dinamica
    TipoCarta *cartas_copia = crear_copia_cartas(cartas_originales, NUM_CARTAS);
    // Se reparten las cartas de manera aleatoria
    randomizar_cartas(cartas_copia, NUM_CARTAS);
    // Se crea la cola
    Queue *fortuna = queue_create();
    // Se rellena la cola
    arreglo_a_cola(cartas_copia, NUM_CARTAS, fortuna);
    // Se retorna la cola
    return fortuna;
}

// Función para inicializar las cartas del arca comunal
Queue* inicializar_arca_comunal()
{
    // Se crea el arreglo de cartas
    TipoCarta *cartas_originales = arca_comunal();
    // Se realiza la copia con memoria dinamica
    TipoCarta *cartas_copia = crear_copia_cartas(cartas_originales, NUM_CARTAS);
    // Se reparten las cartas de manera aleatoria
    randomizar_cartas(cartas_copia, NUM_CARTAS);
    // Se crea la cola
    Queue *arca_comunal = queue_create();
    // Se rellena la cola
    arreglo_a_cola(cartas_copia, NUM_CARTAS, arca_comunal);
    // Se retorna la cola
    return arca_comunal;
}

// Función para definir las casillas con todos sus campos
TipoCasilla *inicializar_casillas() {
  // Crear un array estático de TipoCasilla con todas las casillas iniciales
  static TipoCasilla casillas[NUM_CASILLAS] = {
    {"SALIDA", 'E', NULL, 0, 0, 0, 0, "SALIDA", -1, -1, false, 0},
    {"TORPEDERAS", 'P', NULL, 600, 40, 0, 500, "PLAYA ANCHA", 2, 2, false, 1},
    {"ARCA COMUNAL", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, -1, false, 2},
    {"SEDE ALIMENTOS PUCV", 'P', NULL, 800, 65, 0, 500, "PLAYA ANCHA", 2, 2, false, 3},
    {"SERVICIOS IMPUESTOS INTERNOS", 'I', NULL, 2000, 0, 0, 0, "PAGAR", -1, -1, false, 4},
    {"ESTACIÓN PUERTO", 'M', NULL, 2000, 250, 0, 0, "METRO", 0, -1, false, 5},
    {"PARQUE ITALIA", 'P', NULL, 1000, 120, 0, 500, "VALPARAÍSO", 3, 3, false, 6},
    {"FORTUNA", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, -1, false, 7},
    {"IBC PUCV", 'P', NULL, 1100, 150, 0, 500, "VALPARAÍSO", 3, 3, false, 8},
    {"AV. PEDRO MONT", 'P', NULL, 1200, 160, 0, 500, "VALPARAÍSO", 3, 3, false, 9},
    {"CARCEL", 'N', NULL, 0, 0, 0, 0, "NEUTRO", -1, -1,false, 10},
    {"LAGUNA VERDE", 'P', NULL, 1400, 200, 0, 1000, "CURAUMA", 4, 3, false, 11},
    {"CAMPUS CURAUMA PUCV", 'P', NULL, 1500, 235, 0, 1000, "CURAUMA", 4, 3, false, 12},
    {"ESVAL (COMPANIA AGUA)", 'S', NULL, 1500, 50, 0, 0, "COMPAÑIAS", 1, -1, false, 13},
    {"LAGO PEÑUELAS", 'P', NULL, 1650, 250, 0, 1000, "CURAUMA", 4, 3, false, 14},
    {"ESTACIÓN BARÓN", 'M', NULL, 2000, 250, 0, 0, "METRO", 0, -1, false, 15},
    {"MUELLE BARÓN", 'P', NULL, 1800, 280, 0, 1000, "COSTA VALPO", 5, 3, false, 16},
    {"CALETA PORTALES", 'P', NULL, 1900, 290, 0, 1000, "COSTA VALPO", 5, 3, false, 17},
    {"ARCA COMUNAL", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, -1, false, 18},
    {"ESCUELA ECONOMÍA PUCV", 'P', NULL, 2000, 320, 0, 1000, "COSTA VALPO", 5, 3, false, 19},
    {"RELOJ DE FLORES", 'N', NULL, 0, 0, 0, 0, "NEUTRO", -1, -1, false, 20},
    {"SEDE MECÁNICA PUCV", 'P', NULL, 2200, 375, 0, 1500, "INTERIOR", 6, 3, false, 21},
    {"QUILPUE", 'P', NULL, 2400, 400, 0, 1500, "INTERIOR", 6, 3, false, 22},
    {"FORTUNA", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, -1, false, 23},
    {"VILLA ALEMANA", 'P', NULL, 2400, 400, 0, 1500, "INTERIOR", 6, 3, false, 24},
    {"ESTACIÓN LIMACHE", 'M', NULL, 2000, 250, 0, 0, "METRO", 0, -1, false, 25},
    {"OLMUE", 'P', NULL, 2600, 440, 0, 1500, "INTERIOR II", 7, 3, false, 26},
    {"QUILLOTA", 'P', NULL, 2600, 440, 0, 1500, "INTERIOR II", 7, 3, false, 27},
    {"CHILQUINTA (COMPANIA LUZ)", 'S', NULL, 1500, 50, 0, 0, "COMPAÑIAS", 1, -1, false, 28},
    {"FACU. AGRONOMÍA PUCV", 'P', NULL, 2800, 480, 0, 1500, "INTERIOR II", 7, 3, false, 29},
    {"VAYA A CARCEL", 'J', NULL, 0, 0, 0, 0, "CARCEL", -1, -1, false, 30},
    {"MUELLE VERGARA", 'P', NULL, 3000, 520, 0, 2000, "VIÑA DEL MAR", 8, 3, false, 31},
    {"SEDE SAUSALITO PUCV", 'P', NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", 8, 3, false, 32},
    {"ARCA COMUNAL", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, -1, false, 33},
    {"15 NORTE", 'P', NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", 8, 3, false, 34},
    {"ESTACIÓN MIRAMAR", 'M', NULL, 2000, 250, 0, 0, "METRO", 0, -1, false, 35},
    {"RENACA", 'P', NULL, 3500, 700, 0, 2000, "NORTE VIÑA", 9, 2, false, 36},
    {"FORTUNA", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, -1, false, 37},
    {"MC DONALD", 'I', NULL, 1000, 0, 0, 0, "PAGAR", -1, -1, false, 38},
    {"DUNAS DE CONCON", 'P', NULL, 4000, 1000, 0, 2000, "NORTE VIÑA", 9, 2, false, 39}};
    

  return casillas;
}

// Función para inicializar el tablero
void inicializarTablero(PartidaGlobal *partida, TipoCasilla casillas[NUM_CASILLAS]) {
    // Se reserva memoria para el tablero
    partida->tablero = malloc(NUM_CASILLAS * sizeof(TipoCasilla *));
    if (partida->tablero == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el tablero.\n");
        return;
    }
    
    // Se recorre el arreglo de casillas definidas 
    for (int i = 0; i < NUM_CASILLAS; i++) {
        // Se reserva memoria para cada casilla
        TipoCasilla *casilla = (TipoCasilla *)malloc(sizeof(TipoCasilla));
        // Se copia el contenido de la casilla en el puntero
        casilla = &casillas[i];
        partida->tablero[casilla->num_casilla] = casilla;
    }
}

// Función para presionar enter para continuar
void presioneEnter() {
    while (getchar() != '\n');  // Descarta el caracter hasta leer un salto de linea
}

//Función para limpiar la pantalla
void limpiar_pantalla() {
    system("clear");
}

// Función para inicializar semilla aleatoria, esto permitirá que cada vez que 
// se corra el programa, los resultados sean diferentes
void inicializar_aleatoriedad(){
    srand(time(NULL));
}

// Función para tirar un dado
int tirar_dado(){
    int dado = (rand() % 6) + 1; // Genera un número aleatorio entre 1 y 6
    return dado;
}

// Función que inicializa el contador de struct TipoJugador en 0
void inicializar_contador(TipoJugador *jugador){
    for(int i = 0; i < 10; i++){
        jugador->cont[i] = 0;
    }
}

//Funcion para solicitar la cantidad de jugadores
int solicitar_jugadores()
{
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║               SELECCIÓN DE JUGADORES             ║\n");
    printf("╚══════════════════════════════════════════════════╝\n\n");
    // Creación de variable que contiene los jugadores
    int num_jugadores;
    do {
        printf("Ingrese el número de jugadores (2 a 4): ");
        scanf("%d", &num_jugadores); // Se leen los jugadores
        while (getchar() != '\n'); // Limpiar el buffer de entrada  
        if (num_jugadores < 2 || num_jugadores > 4)
            printf("¡El número de jugadores debe estar entre 2 y 4!\n\n");
    } while (num_jugadores < 2 || num_jugadores > 4);
    return num_jugadores; // Se retorna la cantidad de jugadores
}

// Función para asignar los jugadores de la partida
void asignar_jugadores(PartidaGlobal *partida, int num_jugadores) {
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

        // Se solicita y lee el nombre del jugador
        printf("Ingrese el nombre del jugador %d: ", i+1);
        fgets(jugador->nombre_jugador, sizeof(jugador->nombre_jugador), stdin);
        jugador->nombre_jugador[strcspn(jugador->nombre_jugador, "\n")] = '\0'; // Eliminar el salto de línea

        // Se inicializan las variables del jugador
        jugador->dinero = 15000;
        jugador->penalizacion = 0;
        jugador->posicion = 0;
        jugador->propiedades = list_create();
        inicializar_contador(jugador);

        // Agregar el jugador a la lista de jugadores de la partida
        list_pushBack(partida->jugadores, jugador);


    }
    // Verificar que se añadieron correctamente 
    int tamano = list_size(partida->jugadores);
    if (tamano == num_jugadores) {
        printf("Se han asignado correctamente los jugadores\n");
    } else {
        printf("No se han asignado correctamente los jugadores\n");
    }

}

// Función para hipotecar una propiedad
void hipotecar_propiedad(TipoJugador *jugador, TipoCasilla *propiedad)
{
    // Se comprueba que el jugador sea el propietario
    if (propiedad->propietario == jugador)
    {
        // Se comprueba si la propiedad está hipotecada
        if (!propiedad->hipotecado)
        {
            // Se determina el valor de la hipoteca
            int valor_hipoteca = (propiedad->precio / 2) + ((propiedad->precio_casa * propiedad->casas) /2);
            
            printf("¿\nQuieres hipotecar la propiedad %s por %d? (s/n): ", propiedad->nombre, valor_hipoteca);
            char respuesta;
            scanf(" %c", &respuesta);
            getchar();
            // Si la respuesta es afirmativa se hipoteca la propiedad
            if (respuesta == 's' || respuesta == 'S')
            {
                propiedad->hipotecado = true;
                jugador->dinero += valor_hipoteca;
                printf("\nFelicidades %s, has hipotecado la propiedad %s y has recibido %d!\n", jugador->nombre_jugador, propiedad->nombre, valor_hipoteca);

                printf("\nRecuerda que no podrás cobrar renta por %s mientras esté hipotecada.\n\n", propiedad->nombre);
            }
            else // Si la respuesta es negativa se cancela la hipoteca
            {
                printf("\nHaz decidido no hipotecar la propiedad %s.\n\n", propiedad->nombre);
                return;
            }
        }
        else // Si la propiedad ya esta hipotecada, se da un aviso
        {
            printf("\nLa propiedad %s ya está hipotecada.\n\n", propiedad->nombre);
        }
    }
    else // Si la propiedad no es del jugador, se da un aviso
    {
        printf("\nLa propiedad %s no está a tu nombre.\n\n", propiedad->nombre);
    }
}

// Función para recuperar una propiedad hipotecada
void recuperar_propiedad_hipotecada(TipoJugador *jugador, TipoCasilla *propiedad)
{
    // Se comprueba que este hipotecada y que sea del jugador
    if (propiedad->hipotecado && propiedad->propietario == jugador) {
        // Se determina el precio de la recuperación
        int precio_venta = (propiedad->precio / 2) + (propiedad->precio * 0.1) +((propiedad->precio_casa * propiedad->casas) / 2);
        
        
        // Preguntar al jugador si desea recuperar la propiedad
        printf("\n¿Quieres recuperar la propiedad %s por %d? (s/n): ", propiedad->nombre, precio_venta);
        char respuesta;
        scanf(" %c", &respuesta);
        getchar();

        if (respuesta == 's' || respuesta == 'S') {
            // Verificar si el jugador tiene suficiente dinero para recuperar la propiedad
            if (jugador->dinero >= precio_venta) {
                jugador->dinero -= precio_venta;
                propiedad->hipotecado = false;

                printf("\n%s, has recuperado la propiedad %s por %d!\n\n",
                       jugador->nombre_jugador, propiedad->nombre, precio_venta);
            } else {
                printf("\n%s, no tienes suficiente dinero para recuperar la propiedad %s.\n\n",
                       jugador->nombre_jugador, propiedad->nombre);
            }
        } else { // Si la respuesta es negativa se cancela la recuperación
            printf("\nNo se ha recuperado la propiedad %s.\n\n", propiedad->nombre);
        }

    } else {
        printf("\nEsta propiedad no esta hipotecada, o no es tuya.\n\n");
    } 
}

// Función para verificar si un jugador puede pagar su deuda con lo
// recaudado hipotecando sus propiedades
bool verificarRecaudacionMaxima(List *propiedades, int deuda){
    // Se inicia una variable de recaudación
    int recaudacion = 0;
    // Se recorre la lista de propiedades
    TipoCasilla *propiedad = list_first(propiedades);
    while(propiedad != NULL){
        if(propiedad->hipotecado == false) 
            // Se suma el valor de la hipoteca a la recaudación
            recaudacion += (propiedad->precio / 2) + ((propiedad->precio_casa * propiedad->casas) /2);
        propiedad = list_next(propiedades); // Se avanza a la siguiente propiedad
    }

    // Si la recaudación es menor a la deuda, se retorna true, lo que
    // indica que el jugador esta en bancarrota
    if(recaudacion < deuda){
        printf("No es posible pagar tu deuda\n");
        printf("%d \n", recaudacion);
        return true;
    }
    // Si es que puede cubrirla, se retorna false
    return false;
}

// Función de menú para hipotecar para cubrir deudas
void menuHipotecasYDeudas(TipoJugador *jugador, int deuda){
    printf("Bienvenido al menú de hipotecas para deudores\n");
    printf("Deberás hipotecar hasta cubrir tu deuda\n");

    // Se recorre la lista de propiedades del jugador
    TipoCasilla *propiedad = list_first(jugador->propiedades);
    // Ciclo se repetirá hasta que el jugador cubra su deuda
    while(jugador->dinero < deuda){
        printf("Debes hipotecar, no tienes dinero suficiente para tu deuda\n");
        hipotecar_propiedad(jugador, propiedad);
        // Se recorren sus propiedades de manera circular para el ciclo
        propiedad = next_circular(jugador->propiedades); 
    }
}

// Función para verificar si un jugador se puede salvar de la bancarrota
// modificando el estado de sus propiedades
bool verificar_propiedades_bancarrota(TipoJugador *jugador, int deuda){
    // Se verifica si el jugador tiene propiedades en su poder
    if(!list_isEmpty(jugador->propiedades)){
        // Se verifica si puede pagar su deuda hipotecando
        if(verificarRecaudacionMaxima(jugador->propiedades, deuda)){
            // Si no puede, se retorna true
            return true;
        } else{
            // Si puede, se le ingresa al menu de hipotecas para deudores
            // Una vez que pueda pagar su deuda, se retorna false
            menuHipotecasYDeudas(jugador, deuda);
            return false;
        }
    } else return true; // Si no tiene propiedades se retorna true
}

// Función para eliminar un jugador de la partida
void eliminarJugador(TipoJugador **jugador, PartidaGlobal *partida){
    // Se recorre la lista de propiedades del jugador
    printf("\n%s haz sido eliminado, estas en bancarrota.\n\n", (*jugador)->nombre_jugador);
    TipoCasilla *propiedad = list_first((*jugador)->propiedades);
    while(propiedad != NULL){
        // Se reinician los campos de la propiedas
        propiedad->propietario = NULL;
        propiedad->casas = 0;
        propiedad->hipotecado = false;
        propiedad = list_next((*jugador)->propiedades);
    }
    // Se elimina el jugador de la lista de jugadores
    list_popCurrent(partida->jugadores);
    (*jugador) = NULL;
}

// Función para verificar si un jugador esta en bancarrota
void verificar_bancarrota(TipoJugador *jugador, int dinero_a_pagar, PartidaGlobal *partida){
    // Se verifica si el jugador no tiene dinero para pagar su deuda
    if(jugador->dinero - dinero_a_pagar < 0){
        // Se verifica si puede pagar su deuda hipotecando
        if(verificar_propiedades_bancarrota(jugador, dinero_a_pagar)){
            // Si no puede, el jugador se elimina de la partida
            eliminarJugador(&jugador, partida);
        }
    }
}

bool es_propietario_de_sector(TipoJugador *jugador, TipoCasilla *propiedad){
    return jugador->cont[propiedad->indiceSector] == propiedad->cant_por_sector;
}

// Función para comprar casas en una propiedad
void comprar_casas(TipoJugador *jugador, TipoCasilla *propiedad) {
    // Se verifica que sea una propiedad en la cual se puedan comprar casas
    if(propiedad->tipo != 'P'){
        printf("No se pueden comprar casas para este tipo de propiedad\n");
        return;
    }

    // Se verifica que el jugador posea todo el sector para poder comprar/vender casas
    if(!es_propietario_de_sector(jugador, propiedad)){
        printf("No eres propietario del sector, no puedes administrar las casas\n");
        return;
    }
    
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

// Función para vender casa
void vender_casas(TipoJugador *jugador, TipoCasilla *propiedad) {
    // Se verifica que sea una propiedad en la cual se puedan vender casas
    if(propiedad->tipo != 'P'){
        printf("No se pueden vender casas para este tipo de propiedad\n");
        return;
    }

    // Se verifica que el jugador posea todo el sector para poder comprar/vender casas
    if(!es_propietario_de_sector(jugador, propiedad)){
        printf("No eres propietario del sector, no puedes administrar las casas\n");
        return;
    }
    
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

// Función para inicializar los campos necesarios para la partida
void inicializarCamposNecesarios(PartidaGlobal *partida){
    // Se crea la lista de jugadores
    partida->jugadores = list_create();         
    // Se inicializa la cola de Fortuna
    partida->fortuna = inicializar_fortuna();
    // Se inicializa la cola de Arca comunal
    partida->arca_comunal = inicializar_arca_comunal();
    // Se inicializan las casillas
    TipoCasilla *propiedades = inicializar_casillas();
    // Se inicializa el tablero
    inicializarTablero(partida, propiedades);
}

// Función para mostrar el tipo de casilla
void mostrarTipo(char tipo){
    printf("Tipo: ");

    // Dependiendo del campo "tipo" se imprime el caso debido
    if(tipo == 'P') printf("Propiedad\n");
    else if(tipo == 'S') printf("Servicio\n");
    else if(tipo == 'J') printf("Carcel\n");
    else if(tipo == 'M') printf("Metro\n");
    else if(tipo == 'E') printf("Salida\n");
    else if(tipo == 'N') printf("Neutro\n");
    else if(tipo == 'I') printf("Impuesto\n");
    else if(tipo == 'C') printf("Carta\n");
}

// Función para mostrar el estado del jugador
void mostrar_estado_jugador(TipoJugador *jugador, PartidaGlobal *partida){
    printf("Posición en el tablero: %02d\n", jugador->posicion);
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║               ESTADO DEL JUGADOR                 ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("════════════════════════════════════════════════════");
    printf("\nJugador: %s\n", jugador->nombre_jugador);
    printf("Dinero actual: %d\n", jugador->dinero);
    printf("Turnos de penalización en cárcel: %d\n", jugador->penalizacion);
    printf("Te encuentras en: %s\n", partida->tablero[jugador->posicion]->nombre);
    mostrarTipo(partida->tablero[jugador->posicion]->tipo);
    printf("Propietario: ");
    if(partida->tablero[jugador->posicion]->propietario != NULL)
        printf("%s\n", partida->tablero[jugador->posicion]->propietario->nombre_jugador);
    else printf("-\n");
    printf("════════════════════════════════════════════════════\n\n");
}

// Función para mover un jugador por el tablero
void moverJugador(TipoJugador *jugador, int avance){
    // Se obtiene la nueva posición dentro de las 40 casillas
    int nuevaPosicion = (jugador->posicion + avance) % 40;
        if (nuevaPosicion < jugador->posicion) {  // Pasó por la casilla de salida
            jugador->dinero += 2000;
            printf("Pasaste por la salida y recibiste $2000!. Dinero actual: $%d\n", jugador->dinero);
        }
        jugador->posicion = nuevaPosicion;
        printf("¡Te moviste a la casilla %d!\n", jugador->posicion);
}

// Función para mostrar los detalles de la propiedad
void imprimirDetallesPropiedad(TipoCasilla* propiedad) {
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║            DETALLES DE LA PROPIEDAD              ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("Nombre: %s\n", propiedad->nombre);
    printf("Sector: %s\n", propiedad->sector);
    printf("Precio: $%d\n", propiedad->precio);
    printf("Renta: $%d\n", propiedad->renta);
    if(propiedad->hipotecado == true)
        printf("Hipotecada: Sí\n");
    else
        printf("Hipotecada: No\n");
    if(propiedad->precio_casa > 0){
        printf("Precio por casa: $%d\n", propiedad->precio_casa);
        printf("Numero de casas: %d\n", propiedad->casas);
    }
    printf("Posición: %d\n\n", propiedad->num_casilla);
}

void rellenar_lista_turnos_subasta(List *turnos_subasta, List *jugadores){
    TipoJugador *jugador = list_current(jugadores);
    char *aux = jugador->nombre_jugador;

    list_pushBack(turnos_subasta, jugador);
    jugador = next_circular(jugadores);
    while(strcmp(aux, jugador->nombre_jugador) != 0){
        list_pushBack(turnos_subasta, jugador);
        jugador = next_circular(jugadores);
    }
}

void mostrarDetalles(TipoCasilla *propiedad){
    if(propiedad->tipo == 'P') imprimirDetallesPropiedad(propiedad);
    else if(propiedad->tipo == 'M') imprimirDetallesMetro(propiedad);
    else if(propiedad->tipo == 'S') imprimirDetallesServicio(propiedad);
}

void subasta_de_propiedades(TipoCasilla *propiedad, PartidaGlobal *partida){
    printf("BIENVENIDOS A LA SUBASTA\n");
    printf("Propiedad a subastar en esta ocasión: \n");

    mostrarDetalles(propiedad);
    printf("\nPresiona enter para comenzar la subasta\n");
    presioneEnter();
    
    int dinero_aux;
    int dinero_ofrecido = (propiedad->precio / 2);
    char opcion; 
    
    List* turnos_subasta = list_create();
    rellenar_lista_turnos_subasta(turnos_subasta, partida->jugadores);
    TipoJugador *jugador_de_turno = list_first(turnos_subasta);
    
    while(list_size(turnos_subasta) > 1){
        limpiar_pantalla();
        mostrarDetalles(propiedad);
        printf("Dinero ofrecido: %d\n", dinero_ofrecido);
        
        printf("Turno de %s\n", jugador_de_turno->nombre_jugador);
        printf("Dinero actual: %d\n", jugador_de_turno->dinero);
        
        if(jugador_de_turno->dinero < dinero_ofrecido){
            printf("No tienes dinero suficiente para ofrecer\n");
            list_popCurrent(turnos_subasta);
            continue;
        }
        
        printf("¿%s deseas pujar por la propiedad? (s/n)\n", jugador_de_turno->nombre_jugador);
        scanf(" %c", &opcion);
        getchar();
        
        if(opcion == 's' || opcion == 'S'){
            while(true){
                printf("¿Cuánto dinero deseas ofrecer?\n");
                scanf(" %d", &dinero_aux);
    
                if(dinero_aux <= dinero_ofrecido || dinero_aux > jugador_de_turno->dinero){
                    printf("No puedes ofrecer esa cantidad de dinero\n");
                    printf("¿Deseas volver a intentarlo? (s/n)\n");
                    scanf(" %c", &opcion);
                    getchar();
                    
                    if(opcion == 'n' || opcion == 'N'){
                        list_popCurrent(turnos_subasta);
                        jugador_de_turno = list_current(turnos_subasta);
                        break;
                    }
                } else {
                    dinero_ofrecido = dinero_aux;
                    jugador_de_turno = next_circular(turnos_subasta);
                    break;
                }
            }            
        } else{
            list_popCurrent(turnos_subasta);
            jugador_de_turno = list_current(turnos_subasta);
        }
    }

    TipoJugador *ganador = list_first(turnos_subasta);
    list_clean(turnos_subasta);
    printf("\n¡FELICIDADES %s HAS GANADO LA SUBASTA!\n\n", ganador->nombre_jugador);
    ganador->dinero -= dinero_ofrecido;
    propiedad->propietario = ganador;
    ganador->cont[propiedad->indiceSector]++;
    list_pushFront(ganador->propiedades, propiedad);
    mostrarDetalles(propiedad);
}

// Función para manejar cuando un jugador cae en una propiedad
void casoPropiedad(TipoJugador *jugador, TipoCasilla* propiedad, PartidaGlobal *partida){
    // Verificar que la propiedad no tiene dueño
    if (propiedad->propietario == NULL) {
        // Verificar que el jugador tiene suficiente dinero para comprar la
        // propiedad
        printf("\nDinero Actual: %d\n", jugador->dinero);
        mostrarDetalles(propiedad);  
        if (jugador->dinero >= propiedad->precio) {
          char respuesta;
          printf("¿Quieres comprar %s por %d? (s/n): ", propiedad->nombre,
                 propiedad->precio);
          scanf(" %c", &respuesta);
          getchar();
            
          if (respuesta == 's' || respuesta == 'S') {
            // Reducir el dinero del jugador en el precio de la propiedad
            jugador->dinero -= propiedad->precio;

            // Asignar la propiedad al jugador
            propiedad->propietario = jugador;

            // Aumentar la cantidad de propiedades de ese sector del usuario
            jugador->cont[propiedad->indiceSector]++;
              
            // Agregar la propiedad a la lista de propiedades del jugador
            list_pushFront(jugador->propiedades, propiedad);
            printf("\nFelicidades %s, has comprado la propiedad %s por %d!\n",
                   jugador->nombre_jugador, propiedad->nombre, propiedad->precio);

          } else {
            printf("\nHaz decidido no comprar la propiedad %s, por lo cual pasará a subasta\n", propiedad->nombre);
            printf("\nPresiona enter para continuar a la subasta...\n");
            presioneEnter();
            limpiar_pantalla();
            subasta_de_propiedades(propiedad, partida);
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
            
            printf("\nLa propiedad %s tiene un dueño, debes pagar una renta de: $%d.\n", propiedad->nombre, propiedad->renta);
            // Se verifica si el jugador tiene suficiente dinero para pagar la renta
            verificar_bancarrota(jugador, propiedad->renta, partida);
            if(jugador != NULL){
                jugador->dinero -= propiedad->renta;
                propiedad->propietario->dinero += propiedad->renta;
            }
        }
      }
}

// Función que enseña los detalles de una estación de metro
void imprimirDetallesMetro(TipoCasilla* propiedad) {
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║            DETALLES DE LA ESTACIÓN               ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("Nombre: %s\n", propiedad->nombre);
    printf("Precio: $%d\n", propiedad->precio);
    printf("Renta base: $%d\n", propiedad->renta);
    if(propiedad->hipotecado == true)
        printf("Hipotecada: Sí\n");
    else
        printf("Hipotecada: No\n");
    printf("*El valor de la renta se duplica según la cantidad de metros que controlas*\n\n");
}

// Función para manejar cuando un jugador cae en una casilla de metro
void casoMetro(TipoJugador *jugador, TipoCasilla* propiedad, PartidaGlobal *partida) {
    // Verificar que la propiedad no tiene dueño
    if (propiedad->propietario == NULL) {
        // Verificar que el jugador tiene suficiente dinero para comprar la propiedad
        if (jugador->dinero >= propiedad->precio) {
            char respuesta;
            printf("\nDinero Actual: %d", jugador->dinero);
            mostrarDetalles(propiedad);  // Asumiendo que esta función está definida
            printf("¿Quieres comprar %s por %d? (s/n): ", propiedad->nombre, propiedad->precio);
            scanf(" %c", &respuesta);
            getchar();

            if (respuesta == 's' || respuesta == 'S') {
                // Reducir el dinero del jugador en el precio de la propiedad
                jugador->dinero -= propiedad->precio;

                // Asignar la propiedad al jugador
                propiedad->propietario = jugador;

                // Agregar la propiedad a la lista de propiedades del jugador
                list_pushFront(jugador->propiedades, propiedad);  
                printf("Felicidades %s, has comprado %s por %d!\n", jugador->nombre_jugador, propiedad->nombre, propiedad->precio);

                // Incrementar el número de estaciones de metro del jugador
                jugador->cont[propiedad->indiceSector]++;

            } else {
                printf("\nHaz decidido no comprar la propiedad %s, por lo cual pasará a subasta\n", propiedad->nombre);
                printf("\nPresiona enter para continuar a la subasta...\n");
                presioneEnter();
                limpiar_pantalla();
                subasta_de_propiedades(propiedad, partida);
            }
        } else {
            // El jugador no tiene suficiente dinero para comprar la propiedad
            printf("%s, no tienes suficiente dinero para comprar %s.\n", jugador->nombre_jugador, propiedad->nombre);
        }
    } else {
        // La propiedad ya tiene un dueño
        if (strcmp(propiedad->propietario->nombre_jugador, jugador->nombre_jugador) == 0) {
            printf("Esta estación es tuya\n");
        } else {
            // Calcular la renta según la fórmula especificada
            double renta = propiedad->renta * pow(2.25, (propiedad->propietario->cont[propiedad->indiceSector] - 1));        
            renta = round(renta / 10) * 10;  // Redondear al décimo más cercano
            int renta_final = (int)renta;

            printf("%s tiene un dueño, debes pagar renta de: %d.\n", propiedad->nombre, renta_final);
            verificar_bancarrota(jugador, renta_final, partida);  // Asumiendo que esta función está definida
            if (jugador != NULL) {
                jugador->dinero -= renta_final;
                propiedad->propietario->dinero += renta_final;
            }
        }
    }
     presioneEnter();  // Asumiendo que esta función está definida
}

// Función que enseña los detales de una propiedad de servicio
void imprimirDetallesServicio(TipoCasilla* propiedad) {
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║            DETALLES DE EL SERVICIO               ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("Nombre: %s\n", propiedad->nombre);
    printf("Precio: $%d\n", propiedad->precio);
    if(propiedad->hipotecado == true)
        printf("Hipotecada: Sí\n");
    else
        printf("Hipotecada: No\n");
    printf("Renta 1 Servicio: 60 veces la suma de los dados\n");
    printf("Renta 2 Servicios: 150 veces la suma de los dados\n\n");
    
}

// Función para cuando un jugador cae en una casilla de servicio
void casoServicio(TipoJugador* jugador, TipoCasilla* propiedad, PartidaGlobal* partida, int dados) {
    // Verificar que la propiedad no tiene dueño
    if (propiedad->propietario == NULL) {
        // Verificar que el jugador tiene suficiente dinero para comprar la propiedad
        if (jugador->dinero >= propiedad->precio) {
            char respuesta;
            printf("\nDinero Actual: %d\n", jugador->dinero);
            mostrarDetalles(propiedad);
            printf("¿Quieres comprar la compañía %s por %d? (s/n): ", propiedad->nombre, propiedad->precio);
            scanf(" %c", &respuesta);
            getchar();

            if (respuesta == 's' || respuesta == 'S') {
                // Reducir el dinero del jugador en el precio de la propiedad
                jugador->dinero -= propiedad->precio;

                // Asignar la propiedad al jugador
                propiedad->propietario = jugador;

                // Agregar la propiedad a la lista de propiedades del jugador
                list_pushFront(jugador->propiedades, propiedad);
                printf("Felicidades %s, has comprado la compañía %s por %d!\n", jugador->nombre_jugador, propiedad->nombre, propiedad->precio);
                jugador->cont[propiedad->indiceSector]++;

            } else {
                printf("\nHaz decidido no comprar la propiedad %s, por lo cual pasará a subasta\n", propiedad->nombre);
                printf("\nPresiona enter para continuar a la subasta...\n");
                presioneEnter();
                limpiar_pantalla();
                subasta_de_propiedades(propiedad, partida);
            }
        } else {
            // El jugador no tiene suficiente dinero para comprar la propiedad
            printf("%s, no tienes suficiente dinero para comprar la compañía %s.\n", jugador->nombre_jugador, propiedad->nombre);
        }
    } else {
        // La propiedad ya tiene un dueño
        if (strcmp(propiedad->propietario->nombre_jugador, jugador->nombre_jugador) == 0) {
            printf("Esta compañía es tuya.\n");
        } else {
            if (propiedad->propietario->cont[propiedad->indiceSector] == 1) {
                int renta_servicio = 60 * dados;
                printf("%s tiene un dueño, debes pagar renta de: %d.\n", propiedad->nombre, renta_servicio);
                verificar_bancarrota(jugador, renta_servicio, partida);
                if (jugador != NULL) {
                    jugador->dinero -= renta_servicio;
                    propiedad->propietario->dinero += renta_servicio;
                }
            } else if (propiedad->propietario->cont[propiedad->indiceSector] >= 2) {
                int renta_servicio = 150 * dados;
                printf("%s tiene un dueño, debes pagar renta de: %d.\n", propiedad->nombre, renta_servicio);
                verificar_bancarrota(jugador, renta_servicio, partida);
                if (jugador != NULL) {
                    jugador->dinero -= renta_servicio;
                    propiedad->propietario->dinero += renta_servicio;
                }
            }
        }
    }
    presioneEnter();
}


// Función para manejar cuando un jugador cae en un retiro de carta
void casoCarta(TipoJugador *jugador, PartidaGlobal *partida, TipoCasilla *casilla, int dados)
{
    TipoCarta *carta_sacada;
    // Si es casilla de la fortuna, se retira una carta de fortuna
    if(strcmp(casilla->nombre, "FORTUNA") == 0)
    {
        // Se saca y luego se coloca detrás de la cola
        carta_sacada = queue_remove(partida->fortuna);
        queue_insert(partida->fortuna, carta_sacada);
        printf("\n¡CARTA DE LA FORTUNA!\n");
    }
    else // Si es de la arca comunal, se retira una carta de arca comunal
    {
        // Se saca y luego se coloca detrás de la cola
        carta_sacada = queue_remove(partida->arca_comunal);
        queue_insert(partida->arca_comunal, carta_sacada);
        printf("\n¡CARTA DE LA ARCA COMÚNAL!\n");
    }
    printf("\nCarta sacada: %s\n", carta_sacada->descripcion);

    // Se aplican los cambios correspondientes de la carta

    // Si debe pagar dinero, se verifica la bancarrota
    if(carta_sacada->cambio_dinero < 0)
        verificar_bancarrota(jugador, carta_sacada->cambio_dinero *(-1), partida);

    // Si el jugador entro en bancarrota, se finaliza la función
    if(jugador == NULL) return;

    // Si gana dinero, se le otorga
    jugador->dinero += carta_sacada->cambio_dinero;

    // Si se cambía de posición se le entrega dinero si paso por la salida, a excepción
    // de cuando es enviado directamente a la cárcel
    if(carta_sacada->cambio_posicion != -1){
        if(carta_sacada->cambio_penalizacion == -1){
            if(carta_sacada->cambio_posicion < jugador->posicion){
                printf("\nPasaste por la salida, cobra $2000\n");
                jugador->dinero += 2000;
            }
        }
        // Se le asigna su nueva posición
        jugador->posicion = carta_sacada->cambio_posicion;
        // Se obtiene su nueva casilla
        TipoCasilla *nuevaCasillaActual = partida->tablero[jugador->posicion];

        // Dependiendo el tipo de casilla se le entrega el menú correspondiente, en caso
        // de que esta se pueda comprar, como una propiedad o una estación del metro
        if(nuevaCasillaActual->tipo == 'P')
            casoPropiedad(jugador, nuevaCasillaActual, partida);
        else if(nuevaCasillaActual->tipo == 'M')
            casoMetro(jugador, nuevaCasillaActual, partida);
        else if(nuevaCasillaActual->tipo == 'S')
            casoServicio(jugador, nuevaCasillaActual, partida, dados);               
    }
    // Si le aplica penalización de cárcel, se le asigna
    if(carta_sacada->cambio_penalizacion != -1)
        jugador->penalizacion = carta_sacada->cambio_penalizacion;
}

// Función para manejar cuando un jugador cae en un cobro de impuestos
void casoImpuestos(TipoJugador *jugador, PartidaGlobal *partida, TipoCasilla *casilla){
    printf("\nHaz caido en una casilla de pago\n");
    printf("\n%s\n", casilla->nombre);
    printf("Dinero a pagar: %d\n", casilla->precio);
    // Se obtiene el impuesto del costo de la casilla
    // ya que así se manejan ese tipo de casillas
    int impuesto = casilla->precio;
    // Se verifica si lo puede pagar y se le cobra
    verificar_bancarrota(jugador, impuesto, partida);
    if(jugador != NULL) jugador->dinero -= impuesto;
}


// Función para manejar cuando el jugador cae en una casilla
// que lo envia directo a la carcel
void casoCarcel(TipoJugador *jugador, PartidaGlobal *partida){
    printf("Haz sido detenido, vas a la carcel\n");
    jugador->posicion = 10;
    jugador->penalizacion = 3;
}

// Función para ejecutar la acción de cada casilla correspondiente
void ejecutarAccionCasilla(TipoJugador *jugador, TipoCasilla *casilla, PartidaGlobal *partida, int dados){
    // Dependiendo del tipo de casilla se aplica un caso diferente
    if(casilla->tipo == 'P') casoPropiedad(jugador, casilla, partida);
    else if(casilla->tipo == 'C') casoCarta(jugador, partida, casilla, dados);
    else if(casilla->tipo == 'I') casoImpuestos(jugador, partida, casilla);
    else if(casilla->tipo == 'M')  casoMetro(jugador, casilla, partida);
    else if(casilla->tipo == 'S')  casoServicio(jugador, casilla, partida, dados);
    else if(casilla->tipo == 'J')  casoCarcel(jugador, partida);

    printf("\nPresiona enter para continuar...\n");
    presioneEnter();
    limpiar_pantalla();
}

// Función que muestra el menú de cada turno de jugador
void mostrarMenuDeTurno(){
    printf("¿Qué acción deseas realizar?\n\n");
    printf("1. Modificar campos de tus propiedades\n");
    printf("2. Intercambiar con otros jugadores\n");
    printf("3. Finalizar turno\n");
    printf("4. Declararte en bancarrota\n");
}

void mostrar_opciones_menu_propiedades(TipoCasilla *propiedad){
    limpiar_pantalla();
    mostrarDetalles(propiedad);
    printf("¿Qué deseas hacer?\n");
    printf("1. Comprar casas\n");
    printf("2. Vender casas\n");
    printf("3. Hipotecar propiedades\n");
    printf("4. Deshipotecar propiedades\n");
    printf("5. Salir de este menú\n");
}

TipoCasilla *elegir_propiedad(TipoJugador *jugador){
    if(!list_isEmpty(jugador->propiedades)){
        char respuesta;
        TipoCasilla *propiedad_elegida = list_first(jugador->propiedades);
        while(true){
            mostrarDetalles(propiedad_elegida);
            printf("¿Deseas elegir esta propiedad? (s/n): ");
            scanf(" %c", &respuesta);
            
            if (respuesta == 's' || respuesta == 'S') {
                return propiedad_elegida;
            } else {
                printf("¿Deseas continuar y ver la siguiente propiedad? (s/n): ");
                scanf(" %c", &respuesta);

                if (respuesta == 's' || respuesta == 'S'){
                    propiedad_elegida = next_circular(jugador->propiedades);
                    limpiar_pantalla();
                }
                else return NULL;
            }
        }
    } else printf("No posees ninguna propiedad\n");
    return NULL;
}

void menu_de_propiedades(TipoJugador *jugador){
    limpiar_pantalla();
    printf("Bienvenido al menú de propiedades\n\n");

    char opcion;
    do{
        printf("¿Con qué propiedad deseas trabajar?\n");
        TipoCasilla *propiedad = elegir_propiedad(jugador);
        if(propiedad == NULL){
            printf("No modificaste ninguna propiedad\n");
            return;
        }
        mostrar_opciones_menu_propiedades(propiedad);
        scanf(" %c", &opcion);

        switch(opcion){
            case '1':
                comprar_casas(jugador, propiedad);
                break;
            case '2':
                vender_casas(jugador, propiedad);
                break;
            case '3':
                hipotecar_propiedad(jugador, propiedad);
                break;
            case '4':
                recuperar_propiedad_hipotecada(jugador, propiedad);
                break;
            case '5':
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción escogida no es válida, porfavor intentelo nuevamente\n");
                break;
        }

        if(opcion != '5'){
            printf("¿Deseas continuar en este menú? (s/n): ");
            scanf(" %c", &opcion);

            if(opcion == 'n' || opcion == 'N') return;
        }
        limpiar_pantalla();
    } while(opcion != '5');
}

void rellenar_lista_opciones_intercambio(List *opciones, List *jugadores){
    TipoJugador *jugador = list_current(jugadores);
    char *aux = jugador->nombre_jugador;

    jugador = next_circular(jugadores);
    
    while(strcmp(aux, jugador->nombre_jugador) != 0){
        list_pushBack(opciones, jugador);
        jugador = next_circular(jugadores);
    }
}

void imprimirDetallesParaIntercambio(TipoJugador *jugador){
    printf("====================================\n");
    printf("Nombre: %s\n", jugador->nombre_jugador);
    printf("Dinero: %d\n", jugador->dinero);
    printf("Cantidad de metros: %d\n", jugador->cont[0]);
    printf("Cantidad de servicios: %d\n", jugador->cont[1]);
    printf("Cantidad de propiedades en Playa Ancha: %d\n", jugador->cont[2]);
    printf("Cantidad de propiedades en Valparaíso: %d\n", jugador->cont[3]);
    printf("Cantidad de propiedades en Curauma: %d\n", jugador->cont[4]);
    printf("Cantidad de propiedades en Costa Valpo: %d\n", jugador->cont[5]);
    printf("Cantidad de propiedades en Interior: %d\n", jugador->cont[6]);
    printf("Cantidad de propiedades en Interior II: %d\n", jugador->cont[7]);
    printf("Cantidad de propiedades en Viña del Mar: %d\n", jugador->cont[8]);
    printf("Cantidad de propiedades en Norte Viña: %d\n", jugador->cont[9]);
    printf("====================================\n");
}

TipoJugador *elegir_jugador_para_intercambio(List *opciones){
    TipoJugador *aux;
    char respuesta;

    aux = list_first(opciones);
    while(respuesta != 'S' && respuesta != 's'){
        imprimirDetallesParaIntercambio(aux);
        printf("\n¿Deseas elegir a este jugador? (s/n): \n");
        scanf(" %c", &respuesta);
        getchar();
        limpiar_pantalla();

        if(respuesta != 'S' && respuesta != 's') 
            aux = next_circular(opciones);
    }
    return aux;
}

void eleccion_de_propiedades(List *propiedadesPosibles, List *propiedadesPedidas){
    char respuesta;
    
    printf("\nPresiona enter para continuar a elegir propiedades...\n");
    presioneEnter();

    TipoCasilla *propiedad = list_first(propiedadesPosibles);
    if(propiedad == NULL) {
        printf("Este jugador no posee propiedades\n");
        return;
    }

    while(respuesta != 'N' && respuesta != 'n'){
        limpiar_pantalla();
        mostrarDetalles(propiedad);
        printf("¿Deseas elegir esta propiedad? (s/n): ");
        scanf(" %c", &respuesta);
        getchar();

        if(respuesta == 'S' || respuesta == 's')
            list_pushBack(propiedadesPedidas, propiedad);

        printf("¿Deseas continuar eligiendo propiedades? (s/n): ");
        scanf(" %c", &respuesta);
        getchar();

        if(respuesta == 'S' || respuesta == 's'){
            propiedad = list_next(propiedadesPosibles);
            if(propiedad == NULL){
                printf("\nEl jugador no posee más propiedades\n");
                break;
            }
        }
    }
    printf("\nPresione enter para continuar...\n");
    presioneEnter();
    limpiar_pantalla();
}

void fase_de_eleccion(TipoJugador *elegido, int *dineroPedido, List *propiedadesPedidas){
    printf("Ahora debes elegir lo que quieres de %s\n\n", elegido->nombre_jugador);
    do{

    printf("¿Cuanto dinero deseas de %s?: \n", elegido->nombre_jugador);
    scanf(" %d", dineroPedido);
    getchar();
        
    if(*dineroPedido < 0 || *dineroPedido > elegido->dinero)
        printf("\nNo puedes elegir esa cantidad de dinero...\n Vuelva a intentarlo\n");
    } while(*dineroPedido < 0 || *dineroPedido > elegido->dinero);


    printf("\n¿Que propiedades deseas de %s?\n", elegido->nombre_jugador);
    eleccion_de_propiedades(elegido->propiedades, propiedadesPedidas);
}

void fase_de_ofrecer(TipoJugador *jugador, int *dineroOfrecido, List *propiedadesOfrecidas){
    printf("Ahora debes elegir lo que le ofreceras para el intercambio\n\n");

    do{
    printf("¿Cuanto dinero deseas ofrecer: \n");
    scanf(" %d", dineroOfrecido);
    getchar();
    if(*dineroOfrecido < 0 || *dineroOfrecido > jugador->dinero)
        printf("\nNo puedes elegir esa cantidad de dinero...\n Vuelva a intentarlo\n");
    } while(*dineroOfrecido < 0 || *dineroOfrecido > jugador->dinero);

    printf("\n¿Que propiedades ofreceras?\n");
    eleccion_de_propiedades(jugador->propiedades, propiedadesOfrecidas);
}

void mostrar_detalles_intercambio(int dineroPedido, List *propiedadesPedidas, int dineroOfrecido, List *propiedadesOfrecidas){
    printf("\nDinero pedido: %d\n\n", dineroPedido);
    printf("Se han solicitado las siguientes propiedades: \n");
    TipoCasilla *propiedad = list_first(propiedadesPedidas);
    if(propiedad == NULL) printf("-\n\n");
    while(propiedad != NULL){
        mostrarDetalles(propiedad);
        propiedad = list_next(propiedadesPedidas);
    }
    printf("====================================================\n");
    printf("\nDinero ofrecido: %d\n\n", dineroOfrecido);
    printf("Se han ofrecido las siguientes propiedades: \n");
    propiedad = list_first(propiedadesOfrecidas);
    if(propiedad == NULL) printf("-\n\n");
    while(propiedad != NULL){
        mostrarDetalles(propiedad);
        propiedad = list_next(propiedadesOfrecidas);
    }
}

void cambios_de_dinero(TipoJugador *jugador, TipoJugador *elegido, int dineroPedido, int dineroOfrecido){
    jugador->dinero += dineroPedido;
    jugador->dinero -= dineroOfrecido;

    elegido->dinero -= dineroPedido;
    elegido->dinero += dineroOfrecido;
}

void cambios_de_propiedad(TipoJugador *jugador, TipoJugador *elegido, List *propiedadesPedidas, List *propiedadesOfrecidas){
    TipoCasilla *propiedadPedida = list_first(propiedadesPedidas);
    while(propiedadPedida != NULL){
        propiedadPedida->propietario = jugador;
        jugador->cont[propiedadPedida->indiceSector]++;
        elegido->cont[propiedadPedida->indiceSector]--;
        list_pushFront(jugador->propiedades, propiedadPedida);
        propiedadPedida = list_next(propiedadesPedidas);
    }

    TipoCasilla *propiedadOfrecida = list_first(propiedadesOfrecidas);
    while(propiedadOfrecida != NULL){
        propiedadOfrecida->propietario = elegido;
        elegido->cont[propiedadOfrecida->indiceSector]++;
        jugador->cont[propiedadOfrecida->indiceSector]--;
        list_pushFront(elegido->propiedades, propiedadOfrecida);
        propiedadOfrecida = list_next(propiedadesOfrecidas);
    }
}

void actualizar_listas_propiedades(TipoJugador *jugador, TipoJugador *elegido){
    TipoCasilla *aux1 = list_first(elegido->propiedades);
    while(aux1 != NULL){
        if(strcmp(aux1->propietario->nombre_jugador, elegido->nombre_jugador) != 0){
            list_popCurrent(elegido->propiedades);
            aux1 = list_current(elegido->propiedades);
        }
        else
            aux1 = list_next(elegido->propiedades);
    }

    TipoCasilla *aux2 = list_first(jugador->propiedades);
    while(aux2 != NULL){
        if(strcmp(aux2->propietario->nombre_jugador, jugador->nombre_jugador) != 0){
            list_popCurrent(jugador->propiedades);
            aux2 = list_current(jugador->propiedades);
        }
        else
            aux2 = list_next(jugador->propiedades);
    }
}

void ejecutarIntercambio(TipoJugador *jugador, TipoJugador *elegido, int dineroPedido, int dineroOfrecido, List *propiedadesPedidas, List *propiedadesOfrecidas){
    cambios_de_dinero(jugador, elegido, dineroPedido, dineroOfrecido);
    cambios_de_propiedad(jugador, elegido, propiedadesPedidas, propiedadesOfrecidas);
    actualizar_listas_propiedades(jugador, elegido);
}


void menu_de_intercambio(TipoJugador *jugador, PartidaGlobal *partida){
    char respuesta;
    int dineroPedido;
    int dineroOfrecido;

    limpiar_pantalla();
    printf("Bienvenido al menú de intercambio\n\n");
    printf("Debes elegir con que jugador deseas intercambiar\n\n");
    printf("Presiona enter para continuar a elegir...\n");
    presioneEnter();
    limpiar_pantalla();
    
    List *opciones = list_create();
    rellenar_lista_opciones_intercambio(opciones, partida->jugadores);
    TipoJugador *elegido = elegir_jugador_para_intercambio(opciones);
    list_clean(opciones);

    List *propiedadesPedidas = list_create();
    fase_de_eleccion(elegido, &dineroPedido, propiedadesPedidas);

    printf("\nPresiona enter para avanzar a la siguiente fase del intercambio\n");
    presioneEnter();
    limpiar_pantalla();

    List *propiedadesOfrecidas = list_create();
    fase_de_ofrecer(jugador, &dineroOfrecido, propiedadesOfrecidas);

    printf("====================================================\n");
    printf("\n%s se te ha propuesto el siguiente intercambio: \n\n", elegido->nombre_jugador);
    printf("====================================================\n");

    mostrar_detalles_intercambio(dineroPedido, propiedadesPedidas, dineroOfrecido, propiedadesOfrecidas);
    
    printf("====================================================\n");
    printf("\n%s ¿Deseas aceptar el intercambio? (s/n): ", elegido->nombre_jugador);
    scanf(" %c", &respuesta);
    getchar();
    
    if(respuesta == 'S' || respuesta == 's'){
        ejecutarIntercambio(jugador, elegido, dineroPedido, dineroOfrecido, propiedadesPedidas, propiedadesOfrecidas);
        printf("\n¡EL INTERCAMBIO FUE ACEPTADO!\n");
    }
    else printf("\n¡EL INTERCAMBIO FUE RECHAZADO!\n");
    
    list_clean(propiedadesPedidas);
    list_clean(propiedadesOfrecidas);
    
    printf("\nPresione enter para salir de este menú...\n");
    presioneEnter();
}
//Visual
void inicializarMapa(int mapa[SIZE][SIZE]) {
    // Inicializar la matriz con -1 para identificar las celdas vacías
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            mapa[i][j] = -1;
        }
    }

    // Numerar el perímetro de la matriz
    int contador = 0;
    // Parte superior
    for (int i = 0; i < SIZE; i++) { 
        mapa[0][i] = contador++;
    }
    // Parte derecha
    for (int i = 1; i < SIZE; i++) {
        mapa[i][SIZE - 1] = contador++;
    }
    // Parte inferior
    for (int i = SIZE - 2; i >= 0; i--) {
        mapa[SIZE - 1][i] = contador++;
    }
    // Parte izquierda
    for (int i = SIZE - 2; i > 0; i--) {
        mapa[i][0] = contador++;
    }
}

void visualizarMapa(int mapa[SIZE][SIZE]) {
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║                 MAPA DEL JUEGO                   ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (mapa[i][j] == -1) {
                printf("   ");  // Espacio en blanco para celdas vacías
            } else {
                printf("%02d ", mapa[i][j]);  // Imprimir número con dos dígitos
            }
        }
        printf("\n");
    }
    printf("\n");
}

//FIANZA
void pagarFianza(TipoJugador* jugador, int monto_fianza) {
    // Verificar si el jugador tiene suficiente dinero para pagar la fianza
    if (jugador->dinero >= monto_fianza) {
        jugador->dinero -= monto_fianza;
        jugador->penalizacion = 0; // Eliminar la penalización, sacando al jugador de la cárcel
    printf("\nHas pagado una fianza de %d y has salido de la cárcel.\n\n", monto_fianza);
    } else {
        printf("\nNo tienes suficiente dinero para pagar la fianza de %d.\n", monto_fianza);
    }
    presioneEnter();
}

// Función para manejar el turno de cada jugador
void turnoJugador(TipoJugador** jugador, PartidaGlobal *partida, int mapa[SIZE][SIZE]){

    limpiar_pantalla();
    printf("%s ES TU TURNO!\n\n", (*jugador)->nombre_jugador);

    int cont_dobles = 0;
    do{
        visualizarMapa(mapa);
        // Se muestra el estado del jugador
        mostrar_estado_jugador(*jugador, partida);
        
        // Se comprueba si está en la cárcel
        if((*jugador)->penalizacion > 0){
            printf("¡OH NO!\n\n");
            printf("Estás en la cárcel, tienes %d turnos de penalización restantes\n\n", (*jugador)->penalizacion);

            char opcion;
            printf("¿Quieres pagar una fianza de 500 para salir de la cárcel? (s/n): ");
            scanf(" %c", &opcion);
            if (opcion == 's' || opcion == 'S') {
                pagarFianza(*jugador, 500);
                if((*jugador)->penalizacion == 0) continue; // Si paga la fianza, continúa su turno normalmente
            } else {
                (*jugador)->penalizacion--;
                return;
            }
        }
        
        // Se lanzan los dados
        printf("Presiona enter para lanzar los dados!\n");
        presioneEnter();
        
        limpiar_pantalla();
        int dado1 = tirar_dado();
        int dado2 = tirar_dado();
        int totalDados = dado1 + dado2;
    
        printf("¡Haz lanzado los dados: %d y %d (Total: %d)!\n\n", dado1, dado2, totalDados);
    
        if(dado1 == dado2){
            cont_dobles++;
            printf("Haz obtenido dobles!\n\n");
    
            if(cont_dobles != 3) printf("Ganas un turno extra!\n\n");
            else {
                printf("¡Oh no!\n\n");
                printf("¡Haz obtenido tres dobles seguidos! Pierdes tu turno y vas a la cárcel!\n\n");
                (*jugador)->posicion = 10;
                (*jugador)->penalizacion = 3;
                return;
            }
        } else cont_dobles = 0;
    
        // Se mueve al jugador
        moverJugador(*jugador, totalDados);
    
        // Se obtiene su casilla actual
        TipoCasilla *casillaActual = partida->tablero[(*jugador)->posicion];
        
        // Se ejecuta la acción de la casilla
        ejecutarAccionCasilla(*jugador, casillaActual, partida, totalDados);

        if((*jugador)->penalizacion > 0){
            printf("\n¡OH NO! Estas en la cárcel\n");
            printf("\nTu turno acaba aquí\n");
            return;
        }
    
        char opcion;

        do{
            limpiar_pantalla();
            visualizarMapa(mapa);
            mostrar_estado_jugador(*jugador, partida);
            mostrarMenuDeTurno();
            scanf(" %c", &opcion);
            getchar();
            switch(opcion){
                case '1':
                    menu_de_propiedades(*jugador);
                    break;
                case '2':
                    menu_de_intercambio(*jugador, partida);
                    break;
                case '3':
                    printf("\nAcabando turno ...\n\n");
                    break;
                case '4':
                    eliminarJugador(jugador, partida);
                    return;
                default:
                    printf("\nOpción inválida. Por favor, selecciona una opción válida.\n\n");
                    break;
            }
        } while(opcion != '3' && opcion != '4');
        
        
        if(cont_dobles == 0) break;
        printf("Presiona enter para ir a tu siguiente turno\n");
        presioneEnter();
        limpiar_pantalla();
        printf("¡TURNO ADICIONAL!\n");
    } while(cont_dobles < 3);
}

// Función para verificar si hay un ganador
TipoJugador *verificarGanador(List *jugadores){
    // Si hay más de un jugador en pie, se retorna NULL
    if(list_size(jugadores) != 1){
        return NULL;
    }
    // Si queda solo 1, se retorna el jugador
    return list_first(jugadores);
}

// Función para mostrar las reglas del juego
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

void liberarMemoria(PartidaGlobal *partida){
    list_clean(partida->jugadores);
    queue_clean(partida->fortuna);
    queue_clean(partida->arca_comunal);
    free(partida);
}

// Función que inicia la partida
void iniciarPartida(){

    // Se reserva memoria
    PartidaGlobal *partida =(PartidaGlobal*)malloc(sizeof(PartidaGlobal));
    if(partida == NULL){
        printf("Error al inicializar la partida\n");
        return;
    }

    // Se inicia la aleatoriedad
    inicializar_aleatoriedad();
    // Se inician los campos necesarios
    inicializarCamposNecesarios(partida);

    // Se ingresan los jugadores
    int num_jugadores = solicitar_jugadores();
    asignar_jugadores(partida, num_jugadores);

    // Se obtiene el primer jugador de la lista
    TipoJugador* jugador_actual = (TipoJugador*)list_first(partida->jugadores);

    //MAPA
    int mapa[SIZE][SIZE];
    inicializarMapa(mapa);
    // Ciclo de juego
    while(true)
    {
        turnoJugador(&jugador_actual, partida, mapa);
        printf("Presione enter para continuar\n");
        presioneEnter();
        
        if(jugador_actual != NULL)
            jugador_actual = next_circular(partida->jugadores);
        else 
            jugador_actual = list_current(partida->jugadores);

        TipoJugador* ganador = verificarGanador(partida->jugadores);
        if (ganador != NULL) {
            printf("¡El jugador %s es el ganador del juego!\n", ganador->nombre_jugador);
            printf("\nPresiona enter para salir\n");
            presioneEnter();
            limpiar_pantalla();
            break;
        }
    }
    liberarMemoria(partida);
}
