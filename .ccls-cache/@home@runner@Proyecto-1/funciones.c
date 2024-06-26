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

// STRUCTS

// Struct para cada jugador
struct TipoJugador{
    int dinero;                    // Dinero del jugador
    int penalizacion;              // Penalizacion de la carcel hacia al jugador
    char nombre_jugador[40];       // Nombre del jugador
    int posicion;                  // Posicion del jugador
    List *propiedades;             // Lista de las propiedades del jugador
    int cant_metros;                // Cantidad de propiedades de tipo Metro
    int cant_servicios;             // Cantidad de propiedades del tipo Servicio
    int playa_ancha;               // Cantidad de propiedades del sector "Playa Ancha"
    int valparaiso;                // Cantidad de propiedades del sector "Valparaiso"
    int curauma;                   // Cantidad de propiedades del sector "Curauma"
    int costa_valpo;               // Cantidad de propiedades del sector "Costa Valpo"
    int interior;                  // Cantidad de propiedades del sector "Interior"
    int interiorII;                // Cantidad de propiedades del sector "Interior II"
    int vina_del_mar;              // Cantidad de propiedades del sector "Viña del Mar"
    int norte_vina;                // Cantidad de propiedades del sector "Norte Viña"
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
    {"AVANCE A DUNAS DE CONCON", 0, 38, -1},
    {"EL BANCO PAGA DIVIDENDOS DE $500", 500, -1, -1},
    {"AVANCE A ESCUELA ECONOMÍA PUCV", 0, 19, -1},
    {"VAYA DIRECTAMENTE A LA CÁRCEL", 0, 10, 3},
    {"USTED HA SIDO ELEGIDO PRESIDENTE DEL CENTRO DE ALUMNOS, PAGUE $1000", -1000, -1, -1},
    {"ENHORABUENA, HAN CARGADO LA BAES! RECIBA $1500.", 1500, -1, -1},
    {"AVANCE A ESTACIÓN MIRAMAR", 0, 34, -1},
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
    {"SALIDA", 'E', NULL, 0, 0, 0, 0, "SALIDA", -1, false, 0},
    {"TORPEDERAS", 'P', NULL, 600, 40, 0, 500, "PLAYA ANCHA", -1, false, 1},
    {"ARCA COMUNAL", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, false, 2},
    {"SEDE ALIMENTOS PUCV", 'P', NULL, 800, 65, 0, 500, "PLAYA ANCHA", -1, false, 3},
    {"SERVICIOS IMPUESTOS INTERNOS", 'I', NULL, 2000, 0, 0, 0, "PAGAR", -1, false, 4},
    {"ESTACIÓN PUERTO", 'M', NULL, 2000, 250, 0, 0, "METRO", -1, false, 5},
    {"PARQUE ITALIA", 'P', NULL, 1000, 120, 0, 500, "VALPARAÍSO", -1, false, 6},
    {"FORTUNA", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, false, 7},
    {"IBC PUCV", 'P', NULL, 1100, 150, 0, 500, "VALPARAÍSO", -1, false, 8},
    {"AV. PEDRO MONT", 'P', NULL, 1200, 160, 0, 500, "VALPARAÍSO", -1, false, 9},
    {"CARCEL", 'N', NULL, 0, 0, 0, 0, "NEUTRO", false, 10},
    {"LAGUNA VERDE", 'P', NULL, 1400, 200, 0, 1000, "CURAUMA", -1, false, 11},
    {"CAMPUS CURAUMA PUCV", 'P', NULL, 1500, 235, 0, 1000, "CURAUMA", -1, false, 12},
    {"ESVAL (COMPANIA AGUA)", 'S', NULL, 1500, 50, 0, 0, "COMPAÑIAS", -1, false, 13},
    {"LAGO PEÑUELAS", 'P', NULL, 1650, 250, 0, 1000, "CURAUMA", -1, false, 14},
    {"ESTACIÓN BARÓN", 'M', NULL, 2000, 250, 0, 0, "METRO", -1, false, 15},
    {"MUELLE BARÓN", 'P', NULL, 1800, 280, 0, 1000, "COSTA VALPO", -1, false, 16},
    {"CALETA PORTALES", 'P', NULL, 1900, 290, 0, 1000, "COSTA VALPO", -1, false, 17},
    {"ARCA COMUNAL", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, false, 18},
    {"ESCUELA ECONOMÍA PUCV", 'P', NULL, 2000, 320, 0, 1000, "COSTA VALPO", -1, false, 19},
    {"RELOJ DE FLORES", 'N', NULL, 0, 0, 0, 0, "NEUTRO", -1, false, 20},
    {"SEDE MECÁNICA PUCV", 'P', NULL, 2200, 375, 0, 1500, "INTERIOR", -1, false, 21},
    {"QUILPUE", 'P', NULL, 2400, 400, 0, 1500, "INTERIOR", -1, false, 22},
    {"FORTUNA", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, false, 23},
    {"VILLA ALEMANA", 'P', NULL, 2400, 400, 0, 1500, "INTERIOR", -1, false, 24},
    {"ESTACIÓN LIMACHE", 'M', NULL, 2000, 250, 0, 0, "METRO", -1, false, 25},
    {"OLMUE", 'P', NULL, 2600, 440, 0, 1500, "INTERIOR II", -1, false, 26},
    {"QUILLOTA", 'P', NULL, 2600, 440, 0, 1500, "INTERIOR II", -1, false, 27},
    {"CHILQUINTA (COMPANIA LUZ)", 'S', NULL, 1500, 50, 0, 0, "COMPAÑIAS", -1, false, 28},
    {"FACU. AGRONOMÍA PUCV", 'P', NULL, 2800, 480, 0, 1500, "INTERIOR II", -1, false, 29},
    {"VAYA A CARCEL", 'J', NULL, 0, 0, 0, 0, "CARCEL", -1, false, 30},
    {"MUELLE VERGARA", 'P', NULL, 3000, 520, 0, 2000, "VIÑA DEL MAR", -1, false, 31},
    {"SEDE SAUSALITO PUCV", 'P', NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", -1, false, 32},
    {"ARCA COMUNAL", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, false, 33},
    {"15 NORTE", 'P', NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", -1, false, 34},
    {"ESTACIÓN MIRAMAR", 'M', NULL, 2000, 250, 0, 0, "METRO", -1, false, 35},
    {"RENACA", 'P', NULL, 3500, 700, 0, 2000, "NORTE VIÑA", -1, false, 36},
    {"FORTUNA", 'C', NULL, 0, 0, 0, 0, "CARTAS", -1, false, 37},
    {"MC DONALD", 'I', NULL, 1000, 0, 0, 0, "PAGAR", -1, false, 38},
    {"DUNAS DE CONCON", 'P', NULL, 4000, 1000, 0, 2000, "NORTE VIÑA", -1, false, 39}};
    

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

//Funcion para solicitar la cantidad de jugadores
int solicitar_jugadores()
{
    // Creación de variable que contiene los jugadores
    int num_jugadores;
    do {
        printf("Ingrese el número de jugadores (2 a 4): ");
        scanf("%d", &num_jugadores); // Se leen los jugadores
        while (getchar() != '\n'); // Limpiar el buffer de entrada  
        if (num_jugadores < 2 || num_jugadores > 4)
            printf("El número de jugadores debe estar entre 2 y 4.\n");
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
            
            printf("¿Quieres hipotecar la propiedad %s por %d? (s/n): ", propiedad->nombre, valor_hipoteca);
            char respuesta;
            scanf(" %c", &respuesta);
            getchar();
            // Si la respuesta es afirmativa se hipoteca la propiedad
            if (respuesta == 's' || respuesta == 'S')
            {
                propiedad->hipotecado = true;
                jugador->dinero += valor_hipoteca;
                printf("Felicidades %s, has hipotecado la propiedad %s y has recibido %d!\n", jugador->nombre_jugador, propiedad->nombre, valor_hipoteca);

                printf("Recuerda que no podrás cobrar renta por %s mientras esté hipotecada.\n", propiedad->nombre);
            }
            else // Si la respuesta es negativa se cancela la hipoteca
            {
                printf("Haz decidido no hipotecar la propiedad %s.\n", propiedad->nombre);
                return;
            }
        }
        else // Si la propiedad ya esta hipotecada, se da un aviso
        {
            printf("La propiedad %s ya está hipotecada.\n", propiedad->nombre);
        }
    }
    else // Si la propiedad no es del jugador, se da un aviso
    {
        printf("La propiedad %s no está a tu nombre.\n", propiedad->nombre);
    }
}

// Función para recuperar una propiedad hipotecada
void recuperar_propiedad_hipotecada(TipoJugador *jugador, TipoCasilla *propiedad)
{
    // Se comprueba que este hipotecada y que sea del jugador
    if (propiedad->hipotecado && propiedad->propietario == jugador) {
        // Se determina el precio de la recuperación
        int precio_venta = ((propiedad->precio / 2) + ((propiedad->precio_casa * propiedad->casas) /2)) + (precio_venta * 0.1);
        
        // Preguntar al jugador si desea recuperar la propiedad
        printf("¿Quieres recuperar la propiedad %s por %d? (s/n): ", propiedad->nombre, precio_venta);
        char respuesta;
        scanf(" %c", &respuesta);
        getchar();

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
        } else { // Si la respuesta es negativa se cancela la recuperación
            printf("No se ha recuperado la propiedad %s.\n", propiedad->nombre);
        }

    } else {
        printf("Esta propiedad no esta hipotecada, o no es tuya.\n");
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

// Función para comprar casas en una propiedad
void comprar_casas(TipoJugador *jugador, TipoCasilla *propiedad) {
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
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║               ESTADO DEL JUGADOR                 ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("════════════════════════════════════════════════════");
    printf("\nJugador: %s\n", jugador->nombre_jugador);
    printf("Dinero actual: %d\n", jugador->dinero);
    printf("Turnos de penalización en cárcel: %d\n", jugador->penalizacion);
    printf("Posición en el tablero: %d\n", jugador->posicion);
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

// Función para el menu de decisión sobre propiedades
void menu_de_propiedades(TipoJugador *jugador, TipoCasilla *propiedad){
    char opcion;

    do{
        printf("¿Qué acción deseas realizar?\n");
        printf("1. Comprar casas\n");
        printf("2. Vender casas\n");
        printf("3. Continuar\n");
        scanf(" %c", &opcion);
        getchar();
        switch(opcion){
            case '1':
                comprar_casas(jugador, propiedad);
                break;
            case '2':
                vender_casas(jugador, propiedad);
                break;
            case '3':
                return;
            default:
                printf("Opción ingresada no es válida\n");
                break;
        }
    } while(opcion != '3');
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
    if(propiedad->precio_casa > 0){
        printf("Precio por casa: $%d\n", propiedad->precio_casa);
        printf("Numero de casas: %d\n", propiedad->casas);
    }
    printf("Posición: %d\n\n", propiedad->num_casilla);
}



// Función para manejar cuando un jugador cae en una propiedad
void casoPropiedad(TipoJugador *jugador, TipoCasilla* propiedad, PartidaGlobal *partida){
    // Verificar que la propiedad no tiene dueño
    if (propiedad->propietario == NULL) {
        // Verificar que el jugador tiene suficiente dinero para comprar la
        // propiedad
        printf("\nDinero Actual: %d\n", jugador->dinero);
        imprimirDetallesPropiedad(propiedad);  
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

            // Agregar la propiedad a la lista de propiedades del jugador
            list_pushFront(jugador->propiedades, propiedad);
            printf("\nFelicidades %s, has comprado la propiedad %s por %d!\n",
                   jugador->nombre_jugador, propiedad->nombre, propiedad->precio);

          } else {
            printf("\nHaz decidido no comprar la propiedad %s.\n", propiedad->nombre);
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

void imprimirDetallesMetro(TipoCasilla* propiedad) {
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║            DETALLES DE LA ESTACIÓN               ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("Nombre: %s\n", propiedad->nombre);
    printf("Precio: $%d\n", propiedad->precio);
    printf("Renta base: $%d\n", propiedad->renta);
    printf("*El valor de la renta se duplica según la cantidad de metros que controlas*\n\n");
}

// Función para manejar cuando un jugador cae en una 
// casilla metro
void casoMetro(TipoJugador *jugador, TipoCasilla* propiedad, PartidaGlobal *partida) {
    // Verificar que la propiedad no tiene dueño
    if (propiedad->propietario == NULL) {
        // Verificar que el jugador tiene suficiente dinero para comprar la propiedad
        if (jugador->dinero >= propiedad->precio) {
            char respuesta;
            printf("\nDinero Actual: %d", jugador->dinero);
            imprimirDetallesMetro(propiedad);  // Asumiendo que esta función está definida
            printf("¿Quieres comprar %s por %d? (s/n): ", propiedad->nombre, propiedad->precio);
            scanf(" %c", &respuesta);

            if (respuesta == 's' || respuesta == 'S') {
                // Reducir el dinero del jugador en el precio de la propiedad
                jugador->dinero -= propiedad->precio;

                // Asignar la propiedad al jugador
                propiedad->propietario = jugador;

                // Agregar la propiedad a la lista de propiedades del jugador
                list_pushFront(jugador->propiedades, propiedad);  // Asumiendo que esta función está definida
                // limpiar_pantalla();  // Asumiendo que esta función está definida
                printf("Felicidades %s, has comprado %s por %d!\n", jugador->nombre_jugador, propiedad->nombre, propiedad->precio);

                // Incrementar el número de estaciones de metro del jugador
                jugador->cant_metros++;

            } else {
                printf("Haz decidido no comprar %s.\n", propiedad->nombre);
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
            double renta = propiedad->renta * pow(2.25, (propiedad->propietario->cant_metros - 1));        
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

void imprimirDetallesServicio(TipoCasilla* propiedad) {
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║            DETALLES DE EL SERVICIO               ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");
    printf("Nombre: %s\n", propiedad->nombre);
    printf("Precio: $%d\n", propiedad->precio);
    printf("Renta 1 Servicio: 60 veces la suma de los dados\n");
    printf("Renta 2 Servicios: 150 veces la suma de los dados\n\n");
}


void casoServicio(TipoJugador* jugador, TipoCasilla* propiedad, PartidaGlobal* partida, int dados) {
    // Verificar que la propiedad no tiene dueño
    if (propiedad->propietario == NULL) {
        // Verificar que el jugador tiene suficiente dinero para comprar la propiedad
        if (jugador->dinero >= propiedad->precio) {
            char respuesta;
            printf("\nDinero Actual: %d\n", jugador->dinero);
            imprimirDetallesServicio(propiedad);
            printf("¿Quieres comprar la compañía %s por %d? (s/n): ", propiedad->nombre, propiedad->precio);
            scanf(" %c", &respuesta);

            if (respuesta == 's' || respuesta == 'S') {
                // Reducir el dinero del jugador en el precio de la propiedad
                jugador->dinero -= propiedad->precio;

                // Asignar la propiedad al jugador
                propiedad->propietario = jugador;

                // Agregar la propiedad a la lista de propiedades del jugador
                list_pushFront(jugador->propiedades, propiedad);
                limpiar_pantalla();
                printf("Felicidades %s, has comprado la compañía %s por %d!\n", jugador->nombre_jugador, propiedad->nombre, propiedad->precio);
                jugador->cant_servicios += 1;

            } else {
                printf("Has decidido no comprar %s.\n", propiedad->nombre);
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
            if (propiedad->propietario->cant_servicios == 1) {
                int renta_servicio = 60 * dados;
                printf("%s tiene un dueño, debes pagar renta de: %d.\n", propiedad->nombre, renta_servicio);
                verificar_bancarrota(jugador, renta_servicio, partida);
                if (jugador != NULL) {
                    jugador->dinero -= renta_servicio;
                    propiedad->propietario->dinero += renta_servicio;
                }
            } else if (propiedad->propietario->cant_servicios >= 2) {
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
void casoCarta(TipoJugador *jugador, PartidaGlobal *partida, TipoCasilla *casilla)
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
                printf("Pasaste por la salida, cobra $2000\n");
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
        return;
               
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
    else if(casilla->tipo == 'C') casoCarta(jugador, partida, casilla);
    else if(casilla->tipo == 'I') casoImpuestos(jugador, partida, casilla);
    else if(casilla->tipo == 'M')  casoMetro(jugador, casilla, partida);
    else if(casilla->tipo == 'S')  casoServicio(jugador, casilla, partida, dados);

    else if(casilla->tipo == 'J')  casoCarcel(jugador, partida);

    printf("\nPresiona enter para continuar...\n");
    presioneEnter();
    limpiar_pantalla();
}

void mostrarMenuDeTurno(){
    printf("¿Qué acción deseas realizar?\n\n");
    printf("1. Construir o vender casas\n");
    printf("2. Manejar hipotecas de las propiedades\n");
    printf("3. Intercambiar con otros jugadores\n");
    printf("4. Finalizar turno\n");
    printf("5. Declararte en bancarrota\n");
}



// Función para manejar el turno de cada jugador
void turnoJugador(TipoJugador** jugador, PartidaGlobal *partida){

    limpiar_pantalla();
    printf("%s ES TU TURNO!\n\n", (*jugador)->nombre_jugador);

    int cont_dobles = 0;
    do{
        // Se muestra el estado del jugador
        mostrar_estado_jugador(*jugador, partida);
        
        // Se comprueba si está en la cárcel
        if((*jugador)->penalizacion > 0){
            printf("¡OH NO!\n\n");
            printf("Estas en la cárcel, tienes %d turnos de penalización restantes\n\n", (*jugador)->penalizacion);
            (*jugador)->penalizacion --;
            return;
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
        

        
        
        char opcion;

        do{
            limpiar_pantalla();
            mostrar_estado_jugador(*jugador, partida);
            mostrarMenuDeTurno();
            scanf(" %c", &opcion);
            getchar();
            switch(opcion){
                case '1':
                    // Lógica para construir o vender
                    break;
                case '2':
                    // Lógica para manejar hipotecas
                    break;
                case '3':
                    // Lógica para intercambiar
                    break;
                case '4':
                    printf("\nAcabando turno ...\n\n");
                    break;
                case '5':
                    eliminarJugador(jugador, partida);
                    return;

                default:
                    printf("\nOpción inválida. Por favor, selecciona una opción válida.\n\n");
                    break;
            }
        } while(opcion != '4' && opcion != '5');
        
        
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

    // Ciclo de juego
    while(true)
    {
        turnoJugador(&jugador_actual, partida);
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
    //liberarMemoria
}
