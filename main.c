#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <string.h>
#include "tdas/list.h"
#include "tdas/queue.h"

//STRUCTS
// Declarar los datos de los jugadores
typedef struct {
    int dinero;                    // Dinero del jugador
    int penalizacion;              // Penalizacion de la carcel hacia al jugador
    char nombre_jugador[40];       // Nombre del jugador
    int posicion;                  // Posicion del jugador
    List *propiedades;    
    List *cartas;
}TipoJugador;

typedef struct  {
    char nombre[50];
    bool esPropiedad;
    TipoJugador* propietario; //REEMPLAZAR VOID* POR TIPO JUGADOR
    int precio;
    int renta_base;
    int casas;
    int precio_casa;
    char sector[50];
    bool hipotecado;
}TipoPropiedad; 


typedef struct{
    char *descripcion;
    void *efecto;
}TipoCarta;


// Declarar los datos de las casillas
typedef struct {
    char nombre_casilla[40];       // Nombre de la casilla
    char tipo[5];                 // Tipo de casilla "P" para propiedad y "C" para carta
    union {
        TipoPropiedad *propiedad;    
        int cantidad_impuesto;
        TipoCarta* carta;              
    };
}TipoCasilla;

typedef struct {
     TipoCasilla *casillas[40]; // Arreglo de casillas del tablero
}TipoTablero;

typedef struct {
    TipoTablero *tablero;
    List *jugadores;
    //circle_list *turnos;
    Queue* fortuna;
    Queue* arca_comunal;
}partidaGlobal;

//INICIACIÓN DEL JUEGO
void inicializar_propiedades( TipoPropiedad propiedades[]) {
    // Inicialización de propiedades
     TipoPropiedad propiedades_iniciales[] = {
        {"TORPEDERAS", true, NULL, 600, 40, 0, 500, "PLAYA ANCHA", false},
        {"PUCV SEDE ALIMENTOS", true, NULL, 650, 85, 0, 500, "PLAYA ANCHA", false},
        {"ESTACIÓN PUERTO", false, NULL, 2000, 500, 0, 0, "METRO", false},
        {"PLAZA VICTORIA", true, NULL, 1000, 120, 0, 500, "VALPARAÍSO", false},
        {"PUCV IBC", true, NULL, 1100, 150, 0, 500, "VALPARAÍSO", false},
        {"AV. PEDRO MONT", true, NULL, 1200, 160, 0, 500, "VALPARAÍSO", false},
        {"LAGUNA VERDE", true, NULL, 1400, 200, 0, 1000, "CURAUMA", false},
        {"PUCV CAMPUS CURAUMA", true, NULL, 1500, 235, 0, 1000, "CURAUMA", false},
        {"LAGO PEÑUELAS", true, NULL, 1650, 250, 0, 1000, "CURAUMA", false},
        {"ESTACIÓN BARÓN", false, NULL, 2000, 500, 0, 0, "METRO", false},
        {"MUELLE BARÓN", true, NULL, 1800, 280, 0, 1000, "COSTA VALPO", false},
        {"CALETA PORTALES", true, NULL, 1900, 290, 0, 1000, "COSTA VALPO", false},
        {"PUCV ESCUELA ECONOMÍA", true, NULL, 2000, 320, 0, 1000, "COSTA VALPO", false},
        {"PUCV SEDE MECÁNICA", true, NULL, 2200, 375, 0, 1500, "INTERIOR", false},
        {"QUILPUE", true, NULL, 2400, 400, 0, 1500, "INTERIOR", false},
        {"VILLA ALEMANA", true, NULL, 2400, 400, 0, 1500, "INTERIOR", false},
        {"ESTACIÓN LIMACHE", false, NULL, 2000, 500, 0, 0, "METRO", false},
        {"LIMACHE", true, NULL, 2600, 440, 0, 1500, "INTERIOR II", false},
        {"QUILLOTA", true, NULL, 2600, 440, 0, 1500, "INTERIOR II", false},
        {"PUCV FACU. AGRONOMÍA", true, NULL, 2800, 480, 0, 1500, "INTERIOR II", false},
        {"MUELLE VERGARA", true, NULL, 3000, 520, 0, 2000, "VIÑA DEL MAR", false},
        {"PUCV SEDE SAUSALITO", true, NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", false},
        {"15 NORTE", true, NULL, 3200, 560, 0, 2000, "VIÑA DEL MAR", false},
        {"ESTACIÓN MIRAMAR", false, NULL, 2000, 500, 0, 0, "METRO", false},
        {"REÑACA", true, NULL, 3500, 700, 0, 2000, "NORTE VIÑA", false},
        {"DUNAS DE CONCON", true, NULL, 4000, 1000, 0, 2000, "NORTE VIÑA", false},
        {"ESVAL (AGUA)", false, NULL, 2000, 50, 0, 0, "COMPAÑIAS", false},
        {"CHILQUINTA (LUZ)", false, NULL, 2000, 50, 0, 0, "COMPAÑIAS", false}
    };

    // Copiar los datos iniciales a las propiedades pasadas como argumento
    int num_propiedades = sizeof(propiedades_iniciales) / sizeof(propiedades_iniciales[0]);
    for (int i = 0; i < num_propiedades; i++) {
        propiedades[i] = propiedades_iniciales[i];
    }
}


//Función para limpiar la pantalla
void limpiar_pantalla() {
    system("clear");
}

void presioneEnter() {
    printf("Presione Enter para continuar...\n\n");
    getchar();
    // Limpiar BUFFER
    while (getchar() != '\n');





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
    printf("7. El juego termina cuando solo queda un jugador con dinero y propiedades. O cuando hayan pasado un determinado número de turnos, cuando esto pase, el jugador que haya más acumulado más dinero entre su plata actual y el valor de sus propiedades, ganará.\n");
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
                    propiedad->renta_base,
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


int main() {
    int opcion;

    // Loop del menú inicial
    do {
        mostrarMenuInicial();
        scanf("%d", &opcion);
        switch(opcion) {
            case 1:
                // Lógica para iniciar una nueva partida
                limpiar_pantalla();
                printf("Iniciando nueva partida...\n");

                // Inicializamos las propiedades
                TipoPropiedad propiedades[30]; // Asegúrate de que el tamaño sea suficiente para todas las propiedades

                inicializar_propiedades(propiedades);

                break;
            case 2:
                // Lógica para cargar una partida guardada
                limpiar_pantalla();
                printf("Cargando partida...\n");

                break;
            case 3:
                limpiar_pantalla();
                printf("Mostrando reglas del juego...\n\n");
                // Lógica para mostrar las reglas del juego
                mostrar_reglas();
                limpiar_pantalla();
                break;
            case 4:
                limpiar_pantalla();
                printf("Saliendo del juego...\n");

                // Lógica para salir del juego
                break;
            default:
                limpiar_pantalla();
                printf("Opción no válida. Por favor, seleccione una opción válida.\n");
        }
    } while(opcion != 4); // Salir del loop si se selecciona la opción 4 (Salir del juego)
    return 0;
}
